#ifndef IRIS_ZZ_NGRAM_DATABASE_HPP
#define IRIS_ZZ_NGRAM_DATABASE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>

#include <iris/ngram/gram.hpp>
#include <iris/ngram/id.hpp>
#include <iris/ngram/search_query.hpp>

#include <iris/default_init_allocator.hpp>
#include <iris/interval.hpp>
#include <iris/format.hpp>

#include <memory>
#include <concepts>
#include <span>
#include <flat_map>
#include <unordered_set>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <compare>
#include <stdexcept>

#include <cassert>

namespace iris::ngram {

struct gram_occurrence
{
    document_id doc_id;
    int pos;

    [[nodiscard]] constexpr bool operator==(gram_occurrence const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram_occurrence const&) const noexcept = default;
};

namespace detail {

enum struct [[nodiscard]] search_continuation : bool
{
    abort = false,
    proceed = true,
};

class posting_list
{
public:
    void append(document_id const doc_id, int pos)
    {
        if (postings_.empty() || postings_.back().doc_id != doc_id) {
            if (!postings_.empty() && postings_.back().doc_id > doc_id) {
                throw std::invalid_argument{"documents must be indexed in non-decreasing order of document ID"};
            }
            postings_.emplace_back(
                doc_id,
                static_cast<unsigned>(positions_.size()),
                0
            );
        }

        ++postings_.back().pos_count;
        positions_.emplace_back(pos);
    }

    void to_occurrence_list(std::vector<gram_occurrence>& occs) const
    {
        occs.clear();
        for (auto const& posting : postings_) {
            for (std::size_t i = posting.pos_offset; i < posting.pos_offset + posting.pos_count; ++i) {
                occs.emplace_back(posting.doc_id, positions_[i]);
            }
        }
    }

    template<class F>
    void for_each_documents(F&& f) const
    {
        static_assert(std::invocable<F, document_id, std::span<int const>>);

        constexpr bool f_returns_continuation = std::same_as<
            std::invoke_result_t<F, document_id, std::span<int const>>,
            search_continuation
        >;

        for (auto const& posting : postings_) {
            std::span<int const> const posting_span{
                positions_.begin() + posting.pos_offset,
                static_cast<std::size_t>(posting.pos_count)
            };

            if constexpr (f_returns_continuation) {
                search_continuation const cont = f(posting.doc_id, posting_span);
                if (cont == search_continuation::abort) break;
            } else {
                f(posting.doc_id, posting_span);
            }
        }
    }

private:
    struct posting_t
    {
        document_id doc_id;
        unsigned pos_offset = 0;
        unsigned pos_count = 0;
    };

    std::vector<posting_t> postings_;
    std::vector<int> positions_;
};

template<std::size_t N, class CharT, class PostingListT = posting_list>
class gram_index
{
    using entry_map = std::flat_map<gram<N, CharT>, std::unique_ptr<PostingListT>>;
    static constexpr std::size_t side_merge_threshold = 2048;

public:
    [[nodiscard]] auto find_list(this auto&& self, gram<N, CharT> const ng)
    {
        if (auto const it = self.gram_entries_.find(ng); it != self.gram_entries_.end()) {
            return it->second.get();
        }
        if (auto const it = self.side_entries_.find(ng); it != self.side_entries_.end()) {
            return it->second.get();
        }
        return static_cast<PostingListT*>(nullptr);
    }

    void find_occurrences(gram<N, CharT> const ng, std::vector<gram_occurrence>& occs) const
    {
        occs.clear();
        auto const* list = this->find_list(ng);
        if (!list) return;
        list->to_occurrence_list(occs);
    }

    template<class F>
    void search(gram<N, CharT> const ng, F&& f) const
    {
        auto const* list = this->find_list(ng);
        if (!list) return;
        list->for_each_documents(f);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return gram_entries_.empty() && side_entries_.empty();
    }

    void clear() noexcept
    {
        gram_entries_.clear();
        side_entries_.clear();
    }

    void merge_new_entries(std::vector<std::pair<gram<N, CharT>, std::unique_ptr<PostingListT>>>& pending)
    {
        if (pending.empty()) return; // vocabulary saturated

        for (auto& [key, pl] : pending) {
            [[maybe_unused]]
            auto const it = side_entries_.try_emplace(
                side_entries_.end(), // hint
                key, std::move(pl)
            );
            assert(it->second != nullptr && pl == nullptr);
        }
        if (side_entries_.size() >= side_merge_threshold) {
            this->flush_side();
        }
    }

private:
    void flush_side()
    {
        if (side_entries_.empty()) return;

        auto [skeys, svalues] = std::move(side_entries_).extract();
        auto [keys, values] = std::move(gram_entries_).extract();

        std::size_t const old_size = keys.size();
        std::size_t const add = skeys.size();
        keys.resize(old_size + add);
        values.resize(old_size + add);

        // Backward merge
        std::size_t out = old_size + add;
        std::size_t i = old_size;
        std::size_t j = add;
        while (j > 0) {
            if (i > 0 && skeys[j - 1] < keys[i - 1]) {
                --out;
                --i;
                keys[out] = keys[i];
                values[out] = std::move(values[i]);
            } else {
                assert(i == 0 || keys[i - 1] < skeys[j - 1]);
                --out;
                --j;
                keys[out] = skeys[j];
                values[out] = std::move(svalues[j]);
            }
        }
        assert(out == i);

        gram_entries_.replace(std::move(keys), std::move(values));
    }

    // Double-buffered to reduce insertion cost
    entry_map gram_entries_, side_entries_;
};

template<std::size_t N, class CharT>
struct gram_pos_t
{
    gram<N, CharT> ng;
    int pos;

    [[nodiscard]] constexpr bool operator==(gram_pos_t const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram_pos_t const&) const noexcept = default;
};

template<class CharT, class PostingListT = posting_list>
struct index_storage
{
    void append_index(document_id const doc_id, std::basic_string_view<CharT> const input)
    {
        this->template append_index<1>(doc_id, input);
        this->template append_index<2>(doc_id, input);
    }

    void clear() noexcept
    {
        uni_data_.clear();
        bi_data_.clear();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return
            this->template get_data<1>().idx.empty() &&
            this->template get_data<2>().idx.empty();
    }

    template<std::size_t N, class F>
    void search(gram<N, CharT> const ng, F&& f) const
    {
        this->template get_data<N>().idx.search(ng, f);
    }

    template<std::size_t N>
    [[nodiscard]] auto& get_index(this auto& self IRIS_LIFETIMEBOUND) noexcept
    {
        return self.template get_data<N>().idx;
    }

private:
    template<std::size_t N>
    struct gram_index_storage
    {
        gram_index<N, CharT, PostingListT> idx;

        // Caches
        std::vector<gram_pos_t<N, CharT>, default_init_allocator<gram_pos_t<N, CharT>>>
        batch_grams;

        std::vector<std::pair<gram<N, CharT>, std::unique_ptr<PostingListT>>>
        batch_pending;

        void clear() noexcept
        {
            idx.clear();
            batch_grams.clear();
            batch_pending.clear();
        }
    };

    gram_index_storage<1> uni_data_;
    gram_index_storage<2> bi_data_;

    template<std::size_t N>
    [[nodiscard]] auto& get_data(this auto& self IRIS_LIFETIMEBOUND) noexcept
    {
        if constexpr (N == 1) {
            return self.uni_data_;
        } else if constexpr (N == 2) {
            return self.bi_data_;
        } else {
            static_assert(false);
        }
    }

    template<std::size_t N>
    void append_index(
        document_id const doc_id,
        std::basic_string_view<CharT> const input
    )
    {
        if (input.size() < N) return;
        gram_index_storage<N>& data = this->template get_data<N>();

        // Naive per-gram insertion into flat_map is expensive: each *new* key
        // shifts the underlying vectors, so building an index of vocabulary
        // size V costs O(V^2) overall. Instead, per document:
        //
        //   1. Collect grams+positions --- O(G)        G = grams in this doc
        //   2. Sort them ----------------- O(G log G)
        //   3. Existing keys ------------- O(D log V)  D = distinct grams (D <= G)
        //   4. New keys ------------------ O(V + P)    P = brand-new keys (P <= D)
        //
        // Once the vocabulary saturates (P ~ 0, typical after a few documents),
        // step 4 is a no-op and each document costs only O(G log G + D log V).
        //
        // Note: initially implemented by @saki7, then the complexity math is
        // double-checked by Claude.

        data.batch_grams.clear();
        data.batch_grams.resize(input.size() - N + 1);

        if constexpr (N == 1) {
            for (std::size_t i = 0; i < input.size(); ++i) {
                data.batch_grams[i].ng.data = input[i];
                data.batch_grams[i].pos = static_cast<int>(i);
            }

        } else {
            for (std::size_t i = 0; i + N <= input.size(); ++i) {
                data.batch_grams[i].ng.copy_n(input.begin() + i);
                data.batch_grams[i].pos = static_cast<int>(i);
            }
        }
        std::ranges::sort(data.batch_grams);

        data.batch_pending.clear();

        for (auto const& chunk : data.batch_grams | std::views::chunk_by(
            [](auto const& a, auto const& b) { return a.ng == b.ng; }
        )) {
            auto const& key = chunk.front().ng;
            if (PostingListT* const pl = data.idx.find_list(key)) {
                for (auto const& gp : chunk) {
                    pl->append(doc_id, gp.pos);
                }

            } else {
                auto& new_pl = data.batch_pending.emplace_back(key, std::make_unique<PostingListT>()).second;
                for (auto const& gp : chunk) {
                    new_pl->append(doc_id, gp.pos);
                }
            }
        }

        data.idx.merge_new_entries(data.batch_pending);
    }
};

} // detail

struct [[nodiscard]] search_word_match
{
    search_word_match() = default;

    explicit search_word_match(int word_id)
        : word_id(word_id)
    {}

    search_word_match(int word_id, std::initializer_list<interval<int>> spans)
        : word_id(word_id)
        , spans(spans)
    {}

    int word_id = 0;
    unsigned successful_ngrams = 1; // due to the class layout, this must be placed here
    std::vector<interval<int>> spans;

    [[nodiscard]]
    bool operator==(search_word_match const& other) const noexcept
    {
        return word_id == other.word_id && spans == other.spans;
    }
};

class [[nodiscard]] search_result
{
    using doc_matches_map = std::flat_map<document_id, std::vector<search_word_match>>;

    struct word_matches_handle
    {
        doc_matches_map::iterator doc_it;
        search_word_match* word_match = nullptr;

        [[nodiscard]]
        search_word_match* operator->() const noexcept
        {
            return word_match;
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return word_match;
        }
    };

public:
    [[nodiscard]]
    bool has_document(document_id const doc_id) const noexcept
    {
        auto const it = doc_matches_.find(doc_id);
        // An entry with no word matches is a tombstone (soft-erased document
        // awaiting the next sweep), not a match.
        return it != doc_matches_.end() && !it->second.empty();
    }

    [[nodiscard]]
    auto const& doc_matches() const noexcept { return doc_matches_; }

    // Returns whether search must continue
    template<bool IsFirstWord, std::size_t N>
    [[nodiscard]]
    bool init_word_matches(document_id const doc_id, int const word_id, std::span<int const> const positions)
    {
        assert(!positions.empty());

        doc_matches_map::iterator doc_matches_it;
        if constexpr (IsFirstWord) {
            assert(word_id == 0);
            assert(doc_matches_.empty() || doc_matches_.rbegin()->first < doc_id);
            doc_matches_it = doc_matches_.try_emplace(doc_matches_.end(), doc_id); // hint: append
            ++live_doc_count_;

        } else {
            doc_matches_it = doc_matches_.find(doc_id);
            if (doc_matches_it == doc_matches_.end()) return false; // no new docs after word 0
            if (doc_matches_it->second.empty()) return false; // tombstoned (soft-erased) document; skip
        }

        assert(!std::ranges::contains(doc_matches_it->second, word_id, &search_word_match::word_id));
        auto& word_match = doc_matches_it->second.emplace_back(word_id);
        word_match.spans.assign_range(positions | std::views::transform([](int const pos) -> interval<int> {
            return {pos, pos + static_cast<int>(N)};
        }));
        return true;
    }

    [[nodiscard]]
    word_matches_handle get_word_matches(document_id const doc_id, int const word_id)
    {
        auto const doc_matches_it = doc_matches_.find(doc_id);
        if (doc_matches_it == doc_matches_.end()) return {};

        // We don't need to do *full* `std::find` here; the word match is
        // always inserted sequentially so if it exists, it is always placed
        // at the *back* of the vector.
        if (
            doc_matches_it->second.empty() ||
            doc_matches_it->second.back().word_id != word_id
        ) {
            assert(
                doc_matches_it->second.empty() ||
                // Make sure the matching element does not exist at the position except for *back*
                !std::ranges::contains(doc_matches_it->second, word_id, &search_word_match::word_id)
            );
            return {};
        }
        assert(!doc_matches_it->second.back().spans.empty());
        return {doc_matches_it, &doc_matches_it->second.back()};
    }

    void erase_document(word_matches_handle const& handle)
    {
        // This is slow because
        // k erases x O(n) shift each =~ O(n^2) per word
        //doc_matches_.erase(handle.doc_it);

        assert(!handle.doc_it->second.empty()); // never double-tombstone
        handle.doc_it->second.clear(); // make this tombstone
        assert(live_doc_count_ >= 1);
        --live_doc_count_;
    }

    void remove_stale_document_matches(int const word_id, unsigned const expected_ngrams)
    {
        auto [keys, values] = std::move(doc_matches_).extract();

        std::size_t out = 0;
        for (std::size_t in = 0; in < keys.size(); ++in) {
            auto& word_matches = values[in];
            bool is_word_survived = false;
            std::erase_if(word_matches, [&](search_word_match const& wm) {
                if (wm.word_id != word_id) return false;
                if (wm.successful_ngrams != expected_ngrams) return true;
                is_word_survived = true;
                return false;
            });
            if (!is_word_survived || word_matches.empty()) continue;

            if (out != in) {
                keys[out] = keys[in];
                values[out] = std::move(values[in]);
            }
            ++out;
        }
        keys.resize(out);
        values.resize(out);
        doc_matches_.replace(std::move(keys), std::move(values));
        live_doc_count_ = out;
    }

    void reset() noexcept
    {
        doc_matches_.clear();
        live_doc_count_ = 0;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return live_doc_count_ == 0;
    }

    [[nodiscard]]
    explicit operator bool() const noexcept
    {
        return !this->empty();
    }

private:
    doc_matches_map doc_matches_;
    std::size_t live_doc_count_ = 0;
};

template<class CharT = char32_t>
class database
{
public:
    [[nodiscard]] document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        document_id const doc_id{next_doc_id_};
        next_doc_id_ = document_id{std::to_underlying(next_doc_id_) + 1u};

        store_.append_index(doc_id, doc_text);
        return doc_id;
    }

    [[nodiscard]] bool is_visible(document_id const doc_id) const noexcept
    {
        return !invisible_documents_.contains(doc_id);
    }

    void set_visible(document_id const doc_id, bool const flag)
    {
        if (flag) {
            invisible_documents_.erase(doc_id);
        } else {
            invisible_documents_.emplace(doc_id);
        }
    }

    void clear() noexcept
    {
        next_doc_id_ = 0_doc_id;
        store_.clear();
        invisible_documents_.clear();
    }

    template<std::size_t N>
    void find_occurrences(gram<N, CharT> ng, std::vector<gram_occurrence>& occs) const
    {
        occs.clear();
        auto const& idx = store_.template get_index<N>();
        idx.find_occurrences(ng, occs);
    }

    bool search(search_query<CharT> const& query, search_result& search_res) const
    {
        if (query.empty()) return false;
        if (store_.empty()) return false;

        int word_id = 0;
        auto it = query.words().begin();
        assert(!it->empty());
        this->search_word<true>(search_res, word_id++, *it++);
        if (search_res.empty()) {
            search_res.reset(); // remove tombstones
            return false;
        }

        for (; it != query.words().end(); ++it) {
            assert(!it->empty());
            this->search_word<false>(search_res, word_id++, *it);
            if (search_res.empty()) {
                search_res.reset(); // remove tombstones
                break;
            }
        }
        return !search_res.empty();
    }

private:
    template<bool IsFirstWord>
    void search_word(search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(!word.empty());

        if (word.size() == 1) {
            this->search_word_impl<IsFirstWord, 1>(search_res, word_id, word);
        } else {
            this->search_word_impl<IsFirstWord, 2>(search_res, word_id, word);
        }
    }

    template<bool IsFirstWord, std::size_t N>
    void search_word_impl(search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(word.size() >= N);
        auto ng = gram<N, CharT>::from_copy_n(word.begin());

        if constexpr (IsFirstWord) {
            store_.search(ng, [&](document_id const doc_id, std::span<int const> const positions) {
                if (!is_visible(doc_id)) return;
                (void)search_res.init_word_matches<IsFirstWord, N>(doc_id, word_id, positions);
            });
            if (search_res.empty()) return;

        } else {
            std::size_t available_doc_count = 0;
            store_.search(ng, [&](document_id const doc_id, std::span<int const> const positions) {
                if (!is_visible(doc_id)) return;
                if (search_res.init_word_matches<IsFirstWord, N>(doc_id, word_id, positions)) {
                    ++available_doc_count;
                }
            });
            if (available_doc_count == 0) {
                search_res.reset();
                return;
            }
        }

        unsigned current_ngram = 1;
        auto const do_search = [&](int remaining_chars) {
            return [&, remaining_chars, overlapping_chars = int(N) - remaining_chars](document_id const doc_id, std::span<int const> const positions) {
                if (!is_visible(doc_id)) return detail::search_continuation::proceed;

                // Find the existing match set from the previous iteration.
                // If none exists, any subsequent characters of the document will not match.
                //
                // For example, when the document is "今日は晴れです" and current `ng` is "は晴",
                //   - When previous `ng` was "昨日", `search_res` contians no matches => omit further sequence
                //   - When previous `ng` was "今日", `search_res` contains matches => proceed with "は晴"
                auto word_match = search_res.get_word_matches(doc_id, word_id);
                if (!word_match) return detail::search_continuation::proceed;

                // Prevent *resurrecting* the false-positive match on "match -> unmatch -> match" pattern.
                // For example, when the document is "abef" and the query is "abXXef",
                //   - gram{"ab"} -> match (successful_ngrams = 1)
                //   - gram{"XX"} -> no match (successful_ngrams is untouched)
                //   - gram{"ef"} -> successful_ngrams does not match current_ngram!
                if (word_match->successful_ngrams != current_ngram) {
                    search_res.erase_document(word_match);
                    if (search_res.empty()) return detail::search_continuation::abort;
                    return detail::search_continuation::proceed;
                }

                // Find contiguous match; document has [previous ng, current ng]
                for (auto it = word_match->spans.begin(); it != word_match->spans.end();) {
                    auto& prev_pos = *it;

                    if (std::ranges::binary_search(positions, prev_pos.upper - overlapping_chars)) {
                        // Matched; the current word's current n-gram is contiguous to the previous n-gram
                        prev_pos.upper += remaining_chars;
                        ++it;
                        continue;
                    }
                    // Erase exiting match that indicates the below structure
                    // [previous ng, ...some unrelated chars..., current ng]
                    it = word_match->spans.erase(it);
                }

                // Even if *all* existing matches fit
                // [previous ng, ...some unrelated chars..., current ng],
                // we can always remove the entire document from the candidate pool.
                if (word_match->spans.empty()) {
                    search_res.erase_document(word_match);
                    if (search_res.empty()) return detail::search_continuation::abort;
                    return detail::search_continuation::proceed;
                }

                ++word_match->successful_ngrams;
                return detail::search_continuation::proceed;
            };
        };

        std::size_t i = N;
        for (; i + N <= word.size(); i += N) {
            ng.copy_n(word.begin() + i);
            store_.search(ng, do_search(N));
            if (search_res.empty()) return;
            ++current_ngram;
        }

        if constexpr (N >= 2) {
            // When the remaining character count is remainder of `word.size() % N`,
            // search by the *slided* remaining characters.
            //
            // For example, when the document is "今日は晴れです":
            //
            // When doing 3-gram search with "今日は雨":
            //   1. Search by "今日は" in the normal loop
            //
            //   2. Then,
            //      i == 3
            //      remaining_chars == word.size() - i == 1
            //      overlapping_chars == N - remaining_chars == 2
            //      next_search_pos = i - overlapping_chars == 1
            //
            //   3. Try to match "日は雨" in the last loop
            if (int const remaining_chars = static_cast<int>(word.size() - i); remaining_chars > 0) {
                assert(remaining_chars < int(N));
                ng.shift_copy(word.begin() + i, remaining_chars);
                store_.search(ng, do_search(remaining_chars));
                if (search_res.empty()) return;
                ++current_ngram;
            }
        }

        if constexpr (IsFirstWord) {
            // A first word of exactly one n-gram runs no continuation searches
            if (current_ngram == 1) return;
        }
        search_res.remove_stale_document_matches(word_id, current_ngram);
    }

    document_id next_doc_id_{0_doc_id};
    detail::index_storage<CharT> store_;
    std::unordered_set<document_id> invisible_documents_;
};

} // iris::gram


template<class CharT>
struct std::formatter<iris::ngram::document_id, CharT>
    : std::formatter<std::underlying_type_t<iris::ngram::document_id>, CharT>
{
    using base_type = std::formatter<std::underlying_type_t<iris::ngram::document_id>, CharT>;

    template<class Ctx>
    Ctx::iterator format(iris::ngram::document_id doc_id, Ctx& ctx) const
    {
        return base_type::format(std::to_underlying(doc_id), ctx);
    }
};

template<class CharT>
struct std::formatter<iris::ngram::gram_occurrence, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram::gram_occurrence const& occ, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}", occ.doc_id, occ.pos);
    }
};

template<class CharT>
struct std::formatter<iris::ngram::search_word_match, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram::search_word_match const& word_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{{word: #{}, spans: {}}}", word_match.word_id, word_match.spans);
    }
};

#endif

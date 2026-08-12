#ifndef IRIS_NGRAM_HPP
#define IRIS_NGRAM_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/default_init_allocator.hpp>
#include <iris/interval.hpp>
#include <iris/string_algo.hpp>
#include <iris/unicode/string.hpp>
#include <iris/format.hpp>

#include <concepts>
#include <span>
#include <flat_map>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <compare>
#include <stdexcept>

#include <cassert>

namespace iris {

enum struct ngram_document_id : unsigned {};

struct ngram_occurrence
{
    ngram_document_id doc_id;
    int pos;

    [[nodiscard]] constexpr bool operator==(ngram_occurrence const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram_occurrence const&) const noexcept = default;
};

namespace detail {

inline constexpr std::size_t N_GRAM_MAX_OPTIMIZED_N = 2;

} // detail

template<std::size_t N, class CharT>
struct ngram
{
    static_assert(N >= 1);

    // TODO: optimize for N=1
    std::array<CharT, N> chars;

    template<std::forward_iterator It>
    [[nodiscard]] static constexpr ngram from_copy_n(It it)
        noexcept(noexcept(*it++))
    {
        if constexpr (N == 1) {
            ngram ng;
            ng.chars[0] = *it;
            return ng;
        } else if constexpr (N == 2) {
            ngram ng;
            ng.chars[0] = *it++;
            ng.chars[1] = *it;
            return ng;
        } else {
            static_assert(detail::N_GRAM_MAX_OPTIMIZED_N == 2);
            ngram ng;
            std::ranges::copy_n(it, N, ng.chars.begin());
            return ng;
        }
    }

    template<std::size_t Len>
    [[nodiscard]] static constexpr ngram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        assert(chars[Len - 1] == static_cast<CharT>(0));
        return ngram::from_copy_n(std::ranges::begin(chars));
    }

    [[nodiscard]] constexpr bool operator==(ngram const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram const&) const noexcept = default;
};

inline namespace ngram_literals {

[[nodiscard]] constexpr ngram_document_id operator ""_doc_id(unsigned long long id) noexcept
{
    return ngram_document_id{static_cast<std::underlying_type_t<ngram_document_id>>(id)};
}

[[nodiscard]] constexpr auto operator ""_2gram(char32_t const* str, std::size_t len) noexcept
{
    assert(len == 2);
    (void)len;
    return ngram<2, char32_t>{str[0], str[1]};
}

[[nodiscard]] constexpr auto operator ""_1gram(char32_t const* str, std::size_t len) noexcept
{
    assert(len == 1);
    (void)len;
    return ngram<1, char32_t>{str[0]};
}

} // ngram_literals

template<class CharT, std::size_t N>
[[nodiscard]] ngram<N - 1, CharT> to_ngram(CharT const (&chars)[N]) noexcept
{
    return ngram<N - 1, CharT>::from_c_array(chars);
}


namespace detail {

enum struct [[nodiscard]] search_continuation : bool
{
    abort = false,
    proceed = true,
};

struct ngram_posting
{
    ngram_document_id doc_id;
    unsigned pos_offset = 0;
    unsigned pos_count = 0;
};

struct ngram_posting_list
{
    std::vector<ngram_posting> postings;
    std::vector<int> positions;

    void append(ngram_document_id const doc_id, int pos)
    {
        if (postings.empty() || postings.back().doc_id != doc_id) {
            if (!postings.empty() && postings.back().doc_id > doc_id) {
                throw std::invalid_argument{"documents must be indexed in non-decreasing order of document ID"};
            }
            postings.emplace_back(
                doc_id,
                static_cast<unsigned>(positions.size()),
                0
            );
        }
        ++postings.back().pos_count;
        positions.emplace_back(pos);
    }

    void to_occurrence_list(std::vector<ngram_occurrence>& occs) const
    {
        occs.clear();
        for (auto const& posting : postings) {
            for (std::size_t i = posting.pos_offset; i < posting.pos_offset + posting.pos_count; ++i) {
                occs.emplace_back(posting.doc_id, positions[i]);
            }
        }
    }

    template<class F>
    void for_each_documents(F&& f) const
    {
        static_assert(std::invocable<F, ngram_document_id, std::span<int const>>);

        constexpr bool f_returns_continuation = std::same_as<
            std::invoke_result_t<F, ngram_document_id, std::span<int const>>,
            search_continuation
        >;

        for (auto const& posting : postings) {
            std::span<int const> const posting_span{
                positions.begin() + posting.pos_offset,
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
};

template<std::size_t N, class CharT, class PostingListT = ngram_posting_list>
struct ngram_index
{
    void append(ngram<N, CharT> const ng, ngram_document_id const doc_id, int const pos)
    {
        gram_entries[ng].append(doc_id, pos);
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return gram_entries.empty();
    }

    void find_occurrences(ngram<N, CharT> const ng, std::vector<ngram_occurrence>& occs) const
    {
        occs.clear();
        auto const it = gram_entries.find(ng);
        if (it == gram_entries.end()) return;

        it->second.to_occurrence_list(occs);
    }

    template<class F>
    void search(ngram<N, CharT> const ng, F&& f) const
    {
        auto const it = gram_entries.find(ng);
        if (it == gram_entries.end()) return;
        it->second.for_each_documents(f);
    }

    std::flat_map<ngram<N, CharT>, PostingListT> gram_entries;
};

template<std::size_t N, class CharT>
struct ngram_pos_t
{
    ngram<N, CharT> ng;
    int pos;

    [[nodiscard]] constexpr bool operator==(ngram_pos_t const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram_pos_t const&) const noexcept = default;
};

template<class CharT, class PostingListT = ngram_posting_list>
struct ngram_index_storage
{
    void append_index(ngram_document_id const doc_id, std::basic_string_view<CharT> const input)
    {
        this->template append_index<1>(doc_id, input);
        this->template append_index<2>(doc_id, input);
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return
            this->template get_data<1>().idx.empty() &&
            this->template get_data<2>().idx.empty();
    }

    template<std::size_t N, class F>
    void search(ngram<N, CharT> const ng, F&& f) const
    {
        this->template get_data<N>().idx.search(ng, f);
    }

    template<std::size_t N>
    [[nodiscard]] auto& get_index(this auto& self) noexcept IRIS_LIFETIMEBOUND
    {
        return self.template get_data<N>().idx;
    }

private:
    template<std::size_t N>
    struct ngram_index_storage_data
    {
        ngram_index<N, CharT, PostingListT> idx;

        // Caches
        std::vector<ngram_pos_t<N, CharT>, default_init_allocator<ngram_pos_t<N, CharT>>>
        batch_grams;

        std::vector<std::pair<ngram<N, CharT>, PostingListT>>
        batch_pending;
    };

    ngram_index_storage_data<1> uni_data_;
    ngram_index_storage_data<2> bi_data_;

    template<std::size_t N>
    [[nodiscard]] auto& get_data(this auto& self) noexcept IRIS_LIFETIMEBOUND
    {
        if constexpr (N == 1) {
            return self.uni_data_;
        } else if constexpr (N == 2) {
            return self.bi_data_;
        } else {
            static_assert(N_GRAM_MAX_OPTIMIZED_N == 2);
        }
    }

    template<std::size_t N>
    void append_index(
        ngram_document_id const doc_id,
        std::basic_string_view<CharT> const input
    )
    {
        if (input.size() < N) return;
        ngram_index_storage_data<N>& data = this->template get_data<N>();

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
                data.batch_grams[i].ng.chars[0] = input[i];
                data.batch_grams[i].pos = static_cast<int>(i);
            }

        } else {
            for (std::size_t i = 0; i + N <= input.size(); ++i) {
                std::ranges::copy_n(input.begin() + i, N, data.batch_grams[i].ng.chars.begin());
                data.batch_grams[i].pos = static_cast<int>(i);
            }
        }
        std::ranges::sort(data.batch_grams);

        data.batch_pending.clear();

        for (auto const& chunk : data.batch_grams | std::views::chunk_by(
            [](auto const& a, auto const& b) { return a.ng == b.ng; }
        )) {
            auto const& key = chunk.front().ng;
            if (auto const it = data.idx.gram_entries.find(key); it != data.idx.gram_entries.end()) {
                for (auto const& gp : chunk) {
                    it->second.append(doc_id, gp.pos);
                }
            } else {
                auto& pl = data.batch_pending.emplace_back(key, PostingListT{}).second;
                for (auto const& gp : chunk) {
                    pl.append(doc_id, gp.pos);
                }
            }
        }
        data.idx.gram_entries.insert(
            std::sorted_unique,
            std::make_move_iterator(data.batch_pending.begin()),
            std::make_move_iterator(data.batch_pending.end())
        );
    }
};

} // detail


template<class CharT = char32_t>
struct ngram_search_query
{
    explicit ngram_search_query(std::basic_string_view<CharT> input_sv)
    {
        std::basic_string<CharT> input{input_sv};
        iris::compact_spaces(input);
        if (input.empty()) return;

        words_ = input
            | std::views::split(detail::string_algo_traits<CharT>::space)
            | std::views::transform([](auto const& r) {
                return std::basic_string<CharT>{std::from_range, r};
            })
            | std::ranges::to<std::vector>();

        std::ranges::sort(words_);
        {
            auto const [first, last] = std::ranges::unique(words_);
            words_.erase(first, last);
        }
    }

    // ------------------------------------------

    [[nodiscard]]
    auto const& words() const noexcept
    {
        return words_;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return words_.empty();
    }

    [[nodiscard]] bool operator==(ngram_search_query const& other) const noexcept
    {
        return words_ == other.words_;
    }

private:
    std::vector<std::basic_string<CharT>> words_;
};

template<class CharT, std::size_t N>
ngram_search_query(CharT const(&)[N]) -> ngram_search_query<CharT>;


struct [[nodiscard]] ngram_search_word_match
{
    ngram_search_word_match() = default;

    explicit ngram_search_word_match(int word_id)
        : word_id(word_id)
    {}

    ngram_search_word_match(int word_id, std::initializer_list<interval<int>> spans)
        : word_id(word_id)
        , spans(spans)
    {}

    int word_id = 0;
    unsigned successful_ngrams = 1; // due to the class layout, this must be placed here
    std::vector<interval<int>> spans;

    [[nodiscard]]
    bool operator==(ngram_search_word_match const& other) const noexcept
    {
        return word_id == other.word_id && spans == other.spans;
    }
};

class [[nodiscard]] ngram_search_result
{
    using doc_matches_map = std::flat_map<ngram_document_id, std::vector<ngram_search_word_match>>;

    struct word_matches_handle
    {
        doc_matches_map::iterator doc_it;
        ngram_search_word_match* word_match = nullptr;

        [[nodiscard]]
        ngram_search_word_match* operator->() const noexcept
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
    bool has_document(ngram_document_id const doc_id) const noexcept
    {
        return doc_matches_.contains(doc_id);
    }

    [[nodiscard]]
    auto const& doc_matches() const noexcept { return doc_matches_; }

    // Returns whether search must continue
    template<bool IsFirstWord, std::size_t N>
    [[nodiscard]]
    bool init_word_matches(ngram_document_id const doc_id, int const word_id, std::span<int const> const positions)
    {
        assert(!positions.empty());

        doc_matches_map::iterator doc_matches_it;
        if constexpr (IsFirstWord) {
            assert(word_id == 0);
            assert(doc_matches_.empty() || doc_matches_.rbegin()->first < doc_id);
            doc_matches_it = doc_matches_.try_emplace(doc_matches_.end(), doc_id); // hint: append
        } else {
            doc_matches_it = doc_matches_.find(doc_id);
            if (doc_matches_it == doc_matches_.end()) return false; // no new docs after word 0
        }

        assert(!std::ranges::contains(doc_matches_it->second, word_id, &ngram_search_word_match::word_id));
        auto& word_match = doc_matches_it->second.emplace_back(word_id);
        word_match.spans.assign_range(positions | std::views::transform([](int const pos) -> interval<int> {
            return {pos, pos + static_cast<int>(N)};
        }));
        return true;
    }

    [[nodiscard]]
    word_matches_handle get_word_matches(ngram_document_id const doc_id, int const word_id)
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
                !std::ranges::contains(doc_matches_it->second, word_id, &ngram_search_word_match::word_id)
            );
            return {};
        }
        assert(!doc_matches_it->second.back().spans.empty());
        return {doc_matches_it, &doc_matches_it->second.back()};
    }

    void erase_document(word_matches_handle const& handle)
    {
        doc_matches_.erase(handle.doc_it);
    }

    void remove_stale_document_matches(int const word_id, unsigned const expected_ngrams)
    {
        auto [keys, values] = std::move(doc_matches_).extract();

        std::size_t out = 0;
        for (std::size_t in = 0; in < keys.size(); ++in) {
            auto& word_matches = values[in];
            bool has_word = false;
            std::erase_if(word_matches, [&](ngram_search_word_match const& wm) {
                if (wm.word_id != word_id) return false;
                has_word = true;
                return wm.successful_ngrams != expected_ngrams;
            });
            if (!has_word || word_matches.empty()) continue;
            if (out != in) {
                keys[out] = keys[in];
                values[out] = std::move(values[in]);
            }
            ++out;
        }
        keys.resize(out);
        values.resize(out);
        doc_matches_.replace(std::move(keys), std::move(values));
    }

    void clear() noexcept
    {
        doc_matches_.clear();
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return doc_matches_.empty();
    }

    [[nodiscard]]
    explicit operator bool() const noexcept
    {
        return !this->empty();
    }

private:
    doc_matches_map doc_matches_;
};

template<class CharT = char32_t>
class ngram_database
{
public:
    [[nodiscard]]
    ngram_document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        ngram_document_id const doc_id{max_doc_id_};
        max_doc_id_ = ngram_document_id{std::to_underlying(max_doc_id_) + 1u};

        store_.append_index(doc_id, doc_text);
        return doc_id;
    }

    template<std::size_t N>
    void find_occurrences(ngram<N, CharT> ng, std::vector<ngram_occurrence>& occs) const noexcept
    {
        occs.clear();
        auto const& idx = store_.template get_index<N>();
        idx.find_occurrences(ng, occs);
    }

    [[nodiscard]]
    ngram_search_result search(ngram_search_query<CharT> const& query) const
    {
        if (query.empty()) return {};
        if (store_.empty()) return {};

        ngram_search_result search_res;

        int word_id = 0;
        auto it = query.words().begin();
        assert(!it->empty());
        this->search_word<true>(search_res, word_id++, *it++);
        if (search_res.empty()) return search_res;

        for (; it != query.words().end(); ++it) {
            assert(!it->empty());
            this->search_word<false>(search_res, word_id++, *it);
            if (search_res.empty()) break;
        }
        return search_res;
    }

private:
    template<bool IsFirstWord>
    void search_word(ngram_search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(!word.empty());

        if (word.size() == 1) {
            this->search_word_impl<IsFirstWord, 1>(search_res, word_id, word);
        } else {
            this->search_word_impl<IsFirstWord, 2>(search_res, word_id, word);
        }
    }

    template<bool IsFirstWord, std::size_t N>
    void search_word_impl(ngram_search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(word.size() >= N);
        auto ng = ngram<N, CharT>::from_copy_n(word.begin());

        if constexpr (IsFirstWord) {
            store_.search(ng, [&](ngram_document_id const doc_id, std::span<int const> const positions) {
                (void)search_res.init_word_matches<IsFirstWord, N>(doc_id, word_id, positions);
            });
            if (search_res.empty()) return;

        } else {
            std::size_t available_doc_count = 0;
            store_.search(ng, [&](ngram_document_id const doc_id, std::span<int const> const positions) {
                if (search_res.init_word_matches<IsFirstWord, N>(doc_id, word_id, positions)) {
                    ++available_doc_count;
                }
            });
            if (available_doc_count == 0) {
                search_res.clear();
                return;
            }
        }

        unsigned current_ngram = 1;
        auto const do_search = [&](int remaining_chars) {
            return [&, remaining_chars, overlapping_chars = int(N) - remaining_chars](ngram_document_id const doc_id, std::span<int const> const positions) {
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
                //   - ngram{"ab"} -> match (successful_ngrams = 1)
                //   - ngram{"XX"} -> no match (successful_ngrams is untouched)
                //   - ngram{"ef"} -> successful_ngrams does not match current_ngram!
                if (word_match->successful_ngrams != current_ngram) {
                    search_res.erase_document(word_match);
                    if (search_res.empty()) return detail::search_continuation::abort;
                    return detail::search_continuation::proceed;
                }

                // Find contiguous match; document has [previous ng, current ng]
                for (auto it = word_match->spans.begin(); it != word_match->spans.end();) {
                    auto& prev_pos = *it;

                    if (std::ranges::binary_search(positions, prev_pos.right - overlapping_chars)) {
                        // Matched; the current word's current n-gram is contiguous to the previous n-gram
                        prev_pos.right += remaining_chars;
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
            std::ranges::copy_n(word.begin() + i, N, ng.chars.begin());
            store_.search(ng, do_search(N));
            if (search_res.empty()) return;
            ++current_ngram;
        }

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
            assert(remaining_chars < N);
            std::shift_left(ng.chars.begin(), ng.chars.end(), remaining_chars);
            std::ranges::copy_n(word.begin() + i, remaining_chars, ng.chars.begin() + (N - remaining_chars));
            store_.search(ng, do_search(remaining_chars));
            if (search_res.empty()) return;
            ++current_ngram;
        }

        search_res.remove_stale_document_matches(word_id, current_ngram);
    }

    ngram_document_id max_doc_id_{0_doc_id};
    detail::ngram_index_storage<CharT> store_;
};

} // iris


template<class CharT>
struct std::formatter<iris::ngram_document_id, CharT>
    : std::formatter<std::underlying_type_t<iris::ngram_document_id>, CharT>
{
    using base_type = std::formatter<std::underlying_type_t<iris::ngram_document_id>, CharT>;

    template<class Ctx>
    Ctx::iterator format(iris::ngram_document_id doc_id, Ctx& ctx) const
    {
        return base_type::format(std::to_underlying(doc_id), ctx);
    }
};

template<class CharT>
struct std::formatter<iris::ngram_occurrence, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram_occurrence const& occ, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}", occ.doc_id, occ.pos);
    }
};

template<class NGCharT, class CharT>
struct std::formatter<iris::ngram_search_query<NGCharT>, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram_search_query<NGCharT> const& query, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}", query.words() | std::views::transform([](std::u32string_view ustr) {
            return iris::unicode::transcode<char>(ustr);
        }));
    }
};

template<class CharT>
struct std::formatter<iris::ngram_search_word_match, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram_search_word_match const& word_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{{word: #{}, spans: {}}}", word_match.word_id, word_match.spans);
    }
};

#endif

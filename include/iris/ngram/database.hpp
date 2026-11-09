#ifndef IRIS_ZZ_NGRAM_DATABASE_HPP
#define IRIS_ZZ_NGRAM_DATABASE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/detail/search_result_cache.hpp>
#include <iris/ngram/detail/id_store.hpp>
#include <iris/ngram/detail/index.hpp>
#include <iris/ngram/detail/slot.hpp>
#include <iris/ngram/gram.hpp>
#include <iris/ngram/search_query.hpp>
#include <iris/ngram/search_result.hpp>
#include <iris/ngram/id.hpp>

#include <concepts>
#include <span>
#include <flat_map>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <utility>

#include <cassert>
#include <cstddef>

namespace iris::ngram {

namespace detail {

struct [[nodiscard]] search_result_cache_guard
{
    search_result_cache& cache;

    ~search_result_cache_guard() noexcept
    {
        cache.reset();
    }
};

} // detail

template<class CharT = char32_t>
class database
{
public:
    using document_id_type = document_id;

    // NOT thread-safe
    [[nodiscard]] document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        auto transaction = id_store_.add_document();
        store_.append_index(transaction.new_slot(), doc_text);
        transaction.commit();
        return transaction.new_id();
    }

    // NOT thread-safe
    void update_document(document_id const doc_id, std::basic_string_view<CharT> const doc_text)
    {
        auto transaction = id_store_.update_document(doc_id);
        store_.append_index(transaction.new_slot(), doc_text);
        transaction.commit();
    }

    // Remove the document specified by `doc_id`.
    // Removing an already-erased document is noop.
    // NOT thread-safe
    void remove_document(document_id const doc_id) noexcept
    {
        id_store_.remove_document(doc_id);
    }

    // Returns whether `doc_id` refers to an existing (non-erased) document.
    // NOT thread-safe
    [[nodiscard]] bool has_document(document_id const doc_id) const noexcept
    {
        return id_store_.has_document(doc_id);
    }

    // NOT thread-safe
    [[nodiscard]] bool is_visible(document_id const doc_id) const
    {
        return id_store_.is_visible(doc_id);
    }

    // NOT thread-safe
    void set_visible(document_id const doc_id, bool const flag)
    {
        id_store_.set_visible(doc_id, flag);
    }

    // NOT thread-safe
    void clear() noexcept
    {
        id_store_.clear();
        store_.clear();
    }

    // Concurrent `search` calls are thread-safe unless other non-thread-safe functions are called at the same time
    template<class DocumentID, class HashT, class EqualT>
    bool search(this auto const& db, std::basic_string_view<CharT> const query, search_result<DocumentID, HashT, EqualT>& search_res)
    {
        return db.search(std::views::single(query), search_res);
    }

    // Concurrent `search` calls are thread-safe unless other non-thread-safe functions are called at the same time
    template<class DocumentID, class HashT, class EqualT>
    bool search(this auto const& db, search_query<CharT> const& query, search_result<DocumentID, HashT, EqualT>& search_res)
    {
        return db.search(query.words(), search_res);
    }

    // Concurrent `search` calls are thread-safe unless other non-thread-safe functions are called at the same time
    template<std::ranges::forward_range SearchWordRange, class DocumentID, class HashT, class EqualT>
        requires std::convertible_to<std::ranges::range_reference_t<SearchWordRange>, std::basic_string_view<CharT>>
    bool search(this auto const& db, SearchWordRange&& query_words, search_result<DocumentID, HashT, EqualT>& search_res)
    {
        search_res.clear();
        if (std::ranges::empty(query_words)) return false;
        if (db.store_.empty()) return false;

        auto& search_res_cache = get_search_res_cache();
        detail::search_result_cache_guard const search_res_cache_guard{search_res_cache};
        search_res_cache.reset();

        {
            int word_id = 0;
            auto word_it = std::ranges::begin(query_words);

            // Find first non-empty word
            for (; word_it != std::ranges::end(query_words); ++word_it) {
                auto&& word_value = *word_it;
                std::basic_string_view<CharT> const word{word_value};
                if (word.empty()) continue;

                db.template search_word<true>(word_id++, word);

                // If the first word's search yielded zero result, the entire search result shall be empty
                // because it's AND search.
                if (search_res_cache.empty()) {
                    return false;
                }

                break;
            }
            if (word_id == 0) [[unlikely]] {
                // If the first non-empty word never occurred, the entire search result shall be empty
                assert(word_it == std::ranges::end(query_words));
                assert(search_res_cache.empty());
                return false;
            }
            ++word_it;

            for (; word_it != std::ranges::end(query_words); ++word_it) {
                auto&& word_value = *word_it;
                std::basic_string_view<CharT> const word{word_value};
                if (word.empty()) continue;

                db.template search_word<false>(word_id++, word);

                // If the current search word shaved off the result to zero, the entire search result
                // shall be empty because it's AND search.
                if (search_res_cache.empty()) {
                    return false;
                }
            }
        }
        assert(!search_res_cache.empty());
        assert(search_res.empty());

        using document_id_maybe_ref_t = std::conditional_t<
            std::is_reference_v<decltype(db.make_document_id(std::declval<document_id>()))>,
            DocumentID const&,
            DocumentID
        >;
        // Convert cache into real result
        search_res.assign(
            search_res_cache.doc_matches() | std::views::transform([&db](auto&& kv)
                -> std::pair<document_id_maybe_ref_t, std::vector<search_word_match>&&>
            {
                return std::pair<document_id_maybe_ref_t, std::vector<search_word_match>&&>{
                    db.make_document_id(db.id_store_.get_info(std::get<0>(kv)).doc_id),
                    std::move(std::get<1>(kv))
                };
            })
        );

        return true;
    }

private:
    [[nodiscard]] static document_id make_document_id(document_id doc_id) noexcept
    {
        return doc_id;
    }

    // non-templated function to allow class instance wide cache
    static detail::search_result_cache& get_search_res_cache()
    {
        thread_local detail::search_result_cache search_res_cache;
        return search_res_cache;
    }

    template<bool IsFirstWord>
    void search_word(int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(!word.empty());

        if (word.size() == 1) {
            this->search_word_impl<IsFirstWord, 1>(word_id, word);
        } else {
            this->search_word_impl<IsFirstWord, 2>(word_id, word);
        }
    }

    template<bool IsFirstWord, std::size_t N>
    void search_word_impl(int const word_id, std::basic_string_view<CharT> const word) const
    {
        assert(word.size() >= N);

        auto& search_res_cache = get_search_res_cache();

        auto ng = gram<N, CharT>::from_copy_n(word.begin());

        if constexpr (IsFirstWord) {
            store_.search(ng, [&](detail::document_slot const doc_slot, std::span<int const> const positions) {
                auto const& slot_info = id_store_.get_info(doc_slot);
                if (!slot_info.is_used_for_search()) return;
                (void)search_res_cache.init_word_matches<IsFirstWord, N>(doc_slot, word_id, positions);
            });
            if (search_res_cache.empty()) return;

        } else {
            std::size_t available_doc_count = 0;
            store_.search(ng, [&](detail::document_slot const doc_slot, std::span<int const> const positions) {
                auto const& slot_info = id_store_.get_info(doc_slot);
                if (!slot_info.is_used_for_search()) return;
                if (search_res_cache.init_word_matches<IsFirstWord, N>(doc_slot, word_id, positions)) {
                    ++available_doc_count;
                }
            });
            if (available_doc_count == 0) {
                search_res_cache.reset();
                return;
            }
        }

        unsigned current_ngram = 1;
        auto const do_search = [&](int remaining_chars) {
            return [&, remaining_chars, overlapping_chars = int(N) - remaining_chars](detail::document_slot const doc_slot, std::span<int const> const positions) {
                auto const& slot_info = id_store_.get_info(doc_slot);
                if (!slot_info.is_used_for_search()) {
                    return detail::search_continuation::proceed;
                }

                // Find the existing match set from the previous iteration.
                // If none exists, any subsequent characters of the document will not match.
                //
                // For example, when the document is "今日は晴れです" and current `ng` is "は晴",
                //   - When previous `ng` was "昨日", `search_res` contians no matches => omit further sequence
                //   - When previous `ng` was "今日", `search_res` contains matches => proceed with "は晴"
                auto word_match = search_res_cache.get_word_matches(doc_slot, word_id);
                if (!word_match) return detail::search_continuation::proceed;

                // Prevent *resurrecting* the false-positive match on "match -> unmatch -> match" pattern.
                // For example, when the document is "abef" and the query is "abXXef",
                //   - gram{"ab"} -> match (successful_ngrams = 1)
                //   - gram{"XX"} -> no match (successful_ngrams is untouched)
                //   - gram{"ef"} -> successful_ngrams does not match current_ngram!
                if (word_match->successful_ngrams_ != current_ngram) {
                    search_res_cache.erase_document(word_match);
                    if (search_res_cache.empty()) return detail::search_continuation::abort;
                    return detail::search_continuation::proceed;
                }

                // Find contiguous match; document has [previous ng, current ng]
                for (auto it = word_match->spans_.begin(); it != word_match->spans_.end();) {
                    auto& prev_pos = *it;

                    if (std::ranges::binary_search(positions, prev_pos.upper - overlapping_chars)) {
                        // Matched; the current word's current n-gram is contiguous to the previous n-gram
                        prev_pos.upper += remaining_chars;
                        ++it;
                        continue;
                    }
                    // Erase exiting match that indicates the below structure
                    // [previous ng, ...some unrelated chars..., current ng]
                    it = word_match->spans_.erase(it);
                }

                // Even if *all* existing matches fit
                // [previous ng, ...some unrelated chars..., current ng],
                // we can always remove the entire document from the candidate pool.
                if (word_match->spans_.empty()) {
                    search_res_cache.erase_document(word_match);
                    if (search_res_cache.empty()) return detail::search_continuation::abort;
                    return detail::search_continuation::proceed;
                }

                ++word_match->successful_ngrams_;
                return detail::search_continuation::proceed;
            };
        };

        std::size_t i = N;
        for (; i + N <= word.size(); i += N) {
            ng.copy_n(word.begin() + i);
            store_.search(ng, do_search(N));
            if (search_res_cache.empty()) return;
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
                if (search_res_cache.empty()) return;
                ++current_ngram;
            }
        }

        if constexpr (IsFirstWord) {
            // A first word of exactly one n-gram runs no continuation searches
            if (current_ngram == 1) return;
        }
        search_res_cache.remove_stale_document_matches(word_id, current_ngram);
    }

    detail::id_store id_store_;
    detail::index_storage<CharT> store_;
};

} // iris::ngram

#endif

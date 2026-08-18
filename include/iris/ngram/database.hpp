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

#include <span>
#include <flat_map>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <utility>

#include <cassert>

namespace iris::ngram {

template<class CharT = char32_t>
class database
{
public:
    using document_id_type = document_id;

    [[nodiscard]] document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        auto transaction = id_store_.add_document();
        store_.append_index(transaction.new_slot(), doc_text);
        transaction.commit();
        return transaction.new_id();
    }

    void update_document(document_id const doc_id, std::basic_string_view<CharT> const doc_text)
    {
        auto transaction = id_store_.update_document(doc_id);
        store_.append_index(transaction.new_slot(), doc_text);
        transaction.commit();
    }

    void remove_document(document_id const doc_id)
    {
        id_store_.remove_document(doc_id);
    }

    [[nodiscard]] bool has_document(document_id const doc_id) const
    {
        return id_store_.has_document(doc_id);
    }

    [[nodiscard]] bool is_visible(document_id const doc_id) const
    {
        return id_store_.is_visible(doc_id);
    }

    void set_visible(document_id const doc_id, bool const flag)
    {
        id_store_.set_visible(doc_id, flag);
    }

    void clear() noexcept
    {
        id_store_.clear();
        store_.clear();
    }

    template<class DocumentID, class HashT, class EqualT>
    bool search(this auto const& db, search_query<CharT> const& query, search_result<DocumentID, HashT, EqualT>& search_res)
    {
        search_res.clear();
        if (query.empty()) return false;
        if (db.store_.empty()) return false;

        db.search_res_cache_.reset();

        int word_id = 0;
        auto it = query.words().begin();
        assert(!it->empty());
        db.template search_word<true>(word_id++, *it++);
        if (db.search_res_cache_.empty()) {
            db.search_res_cache_.reset(); // remove tombstones
            return false;
        }

        for (; it != query.words().end(); ++it) {
            assert(!it->empty());
            db.template search_word<false>(word_id++, *it);
            if (db.search_res_cache_.empty()) {
                db.search_res_cache_.reset(); // remove tombstones
                return false;
            }
        }

        assert(!db.search_res_cache_.empty());
        assert(search_res.empty());

        using document_id_maybe_ref_t = std::conditional_t<
            std::is_reference_v<decltype(db.make_document_id(std::declval<document_id>()))>,
            DocumentID const&,
            DocumentID
        >;
        // Convert cache into real result
        search_res.assign(
            db.search_res_cache_.doc_matches() | std::views::transform([&db](auto&& kv)
            -> std::pair<document_id_maybe_ref_t, std::vector<search_word_match>&&> {
                return std::pair<document_id_maybe_ref_t, std::vector<search_word_match>&&>{
                    db.make_document_id(db.id_store_.get_info(std::get<0>(kv)).doc_id),
                    std::move(std::get<1>(kv))
                };
            })
        );
        // Clear the cache (only contains the moved-from buffer though)
        db.search_res_cache_.reset();

        return true;
    }

private:
    [[nodiscard]] static document_id make_document_id(document_id doc_id) noexcept
    {
        return doc_id;
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
        auto ng = gram<N, CharT>::from_copy_n(word.begin());

        if constexpr (IsFirstWord) {
            store_.search(ng, [&](detail::document_slot const doc_slot, std::span<int const> const positions) {
                auto const& slot_info = id_store_.get_info(doc_slot);
                if (!slot_info.is_used_for_search()) return;
                (void)search_res_cache_.init_word_matches<IsFirstWord, N>(doc_slot, word_id, positions);
            });
            if (search_res_cache_.empty()) return;

        } else {
            std::size_t available_doc_count = 0;
            store_.search(ng, [&](detail::document_slot const doc_slot, std::span<int const> const positions) {
                auto const& slot_info = id_store_.get_info(doc_slot);
                if (!slot_info.is_used_for_search()) return;
                if (search_res_cache_.init_word_matches<IsFirstWord, N>(doc_slot, word_id, positions)) {
                    ++available_doc_count;
                }
            });
            if (available_doc_count == 0) {
                search_res_cache_.reset();
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
                auto word_match = search_res_cache_.get_word_matches(doc_slot, word_id);
                if (!word_match) return detail::search_continuation::proceed;

                // Prevent *resurrecting* the false-positive match on "match -> unmatch -> match" pattern.
                // For example, when the document is "abef" and the query is "abXXef",
                //   - gram{"ab"} -> match (successful_ngrams = 1)
                //   - gram{"XX"} -> no match (successful_ngrams is untouched)
                //   - gram{"ef"} -> successful_ngrams does not match current_ngram!
                if (word_match->successful_ngrams_ != current_ngram) {
                    search_res_cache_.erase_document(word_match);
                    if (search_res_cache_.empty()) return detail::search_continuation::abort;
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
                    search_res_cache_.erase_document(word_match);
                    if (search_res_cache_.empty()) return detail::search_continuation::abort;
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
            if (search_res_cache_.empty()) return;
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
                if (search_res_cache_.empty()) return;
                ++current_ngram;
            }
        }

        if constexpr (IsFirstWord) {
            // A first word of exactly one n-gram runs no continuation searches
            if (current_ngram == 1) return;
        }
        search_res_cache_.remove_stale_document_matches(word_id, current_ngram);
    }

    detail::id_store id_store_;
    detail::index_storage<CharT> store_;
    mutable detail::search_result_cache search_res_cache_;
};

} // iris::ngram

#endif

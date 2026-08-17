#ifndef IRIS_ZZ_NGRAM_DETAIL_SEARCH_RESULT_CACHE_HPP
#define IRIS_ZZ_NGRAM_DETAIL_SEARCH_RESULT_CACHE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/detail/slot.hpp>
#include <iris/ngram/search_result.hpp>

#include <iris/interval.hpp>

#include <span>
#include <ranges>
#include <algorithm>
#include <iterator>
#include <vector>
#include <flat_map>
#include <utility>

#include <cassert>

namespace iris::ngram::detail {

class [[nodiscard]] search_result_cache
{
    using doc_matches_map = std::flat_map<document_slot, std::vector<search_word_match>>;

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
    [[nodiscard]] auto& doc_matches() noexcept { return doc_matches_; }

    // Returns whether search must continue
    template<bool IsFirstWord, std::size_t N>
    [[nodiscard]] bool init_word_matches(document_slot const doc_slot, int const word_id, std::span<int const> const positions)
    {
        assert(!positions.empty());

        doc_matches_map::iterator doc_matches_it;
        if constexpr (IsFirstWord) {
            assert(word_id == 0);
            assert(doc_matches_.empty() || doc_matches_.rbegin()->first < doc_slot);
            doc_matches_it = doc_matches_.try_emplace(doc_matches_.end(), doc_slot); // hint: append
            ++live_doc_count_;

        } else {
            doc_matches_it = doc_matches_.find(doc_slot);
            if (doc_matches_it == doc_matches_.end()) return false; // no new docs after word 0
            if (doc_matches_it->second.empty()) return false; // tombstoned (soft-erased) document; skip
        }

        assert(!std::ranges::contains(doc_matches_it->second, word_id, &search_word_match::word_id_));
        auto& word_match = doc_matches_it->second.emplace_back(word_id);
        word_match.spans_.assign_range(positions | std::views::transform([](int const pos) -> interval<int> {
            return {pos, pos + static_cast<int>(N)};
        }));
        return true;
    }

    [[nodiscard]] word_matches_handle get_word_matches(document_slot const doc_slot, int const word_id)
    {
        auto const doc_matches_it = doc_matches_.find(doc_slot);
        if (doc_matches_it == doc_matches_.end()) return {};

        // We don't need to do *full* `std::find` here; the word match is
        // always inserted sequentially so if it exists, it is always placed
        // at the *back* of the vector.
        if (
            doc_matches_it->second.empty() ||
            doc_matches_it->second.back().word_id_ != word_id
        ) {
            assert(
                doc_matches_it->second.empty() ||
                // Make sure the matching element does not exist at the position except for *back*
                !std::ranges::contains(doc_matches_it->second, word_id, &search_word_match::word_id_)
            );
            return {};
        }
        assert(!doc_matches_it->second.back().spans_.empty());
        return {doc_matches_it, &doc_matches_it->second.back()};
    }

    void erase_document(word_matches_handle const& handle)
    {
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
                if (wm.word_id_ != word_id) return false;
                if (wm.successful_ngrams_ != expected_ngrams) return true;
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

    // Clears the search result and tombstones
    void reset() noexcept
    {
        doc_matches_.clear();
        live_doc_count_ = 0;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return live_doc_count_ == 0;
    }

private:
    doc_matches_map doc_matches_;
    std::size_t live_doc_count_ = 0;
};

} // iris::ngram::detail

#endif

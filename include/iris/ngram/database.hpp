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
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <limits>
#include <utility>

#include <cassert>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram {

template<class CharT = char32_t>
class database
{
public:
    using document_id_type = document_id;

    // Leaves the documents unchanged if a database limit is exceeded
    [[nodiscard]] document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        this->compact();
        auto const doc_slot = id_store_.slot_for_new_document();
        store_.append_index(doc_slot, doc_text);
        return id_store_.add_document(doc_text.size() + 1);
    }

    void update_document(document_id const doc_id, std::basic_string_view<CharT> const doc_text)
    {
        this->compact();
        auto const doc_slot = id_store_.slot_for_update(doc_id);
        store_.append_index(doc_slot, doc_text);
        id_store_.update_document(doc_id, doc_text.size() + 1);
    }

    // No-op for a removed document
    void remove_document(document_id const doc_id) noexcept
    {
        id_store_.remove_document(doc_id);
    }

    [[nodiscard]] bool has_document(document_id const doc_id) const noexcept
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

    void swap(database& other) noexcept
    {
        using std::swap;
        swap(id_store_, other.id_store_);
        swap(store_, other.store_);
    }

    template<class DocumentID>
    bool search(this auto const& db, std::basic_string_view<CharT> const query, search_result<DocumentID>& search_res)
    {
        return db.search(std::views::single(query), search_res);
    }

    template<class DocumentID>
    bool search(this auto const& db, search_query<CharT> const& query, search_result<DocumentID>& search_res)
    {
        return db.search(query.words(), search_res);
    }

    // AND search; empty words are ignored. Returns whether any document matched.
    template<std::ranges::forward_range SearchWordRange, class DocumentID>
        requires std::convertible_to<std::ranges::range_reference_t<SearchWordRange>, std::basic_string_view<CharT>>
    bool search(this auto const& db, SearchWordRange&& query_words, search_result<DocumentID>& search_res)
    {
        search_res.clear();

        auto& plan = get_search_plan();
        plan.words.clear();
        plan.grams.clear();
        for (auto&& word_value : query_words) {
            std::basic_string_view<CharT> const word{word_value};
            if (word.empty()) continue;
            if (!db.plan_word(plan, word)) return false;
        }
        if (plan.words.empty()) return false;

        // A single gram needs no candidate cache
        if (plan.words.size() == 1 && plan.words.front().length <= 2) {
            search_result_rollback rollback{search_res};
            db.search_single_gram(plan.grams.front().list, plan.words.front().length, search_res);
            rollback.commit();
            return !search_res.empty();
        }

        // Start from the word whose rarest gram is in the fewest documents
        std::ranges::sort(plan.words, {}, [&plan](word_plan const& wp) { return plan.grams[wp.gram_begin].list.postings.size(); });

        std::size_t const word_count = plan.words.size();
        auto& cache = get_search_res_cache();
        cache.reset(word_count);
        for (std::size_t i = 0; i < word_count; ++i) {
            db.search_word(cache, plan, plan.words[i], i == 0);
            if (cache.empty()) return false;
        }

        search_result_rollback rollback{search_res};
        search_res.reset(word_count);
        search_res.reserve_documents(cache.size());
        for (std::size_t candidate = 0; candidate < cache.size(); ++candidate) {
            search_res.add_document(db.make_document_id(db.id_store_.get_info(cache.slot(candidate)).doc_id));
            for (std::size_t word = 0; word < word_count; ++word) {
                search_res.add_word_spans(cache.spans(candidate, word));
            }
        }
        rollback.commit();
        return true;
    }

private:
    [[nodiscard]] static document_id make_document_id(document_id doc_id) noexcept
    {
        return doc_id;
    }

    template<class DocumentID>
    struct search_result_rollback
    {
        explicit search_result_rollback(search_result<DocumentID>& search_res) noexcept
            : search_res_(&search_res)
        {}

        void commit() noexcept
        {
            search_res_ = nullptr;
        }

        ~search_result_rollback()
        {
            if (search_res_) {
                search_res_->clear();
            }
        }

    private:
        search_result<DocumentID>* search_res_;
    };

    void compact()
    {
        if (!id_store_.needs_compaction()) return;

        auto const slot_map = id_store_.compaction_map();
        store_.compact(slot_map);
        id_store_.compact(slot_map);
    }

    [[nodiscard]] static detail::search_result_cache& get_search_res_cache()
    {
        thread_local detail::search_result_cache search_res_cache;
        return search_res_cache;
    }

    struct gram_plan
    {
        int offset; // in the word
        detail::posting_list list;
    };

    struct word_plan
    {
        std::size_t word_id; // the index among the non-empty words
        int length;

        // Into `search_plan::grams`, sorted by the number of postings
        std::size_t gram_begin;
        std::size_t gram_end;
    };

    struct search_plan
    {
        std::vector<word_plan> words;
        std::vector<gram_plan> grams;
    };

    [[nodiscard]] static search_plan& get_search_plan()
    {
        thread_local search_plan plan;
        return plan;
    }

    // Returns false if the word cannot match
    [[nodiscard]] bool plan_word(search_plan& plan, std::basic_string_view<CharT> const word) const
    {
        if (word.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;

        std::size_t const gram_begin = plan.grams.size();
        int const length = static_cast<int>(word.size());

        // Use disjoint bigrams; overlap the final bigram for odd lengths
        for (int offset = 0;; offset += 2) {
            offset = std::max(std::min(offset, length - 2), 0);
            auto const list = store_.find(word.substr(static_cast<std::size_t>(offset), std::min(length, 2)));
            if (list.postings.empty()) return false;
            plan.grams.emplace_back(offset, list);
            if (offset + 2 >= length) break;
        }

        auto const grams = std::span{plan.grams}.subspan(gram_begin);
        std::ranges::sort(grams, {}, [](gram_plan const& gp) { return gp.list.postings.size(); });
        plan.words.emplace_back(plan.words.size(), length, gram_begin, plan.grams.size());
        return true;
    }

    template<class DocumentID>
    void search_single_gram(this auto const& db, detail::posting_list const& list, int const length, search_result<DocumentID>& search_res)
    {
        search_res.reset(1);
        for (auto const& posting : list.postings) {
            auto const& slot_info = db.id_store_.get_info(posting.doc_slot);
            if (!slot_info.is_used_for_search()) continue;

            search_res.add_document(db.make_document_id(slot_info.doc_id));
            search_res.add_word_positions(list.positions(posting), length);
        }
    }

    // Anchor spans at word starts so grams can be checked in rarity order
    void search_word(detail::search_result_cache& cache, search_plan const& plan, word_plan const& wp, bool const is_first) const
    {
        auto const grams = std::span{plan.grams}.subspan(wp.gram_begin, wp.gram_end - wp.gram_begin);
        auto const& first = grams.front();
        if (is_first) {
            for (auto const& posting : first.list.postings) {
                if (!id_store_.get_info(posting.doc_slot).is_used_for_search()) continue;
                cache.add_candidate(posting.doc_slot, wp.word_id, first.list.positions(posting), first.offset, wp.length);
            }

        } else {
            cache.narrow(first.list, [&](std::size_t const candidate, std::span<int const> const positions) {
                return cache.set_word_spans(candidate, wp.word_id, positions, first.offset, wp.length);
            });
        }

        for (auto const& gp : grams.subspan(1)) {
            if (cache.empty()) return;
            cache.narrow(gp.list, [&](std::size_t const candidate, std::span<int const> const positions) {
                return cache.keep_word_spans(candidate, wp.word_id, positions, gp.offset);
            });
        }

        if (wp.length >= 2) {
            cache.remove_overlapping_word_spans(wp.word_id);
        }
    }

    detail::id_store id_store_;
    detail::index_storage<CharT> store_;
};

template<class CharT>
void swap(database<CharT>& a, database<CharT>& b) noexcept
{
    a.swap(b);
}

} // iris::ngram

#endif

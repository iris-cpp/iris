#ifndef IRIS_ZZ_NGRAM_DETAIL_INDEX_HPP
#define IRIS_ZZ_NGRAM_DETAIL_INDEX_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

// Make sure we don't include `ngram/id.hpp` so that we can assure the
// internal logic never uses the external id type
#include <iris/ngram/gram.hpp>

#include <iris/default_init_allocator.hpp>

#include <ranges>
#include <string_view>
#include <memory>
#include <vector>
#include <flat_map>
#include <iterator>
#include <span>
#include <concepts>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <compare>

#include <cassert>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram::detail {

enum struct [[nodiscard]] search_continuation : bool
{
    abort = false,
    proceed = true,
};

class posting_list
{
public:
    void append(document_slot const doc_slot, int pos)
    {
        assert(postings_.empty() || postings_.back().doc_slot == document_slot::sentinel);
        assert(postings_.empty() || postings_.back().pos_offset == positions_.size());

        if (postings_.empty()) {
            postings_.emplace_back(doc_slot, 0u);
            postings_.emplace_back(document_slot::sentinel, 0u);

        } else if (auto last_posting = postings_.end() - 2; last_posting->doc_slot != doc_slot) {
            if (last_posting->doc_slot > doc_slot) {
                throw std::logic_error{"documents must be indexed in non-decreasing order of document_slot"};
            }
            assert(postings_.back().doc_slot == document_slot::sentinel);

            // Promote the sentinel into a real posting
            postings_.back().doc_slot = doc_slot;
            postings_.emplace_back(document_slot::sentinel, static_cast<unsigned>(positions_.size()));
        }

        positions_.emplace_back(pos);
        ++postings_.back().pos_offset;
    }

    template<class F>
    void for_each_documents(F&& f) const
    {
        static_assert(std::invocable<F, document_slot, std::span<int const>>);

        constexpr bool f_returns_continuation = std::same_as<
            std::invoke_result_t<F, document_slot, std::span<int const>>,
            search_continuation
        >;

        for (auto const& [posting, next] : postings_ | std::views::pairwise) {
            std::span<int const> const posting_span{
                std::next(positions_.begin(), posting.pos_offset),
                static_cast<std::size_t>(next.pos_offset - posting.pos_offset)
            };

            if constexpr (f_returns_continuation) {
                search_continuation const cont = f(posting.doc_slot, posting_span);
                if (cont == search_continuation::abort) break;
            } else {
                f(posting.doc_slot, posting_span);
            }
        }
    }

private:
    struct posting_t
    {
        document_slot doc_slot;
        unsigned pos_offset = 0;
    };
    std::vector<posting_t> postings_;
    std::vector<int> positions_;
};

template<std::size_t N, class CharT>
class gram_index
{
    using gram_posting_map = std::flat_map<gram<N, CharT>, std::unique_ptr<posting_list>>;
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
        return static_cast<posting_list*>(nullptr);
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

    void merge_new_entries(std::vector<std::pair<gram<N, CharT>, std::unique_ptr<posting_list>>>& pending)
    {
        if (pending.empty()) return; // vocabulary saturated

        for (auto& [key, pl] : pending) {
            [[maybe_unused]] auto const it = side_entries_.try_emplace(
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
    gram_posting_map gram_entries_, side_entries_;
};

template<std::size_t N, class CharT>
struct gram_pos_t
{
    gram<N, CharT> ng;
    int pos;

    [[nodiscard]] constexpr bool operator==(gram_pos_t const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram_pos_t const&) const noexcept = default;
};

template<class CharT>
struct index_storage
{
    void append_index(document_slot const doc_slot, std::basic_string_view<CharT> const input)
    {
        this->template append_index<1>(doc_slot, input);
        this->template append_index<2>(doc_slot, input);
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
        gram_index<N, CharT> idx;

        // Caches
        std::vector<gram_pos_t<N, CharT>, default_init_allocator<gram_pos_t<N, CharT>>>
        batch_grams;

        std::vector<std::pair<gram<N, CharT>, std::unique_ptr<posting_list>>>
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
        document_slot const doc_slot,
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
            if (posting_list* const pl = data.idx.find_list(key)) {
                for (auto const& gp : chunk) {
                    pl->append(doc_slot, gp.pos);
                }

            } else {
                auto& new_pl = data.batch_pending.emplace_back(key, std::make_unique<posting_list>()).second;
                for (auto const& gp : chunk) {
                    new_pl->append(doc_slot, gp.pos);
                }
            }
        }

        data.idx.merge_new_entries(data.batch_pending);
    }
};

} // iris::ngram::detail

#endif

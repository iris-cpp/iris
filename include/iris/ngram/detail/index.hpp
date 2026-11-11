#ifndef IRIS_ZZ_NGRAM_DETAIL_INDEX_HPP
#define IRIS_ZZ_NGRAM_DETAIL_INDEX_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

// Not including `ngram/id.hpp` keeps the external id type out of the index
#include <iris/ngram/detail/slot.hpp>
#include <iris/ngram/gram.hpp>

#include <iris/default_init_allocator.hpp>
#include <iris/container_traits.hpp>

#include <string_view>
#include <vector>
#include <span>
#include <numeric>
#include <limits>
#include <algorithm>
#include <bit>
#include <type_traits>
#include <utility>
#include <stdexcept>

#include <cassert>
#include <cstdint>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram::detail {

struct posting
{
    document_slot doc_slot;
    std::uint32_t pos_count;
    int pos_or_offset; // the position if `pos_count == 1`, otherwise the offset into the pool
};

// Sorted by `document_slot`; invalidated by any modification of the index
struct posting_list
{
    std::span<posting const> postings;
    std::span<int const> pool;

    [[nodiscard]] std::span<int const> positions(posting const& p IRIS_LIFETIMEBOUND) const noexcept
    {
        if (p.pos_count == 1) return {&p.pos_or_offset, 1};
        return pool.subspan(static_cast<std::size_t>(p.pos_or_offset), p.pos_count);
    }
};

// Each n-gram owns a slot-sorted segment of a shared pool with `bit_ceil(size)` capacity.
// Relocation leaves old segments unused until compaction.
// Multiple positions are pooled; single positions are stored inline.
template<std::size_t N, class CharT>
class gram_index
{
    static_assert(N == 1 || N == 2, "gram_index requires a gram stored as a single integer");

    using key_type = std::make_unsigned_t<decltype(gram<N, CharT>::data)>;

    static constexpr std::uint32_t npos = std::numeric_limits<std::uint32_t>::max();

    // An empty slot has `begin == npos`
    struct chain_slot
    {
        key_type key;
        std::uint32_t begin = npos;
        std::uint32_t size = 0;

        [[nodiscard]] bool is_full() const noexcept
        {
            return std::has_single_bit(size) || size == 0;
        }
    };

    // In the current document iff `serial - serial_base < keys.size()`; the difference is the run id.
    // Slots are not cleared between documents.
    struct run_slot
    {
        key_type key;
        std::uint32_t serial = npos;
    };

    // Scratch for grouping a document by n-gram. Copies omit it; copy assignment keeps the buffers for reuse.
    struct run_buffers
    {
        run_buffers() = default;
        run_buffers(run_buffers const&) noexcept {}
        run_buffers(run_buffers&&) = default;
        run_buffers& operator=(run_buffers const&) noexcept { return *this; }
        run_buffers& operator=(run_buffers&&) = default;

        std::vector<run_slot> slots; // the size is zero or a power of two
        int shift = 0;
        std::uint32_t serial_base = 0;

        std::vector<std::uint32_t, default_init_allocator<std::uint32_t>> run_of; // of each occurrence
        std::vector<key_type> keys;
        std::vector<std::uint32_t> begins; // into `positions`, with the end of the last run
        std::vector<int, default_init_allocator<int>> positions;
    };

public:
    [[nodiscard]] posting_list find(gram<N, CharT> const ng) const noexcept
    {
        auto const* const chain = this->find_chain(key_of(ng));
        if (!chain) return {};
        return {this->segment_of(*chain), positions_};
    }

    // Groups `input` by n-gram for `append()` in O(occurrences + distinct n-grams) expected time.
    // Throws `std::length_error` if appending would exceed the limits; the index is unchanged.
    void group(std::basic_string_view<CharT> const input)
    {
        auto& runs = runs_;
        runs.serial_base += static_cast<std::uint32_t>(runs.keys.size());
        runs.keys.clear();
        runs.begins.clear();
        runs.positions.clear();
        if (input.size() < N) return;

        std::size_t const gram_count = input.size() - N + 1;
        if (gram_count > npos - runs.serial_base) {
            // Serial numbers are exhausted
            std::ranges::fill(runs.slots, run_slot{});
            runs.serial_base = 0;
        }
        if (runs.slots.empty()) {
            this->grow_run_slots();
        }

        container::reserve_geometric(runs.run_of, gram_count);
        runs.run_of.resize(gram_count);

        for (std::size_t i = 0; i < gram_count; ++i) {
            key_type const key = key_of(gram<N, CharT>::from_copy_n(input.begin() + i));
            auto* slot = &this->run_slot_of(key);
            std::uint32_t run = slot->serial - runs.serial_base;

            if (run >= runs.keys.size()) {
                if ((runs.keys.size() + 1) * 2 > runs.slots.size()) {
                    this->grow_run_slots();
                    slot = &this->run_slot_of(key);
                }
                run = static_cast<std::uint32_t>(runs.keys.size());
                runs.keys.emplace_back(key);
                runs.begins.emplace_back(0);
                *slot = {key, runs.serial_base + run};
            }
            runs.run_of[i] = run;
            ++runs.begins[run];
        }

        runs.begins.emplace_back(0);
        std::inclusive_scan(runs.begins.begin(), runs.begins.end(), runs.begins.begin());

        // Fill backwards to keep the positions ascending within each run
        container::reserve_geometric(runs.positions, gram_count);
        runs.positions.resize(gram_count);
        for (std::size_t i = gram_count; i-- > 0;) {
            runs.positions[--runs.begins[runs.run_of[i]]] = static_cast<int>(i);
        }

        std::size_t position_growth = 0;
        for (std::size_t run = 0; run < runs.keys.size(); ++run) {
            if (std::uint32_t const pos_count = runs.begins[run + 1] - runs.begins[run]; pos_count > 1) {
                position_growth += pos_count;
            }
        }
        if (position_growth > static_cast<std::size_t>(std::numeric_limits<int>::max()) - positions_.size()) {
            throw std::length_error{"too many n-gram positions in the index"};
        }

        // Disjoint chains hold at most `postings_.size()` postings, so the growth is at most
        // `postings_.size() * 2 + keys.size()`. Look up the chains for per-chain bounds only
        // if this may exceed the limit; `npos` is reserved for empty slots.
        std::uint64_t const posting_count = postings_.size();
        if (posting_count * 3 + runs.keys.size() <= npos) return;

        std::uint64_t posting_growth = 0;
        for (auto const key : runs.keys) {
            auto const* const chain = this->find_chain(key);
            if (!chain) {
                posting_growth += 1;

            } else if (chain->is_full()) {
                posting_growth += std::max(std::uint64_t{chain->size} * 2, std::uint64_t{1});
            }
        }
        if (posting_count + posting_growth > npos) {
            throw std::length_error{"too many postings in the index"};
        }
    }

    // Requires `group()` and a `doc_slot` greater than all indexed slots
    void append(document_slot const doc_slot)
    {
        auto const& runs = runs_;
        for (std::size_t run = 0; run < runs.keys.size(); ++run) {
            auto& chain = this->insert_chain(runs.keys[run]);
            if (chain.is_full()) {
                this->grow_segment(chain);
            }
            assert(chain.size == 0 || postings_[chain.begin + chain.size - 1].doc_slot < doc_slot);

            std::uint32_t const pos_count = runs.begins[run + 1] - runs.begins[run];
            int const* const run_pos = runs.positions.data() + runs.begins[run];
            if (pos_count == 1) {
                postings_[chain.begin + chain.size] = {doc_slot, pos_count, *run_pos};

            } else {
                postings_[chain.begin + chain.size] = {doc_slot, pos_count, static_cast<int>(positions_.size())};
                positions_.insert(positions_.end(), run_pos, run_pos + pos_count);
            }
            ++chain.size;
        }
    }

    // Drops the postings of the slots mapped to tombstone and renumbers the others by `slot_map`
    void compact(std::span<document_slot const> const slot_map)
    {
        auto const is_live = [&](posting const& p) { return slot_map[to_index(p.doc_slot)] != document_slot::tombstone; };

        std::size_t chain_count = 0;
        std::size_t posting_count = 0;
        std::size_t position_count = 0;
        for (auto const& chain : chain_slots_) {
            if (chain.begin == npos) continue;

            std::size_t live_count = 0;
            for (auto const& p : this->segment_of(chain)) {
                if (!is_live(p)) continue;
                ++live_count;
                if (p.pos_count > 1) {
                    position_count += p.pos_count;
                }
            }
            if (live_count == 0) continue;
            ++chain_count;
            posting_count += std::bit_ceil(live_count);
        }

        gram_index result;
        result.reserve_chains(chain_count);
        result.postings_.reserve(posting_count);
        result.positions_.reserve(position_count);

        for (auto const& chain : chain_slots_) {
            if (chain.begin == npos) continue;

            chain_slot* new_chain = nullptr;
            for (auto const& p : this->segment_of(chain)) {
                if (!is_live(p)) continue;
                if (!new_chain) {
                    new_chain = &result.chain_slot_of(chain.key);
                    *new_chain = {chain.key, static_cast<std::uint32_t>(result.postings_.size()), 0};
                    ++result.chain_count_;
                }

                int pos_or_offset = p.pos_or_offset;
                if (p.pos_count > 1) {
                    pos_or_offset = static_cast<int>(result.positions_.size());
                    auto const positions = std::span{positions_}.subspan(static_cast<std::size_t>(p.pos_or_offset), p.pos_count);
                    result.positions_.insert(result.positions_.end(), positions.begin(), positions.end());
                }
                result.postings_.emplace_back(slot_map[to_index(p.doc_slot)], p.pos_count, pos_or_offset);
                ++new_chain->size;
            }
            if (new_chain) {
                result.postings_.resize(new_chain->begin + std::bit_ceil(std::size_t{new_chain->size}));
            }
        }

        // Keep `runs_` for reuse
        chain_slots_.swap(result.chain_slots_);
        chain_count_ = result.chain_count_;
        chain_shift_ = result.chain_shift_;
        postings_.swap(result.postings_);
        positions_.swap(result.positions_);
    }

    void clear() noexcept
    {
        chain_slots_.clear();
        chain_count_ = 0;
        chain_shift_ = 0;
        postings_.clear();
        positions_.clear();
    }

    void swap(gram_index& other) noexcept
    {
        using std::swap;
        swap(chain_slots_, other.chain_slots_);
        swap(chain_count_, other.chain_count_);
        swap(chain_shift_, other.chain_shift_);
        swap(postings_, other.postings_);
        swap(positions_, other.positions_);
        swap(runs_, other.runs_);
    }

private:
    [[nodiscard]] static constexpr key_type key_of(gram<N, CharT> const ng) noexcept
    {
        return static_cast<key_type>(ng.data);
    }

    // Fibonacci hashing
    [[nodiscard]] static std::size_t home_of(key_type const key, int const shift) noexcept
    {
        return static_cast<std::size_t>((static_cast<std::uint64_t>(key) * 0x9E3779B97F4A7C15ull) >> shift);
    }

    // Returns the slot holding `key`, or the empty slot for it
    template<class Self>
    [[nodiscard]] auto& chain_slot_of(this Self& self, key_type const key) noexcept
    {
        assert(!self.chain_slots_.empty());
        std::size_t const mask = self.chain_slots_.size() - 1;
        auto i = home_of(key, self.chain_shift_);
        while (self.chain_slots_[i].begin != npos && self.chain_slots_[i].key != key) {
            i = (i + 1) & mask;
        }
        return self.chain_slots_[i];
    }

    [[nodiscard]] std::span<posting const> segment_of(chain_slot const& chain) const noexcept
    {
        return std::span{postings_}.subspan(chain.begin, chain.size);
    }

    [[nodiscard]] chain_slot const* find_chain(key_type const key) const noexcept
    {
        if (chain_slots_.empty()) return nullptr;
        auto const& slot = this->chain_slot_of(key);
        return slot.begin == npos ? nullptr : &slot;
    }

    chain_slot& insert_chain(key_type const key)
    {
        if (chain_slots_.empty()) {
            this->reserve_chains(1);
        }
        auto* chain = &this->chain_slot_of(key);
        if (chain->begin == npos) {
            if ((chain_count_ + 1) * 2 > chain_slots_.size()) {
                this->reserve_chains(chain_count_ + 1);
                chain = &this->chain_slot_of(key);
            }
            *chain = {key, 0, 0};
            ++chain_count_;
        }
        return *chain;
    }

    // Keeps the load factor at or below 1/2
    void reserve_chains(std::size_t const count)
    {
        if (count * 2 <= chain_slots_.size()) return;

        std::size_t const new_size = std::bit_ceil(std::max(count * 2, std::size_t{16}));
        std::vector<chain_slot> old_slots(new_size);
        chain_slots_.swap(old_slots);
        chain_shift_ = 64 - std::countr_zero(new_size);

        for (auto const& old_slot : old_slots) {
            if (old_slot.begin == npos) continue;
            this->chain_slot_of(old_slot.key) = old_slot;
        }
    }

    // Returns the slot of the current document holding `key`, or a free slot for it
    [[nodiscard]] run_slot& run_slot_of(key_type const key) noexcept
    {
        auto& runs = runs_;
        std::size_t const mask = runs.slots.size() - 1;
        std::size_t const run_count = runs.keys.size();
        auto i = home_of(key, runs.shift);
        while (runs.slots[i].serial - runs.serial_base < run_count && runs.slots[i].key != key) {
            i = (i + 1) & mask;
        }
        return runs.slots[i];
    }

    // Only the runs of the current document need to be moved
    void grow_run_slots()
    {
        auto& runs = runs_;
        std::size_t const new_size = std::max(runs.slots.size() * 2, std::size_t{16});
        std::vector<run_slot> new_slots(new_size);
        runs.slots.swap(new_slots);
        runs.shift = 64 - std::countr_zero(new_size);

        for (std::size_t run = 0; run < runs.keys.size(); ++run) {
            this->run_slot_of(runs.keys[run]) = {runs.keys[run], runs.serial_base + static_cast<std::uint32_t>(run)};
        }
    }

    void grow_segment(chain_slot& chain)
    {
        assert(chain.is_full());

        if (chain.size == 0) {
            chain.begin = static_cast<std::uint32_t>(postings_.size());
            postings_.resize(postings_.size() + 1);

        } else if (chain.begin + chain.size == postings_.size()) {
            postings_.resize(postings_.size() + chain.size);

        } else {
            auto const new_begin = static_cast<std::uint32_t>(postings_.size());
            postings_.resize(postings_.size() + std::size_t{chain.size} * 2);
            std::copy_n(postings_.begin() + chain.begin, chain.size, postings_.begin() + new_begin);
            chain.begin = new_begin;
        }
    }

    std::vector<chain_slot> chain_slots_; // the size is zero or a power of two
    std::size_t chain_count_ = 0;
    int chain_shift_ = 0;

    std::vector<posting, default_init_allocator<posting>> postings_;
    std::vector<int, default_init_allocator<int>> positions_;

    run_buffers runs_;
};

template<std::size_t N, class CharT>
void swap(gram_index<N, CharT>& a, gram_index<N, CharT>& b) noexcept
{
    a.swap(b);
}

template<class CharT>
struct index_storage
{
    // Checks the limits of both indexes before changing either
    void append_index(document_slot const doc_slot, std::basic_string_view<CharT> const input)
    {
        if (input.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
            throw std::length_error{"document is too long to be indexed"};
        }
        uni_index_.group(input);
        bi_index_.group(input);
        uni_index_.append(doc_slot);
        bi_index_.append(doc_slot);
    }

    void compact(std::span<document_slot const> const slot_map)
    {
        uni_index_.compact(slot_map);
        bi_index_.compact(slot_map);
    }

    void clear() noexcept
    {
        uni_index_.clear();
        bi_index_.clear();
    }

    void swap(index_storage& other) noexcept
    {
        using std::swap;
        swap(uni_index_, other.uni_index_);
        swap(bi_index_, other.bi_index_);
    }

    [[nodiscard]] posting_list find(std::basic_string_view<CharT> const chars) const noexcept
    {
        assert(chars.size() == 1 || chars.size() == 2);
        if (chars.size() == 1) return uni_index_.find(gram<1, CharT>::from_copy_n(chars.begin()));
        return bi_index_.find(gram<2, CharT>::from_copy_n(chars.begin()));
    }

private:
    gram_index<1, CharT> uni_index_;
    gram_index<2, CharT> bi_index_;
};

template<class CharT>
void swap(index_storage<CharT>& a, index_storage<CharT>& b) noexcept
{
    a.swap(b);
}

} // iris::ngram::detail

#endif

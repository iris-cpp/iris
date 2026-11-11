#ifndef IRIS_ZZ_NGRAM_DETAIL_SEARCH_RESULT_CACHE_HPP
#define IRIS_ZZ_NGRAM_DETAIL_SEARCH_RESULT_CACHE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/detail/index.hpp>
#include <iris/ngram/detail/slot.hpp>

#include <iris/interval.hpp>

#include <span>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <stdexcept>

#include <cassert>
#include <cstdint>
#include <cstddef>

namespace iris::ngram::detail {

// Galloping lower bound from the front; skipping k elements costs O(1 + log(1 + k)).
template<class T, class U, class Proj = std::identity>
[[nodiscard]] std::size_t lower_bound_from_front(std::span<T const> const range, U const& value, Proj proj = {})
    noexcept(noexcept(std::invoke(proj, range[0]) < value))
{
    std::size_t lo = 0;
    std::size_t hi = 1;
    while (hi < range.size() && std::invoke(proj, range[hi]) < value) {
        lo = hi;
        hi *= 2;
    }
    auto const sub = range.subspan(lo, std::min(hi, range.size()) - lo);
    return lo + static_cast<std::size_t>(std::ranges::lower_bound(sub, value, {}, proj) - sub.begin());
}

// Candidates stay slot-sorted; each word's spans stay position-sorted.
class search_result_cache
{
public:
    struct span_range
    {
        std::uint32_t begin = 0;
        std::uint32_t end = 0;
    };

    void reset(std::size_t const word_count) noexcept
    {
        word_count_ = word_count;
        slots_.clear();
        ranges_.clear();
        spans_.clear();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return slots_.empty();
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return slots_.size();
    }

    [[nodiscard]] document_slot slot(std::size_t const candidate) const noexcept
    {
        return slots_[candidate];
    }

    [[nodiscard]] std::span<interval<int> const> spans(std::size_t const candidate, std::size_t const word) const noexcept
    {
        auto const& range = ranges_[candidate * word_count_ + word];
        return std::span{spans_}.subspan(range.begin, range.end - range.begin);
    }

    // A gram at `offset` in the word occurring at `pos` implies a span starting at `pos - offset`.

    // Must be called in ascending order of `doc_slot`
    void add_candidate(
        document_slot const doc_slot, std::size_t const word, std::span<int const> const positions,
        int const offset, int const length
    )
    {
        assert(slots_.empty() || slots_.back() < doc_slot);
        auto const range = this->append_spans(positions, offset, length);
        if (range.begin == range.end) return;

        slots_.emplace_back(doc_slot);
        ranges_.resize(ranges_.size() + word_count_);
        ranges_[ranges_.size() - word_count_ + word] = range;
    }

    // Keeps the candidates having a posting in `list` for which `f(candidate, positions)` returns true
    template<class F>
    void narrow(posting_list const& list, F&& f)
    {
        std::span<posting const> postings = list.postings;
        std::size_t out = 0;
        for (std::size_t in = 0; in < slots_.size(); ++in) {
            postings = postings.subspan(lower_bound_from_front(postings, slots_[in], &posting::doc_slot));
            if (postings.empty()) break;
            if (postings.front().doc_slot != slots_[in]) continue;
            if (!f(in, list.positions(postings.front()))) continue;

            if (out != in) {
                slots_[out] = slots_[in];
                std::copy_n(ranges_.begin() + in * word_count_, word_count_, ranges_.begin() + out * word_count_);
            }
            ++out;
        }
        slots_.resize(out);
        ranges_.resize(out * word_count_);
    }

    [[nodiscard]] bool set_word_spans(
        std::size_t const candidate, std::size_t const word, std::span<int const> const positions,
        int const offset, int const length
    )
    {
        assert(word < word_count_);
        auto const range = this->append_spans(positions, offset, length);
        ranges_[candidate * word_count_ + word] = range;
        return range.begin != range.end;
    }

    [[nodiscard]] bool keep_word_spans(
        std::size_t const candidate, std::size_t const word, std::span<int const> positions, int const offset
    ) noexcept
    {
        auto& range = ranges_[candidate * word_count_ + word];
        auto out = range.begin;
        for (auto in = range.begin; in < range.end; ++in) {
            int const pos = spans_[in].lower + offset;
            positions = positions.subspan(lower_bound_from_front(positions, pos));
            if (positions.empty()) break;
            if (positions.front() != pos) continue;
            spans_[out++] = spans_[in];
        }
        range.end = out;
        return range.begin != range.end;
    }

    // Runs after all grams are checked so that a span failing later cannot hide a match
    void remove_overlapping_word_spans(std::size_t const word) noexcept
    {
        for (std::size_t candidate = 0; candidate < slots_.size(); ++candidate) {
            auto& range = ranges_[candidate * word_count_ + word];
            auto out = range.begin;
            for (auto in = range.begin; in < range.end; ++in) {
                if (out != range.begin && spans_[in].lower < spans_[out - 1].upper) continue;
                spans_[out++] = spans_[in];
            }
            range.end = out;
        }
    }

private:
    [[nodiscard]] span_range append_spans(std::span<int const> const positions, int const offset, int const length)
    {
        assert(0 <= offset && offset < length);

        if (positions.size() > std::numeric_limits<std::uint32_t>::max() - spans_.size()) {
            throw std::length_error{"too many spans in the search"};
        }
        auto const begin = static_cast<std::uint32_t>(spans_.size());
        for (int const pos : positions) {
            if (pos < offset) continue;
            int const start = pos - offset;
            if (start > std::numeric_limits<int>::max() - length) break;
            spans_.emplace_back(start, start + length);
        }
        return {begin, static_cast<std::uint32_t>(spans_.size())};
    }

    std::size_t word_count_ = 0;
    std::vector<document_slot> slots_;

    // `ranges_[candidate * word_count_ + word]` refers to `spans_`
    std::vector<span_range> ranges_;

    // Spans removed by `keep_word_spans()` leave holes
    std::vector<interval<int>> spans_;
};

} // iris::ngram::detail

#endif

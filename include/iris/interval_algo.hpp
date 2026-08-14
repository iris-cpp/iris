#ifndef IRIS_ZZ_INTERVAL_ALGO_HPP
#define IRIS_ZZ_INTERVAL_ALGO_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/interval.hpp>

#include <concepts>
#include <type_traits>
#include <algorithm>
#include <ranges>
#include <limits>
#include <vector>
#include <map>
#include <set>

#include <cstddef>

namespace iris {

namespace detail {

template<class IntervalT, class WordID = int>
struct interval_word_candidate
{
    IntervalT span;
    WordID word_id;

    [[nodiscard]] constexpr bool operator==(interval_word_candidate const&) const noexcept = default;
};

// Typically: `std::vector<IntervalT>`
template<class R, class IntervalT>
concept IntervalCandidateList =
    std::ranges::forward_range<R> &&
    std::convertible_to<std::ranges::range_value_t<R>, IntervalT>;

// Typically: `std::vector<std::vector<IntervalT>>`
template<class R, class IntervalT>
concept WordCandidateList =
    std::ranges::forward_range<R> &&
    IntervalCandidateList<std::ranges::range_value_t<R>, IntervalT>;

} // detail


// Selects one span per word so that the extent of the selection (the hull
// from the smallest `lower` to the largest `upper`) is minimal, i.e. the chosen
// spans lie as close together as possible.
//
// Input (`words`) is typically a range of ranges that has the value type of `IntervalT`,
// for example `std::vector<std::vector<iris::interval<int>>>`.
//
// The result is sorted by `lower`. Overlapping selections are merged into one
// interval; touching ones stay separate.
//
// Some corner cases:
//   - Words without spans are skipped.
//   - Ties prefer the leftmost selection.
template<
    class WordsR,
    class IntervalT = std::ranges::range_value_t<std::ranges::range_value_t<WordsR>>,
    class IntervalListT = std::vector<IntervalT>,
    class WordID = int
>
    requires detail::WordCandidateList<WordsR, IntervalT>
[[nodiscard]]
IntervalListT select_min_extent(WordsR&& words)
{
    static_assert(std::same_as<std::ranges::range_value_t<IntervalListT>, IntervalT>);
    static_assert(std::convertible_to<WordID, std::size_t>);

    using Cand = detail::interval_word_candidate<IntervalT, WordID>;
    using interval_value_type = IntervalT::value_type;

    std::vector<Cand> cands;
    int active = 0;
    {
        WordID word_id = WordID{0};
        for (auto word_it = std::ranges::begin(words); word_it != std::ranges::end(words); ++word_it, ++word_id) {
            if (std::ranges::empty(*word_it)) continue; // word matched nowhere: skip

            ++active;
            for (IntervalT const& span : *word_it) {
                cands.emplace_back(span, word_id);
            }
        }
    }
    if (active == 0) return {};

    // Descending by lower: the sweep adds spans as the threshold t moves left.
    std::ranges::sort(cands, [](Cand const& a, Cand const& b) {
        return a.span.lower > b.span.lower;
    });

    std::map<WordID, interval_value_type> suffmin; // word -> min upper with lower >= t
    std::multiset<interval_value_type> uppers; // current suffmin values

    using score_type = std::common_type_t<long long, interval_value_type>;
    score_type best_score = std::numeric_limits<score_type>::max();
    interval_value_type best_t = 0;

    for (std::size_t i = 0; i < cands.size(); ) {
        interval_value_type const t = cands[i].span.lower;
        for (; i < cands.size() && cands[i].span.lower == t; ++i) {
            auto const [it, fresh] = suffmin.try_emplace(cands[i].word_id, cands[i].span.upper);
            if (fresh) {
                uppers.insert(cands[i].span.upper);

            } else if (cands[i].span.upper < it->second) {
                uppers.erase(uppers.find(it->second));
                it->second = cands[i].span.upper;
                uppers.insert(cands[i].span.upper);
            }
        }
        if (int(suffmin.size()) == active) {
            score_type const score = static_cast<score_type>(*uppers.rbegin()) - t;
            if (score <= best_score) {
                best_score = score;
                best_t = t; // leftmost tie-break
            }
        }
    }

    // Reconstruct: per word, the span with lower >= best_t minimizing (upper, lower).
    std::map<WordID, IntervalT> chosen;
    for (Cand const& cand : cands) {
        if (cand.span.lower < best_t) continue;

        auto const [it, fresh] = chosen.try_emplace(cand.word_id, cand.span);
        if (
            !fresh &&
            (
                cand.span.upper < it->second.upper ||
                (cand.span.upper == it->second.upper && cand.span.lower < it->second.lower)
            )
        ) {
            it->second = cand.span;
        }
    }

    std::vector<IntervalT> out;
    for (auto const& [word_id, span] : chosen) {
        out.emplace_back(span);
    }
    std::ranges::sort(out, [](IntervalT const& a, IntervalT const& b) {
        return a.lower != b.lower ? a.lower < b.lower : a.upper < b.upper;
    });

    // Different words' winners may overlap (substring terms): merge strict overlaps,
    // dedupe identical spans; touching winners stay separate matches.
    IntervalListT merged;
    for (auto const& span : out) {
        if (
            !merged.empty() &&
            (span.lower < merged.back().upper || span == merged.back())
        ) {
            merged.back().upper = std::max(merged.back().upper, span.upper);

        } else {
            merged.emplace_back(span);
        }
    }
    return merged;
}

} // iris

#endif

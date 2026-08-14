// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/interval.hpp>
#include <iris/interval_algo.hpp>

#include <random>
#include <algorithm>
#include <limits>
#include <vector>
#include <format>
#include <utility>

using namespace std::string_literals;
using namespace std::string_view_literals;

using iris::interval;
using I = interval<int>;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("interval_algo: select_min_extent")
{
    using ivec = std::vector<interval<int>>;
    using words_t = std::vector<ivec>;

    // The closest candidate of a multi-span word is chosen
    CHECK(iris::select_min_extent(words_t{{{10, 13}}, {{15, 17}}, {{0, 10}, {200, 210}}})
        == ivec{{0, 10}, {10, 13}, {15, 17}});

    // The redundant wide span of word 0 must not inflate the hull
    CHECK(iris::select_min_extent(words_t{{{10, 12}, {11, 200}}, {{11, 13}}})
        == ivec{{10, 13}}); // overlapping winners merge into one match

    // Two equally tight clusters: the leftmost wins
    CHECK(iris::select_min_extent(words_t{{{0, 2}, {100, 102}}, {{5, 7}, {105, 107}}})
        == ivec{{0, 2}, {5, 7}});

    // A word without candidates is skipped
    CHECK(iris::select_min_extent(words_t{{}, {{5, 7}}}) == ivec{{5, 7}});

    CHECK(iris::select_min_extent(words_t{{}, {}}).empty());
    CHECK(iris::select_min_extent(words_t{}).empty());

    // Single word: the shortest span wins
    CHECK(iris::select_min_extent(words_t{{{8, 12}, {3, 5}, {20, 21}}}) == ivec{{20, 21}});

    // Equal lengths: the leftmost wins
    CHECK(iris::select_min_extent(words_t{{{10, 11}, {5, 6}}}) == ivec{{5, 6}});

    // Identical spans from different words are deduplicated
    CHECK(iris::select_min_extent(words_t{{{7, 9}}, {{7, 9}}}) == ivec{{7, 9}});

    // Touching winners stay separate matches
    CHECK(iris::select_min_extent(words_t{{{0, 5}}, {{5, 9}}}) == ivec{{0, 5}, {5, 9}});

    // A caret is a valid winner
    CHECK(iris::select_min_extent(words_t{{{10, 10}}, {{12, 14}}})
        == ivec{{10, 10}, {12, 14}});

    // A caret inside another word's winner is absorbed by the merge
    CHECK(iris::select_min_extent(words_t{{{10, 10}}, {{8, 12}}}) == ivec{{8, 12}});
}

TEST_CASE("interval_algo: select_min_extent (brute hull)")
{
    // Brute force over every one-span-per-word combination:
    //   - The selector's hull must match the minimum
    //   - Its output must be sorted and non-overlapping (touching allowed)

    auto const brute_hull = [](std::vector<std::vector<I>> const& words) -> long long {
        std::vector<std::vector<I>> lists;
        for (auto const& w : words) {
            if (!w.empty()) {
                lists.emplace_back(w);
            }
        }
        if (lists.empty()) return -1;

        long long best = std::numeric_limits<long long>::max();
        std::vector<std::size_t> idx(lists.size(), 0);
        while (true) {
            int lo = std::numeric_limits<int>::max();
            int up = std::numeric_limits<int>::min();
            for (std::size_t k = 0; k < lists.size(); ++k) {
                lo = std::min(lo, lists[k][idx[k]].lower);
                up = std::max(up, lists[k][idx[k]].upper);
            }
            best = std::min(best, static_cast<long long>(std::max(up, lo)) - lo);

            std::size_t k = 0;
            while (k < lists.size() && ++idx[k] == lists[k].size()) {
                idx[k++] = 0;
            }
            if (k == lists.size()) break;
        }
        return best;
    };

    std::mt19937 rng(37);

    for (int i = 0; i < 20000; ++i) {
        std::vector<std::vector<I>> words(1 + rng() % 4);
        for (auto& w : words) {
            std::size_t const k = rng() % 5; // zero-candidate words included
            for (std::size_t j = 0; j < k; ++j) {
                int const lo = static_cast<int>(rng() % 40);
                w.emplace_back(lo, lo + static_cast<int>(rng() % 7)); // carets included
            }
        }
        auto const out = iris::select_min_extent(words);
        long long const want = brute_hull(words);
        if (want < 0) {
            CHECK(out.empty());
            continue;
        }
        REQUIRE(!out.empty());
        CHECK(static_cast<long long>(out.back().upper) - out.front().lower == want);
        for (std::size_t k = 1; k < out.size(); ++k) {
            CHECK(out[k - 1].upper <= out[k].lower);
        }
    }
}

// NOLINTEND(readability-container-size-empty)

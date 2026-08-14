// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/interval.hpp>
#include <iris/interval_set.hpp>

#include <string_view>
#include <type_traits>
#include <utility>
#include <iterator>
#include <ranges>
#include <vector>

using namespace std::string_view_literals;
using iris::interval;
using IVS = iris::interval_set<interval<int>>;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("interval_set: type_traits")
{
    STATIC_CHECK(std::bidirectional_iterator<IVS::iterator>);
    STATIC_CHECK(std::bidirectional_iterator<IVS::const_iterator>);

    STATIC_CHECK(std::ranges::bidirectional_range<IVS>);
    STATIC_CHECK(std::ranges::sized_range<IVS>);

    STATIC_CHECK(std::is_default_constructible_v<IVS>);
    STATIC_CHECK(std::is_nothrow_destructible_v<IVS>);
    STATIC_CHECK(std::is_copy_constructible_v<IVS>);
    STATIC_CHECK(std::is_move_constructible_v<IVS>);
    STATIC_CHECK(std::is_nothrow_move_constructible_v<IVS> == std::is_nothrow_move_constructible_v<IVS::map_type>);
    STATIC_CHECK(std::is_nothrow_move_assignable_v<IVS> == std::is_nothrow_move_assignable_v<IVS::map_type>);
    STATIC_CHECK(std::is_copy_assignable_v<IVS>);
    STATIC_CHECK(std::is_nothrow_swappable_v<IVS>);

    STATIC_CHECK(!std::is_constructible_v<IVS, interval<int>>);
    STATIC_CHECK(std::is_constructible_v<IVS, std::initializer_list<interval<int>>>);
}

TEST_CASE("interval_set: construction")
{
    {
        STATIC_CHECK(std::is_constructible_v<IVS, std::vector<interval<int>>::iterator, std::vector<interval<int>>::iterator>);
        std::vector<interval<int>> v;
        [[maybe_unused]] IVS ivs{v.begin(), v.end()};
    }

    {
        STATIC_CHECK(std::is_constructible_v<IVS, std::from_range_t, std::vector<interval<int>>>);
        [[maybe_unused]] IVS ivs{std::from_range, std::vector<interval<int>>{}};
    }

    {
        std::vector<int> ivec;
        auto view = ivec | std::views::transform([](int const lower) -> interval<int> {
            return {lower, lower + 1};
        });
        using View = decltype(view);
        STATIC_CHECK(std::is_constructible_v<IVS, std::from_range_t, View&>);
        STATIC_CHECK(std::is_constructible_v<IVS, std::from_range_t, View const&>);
        STATIC_CHECK(std::is_constructible_v<IVS, std::from_range_t, View>);

        IVS const ivs{std::from_range, view};
        CHECK(ivs.empty());
    }
    {
        std::vector<int> ivec{0};
        auto view = ivec | std::views::transform([](int const lower) -> interval<int> {
            return {lower, lower + 1};
        });
        IVS const ivs{std::from_range, view};
        CHECK(ivs.size() == 1);
        CHECK(ivs == IVS{{0, 1}});
    }
    {
        std::vector<int> ivec{0};
        auto view = ivec | std::views::transform([](int const lower) -> interval<int> {
            return {lower, lower};
        });
        IVS const ivs{std::from_range, view};
        CHECK(ivs.empty());
    }

    {
        std::vector<int> ivec{0, 4, 8};
        auto view = ivec | std::views::transform([](int const lower) -> interval<int> {
            return {lower, lower + 1};
        });

        IVS const ivs{std::from_range, view};
        CHECK(ivs.size() == 3);
        CHECK(ivs == IVS{{0, 1}, {4, 5}, {8, 9}});
    }
    {
        std::vector<int> ivec{0, 1, 2};
        auto view = ivec | std::views::transform([](int const lower) -> interval<int> {
            return {lower, lower + 1};
        });
        IVS const ivs{std::from_range, view};
        CHECK(ivs.size() == 1);
        CHECK(ivs == IVS{{0, 3}});
    }
}

TEST_CASE("interval_set: identity")
{
    {
        IVS ivs;
        CHECK(ivs.size() == 0);
        CHECK(ivs.empty());
        CHECK(ivs.begin() == ivs.end());
        CHECK(ivs.extent() == interval<int>{});
        CHECK(ivs == ivs);
        CHECK((ivs <=> ivs) == std::strong_ordering::equal);
    }

    {
        IVS ivs{{0, 0}};
        CHECK(ivs.size() == 0);
        CHECK(ivs.empty());
        CHECK(ivs.begin() == ivs.end());
        CHECK(ivs.extent() == interval<int>{});
        CHECK(ivs == ivs);
        CHECK((ivs <=> ivs) == std::strong_ordering::equal);
    }
    {
        IVS ivs{{1, 1}};
        CHECK(ivs.size() == 0);
        CHECK(ivs.empty());
        CHECK(ivs.begin() == ivs.end());
        CHECK(ivs.extent() == interval<int>{}); // not {1, 1}
        CHECK(ivs == ivs);
        CHECK((ivs <=> ivs) == std::strong_ordering::equal);
    }

    {
        IVS ivs{{0, 1}};
        CHECK(ivs.size() == 1);
        CHECK(!ivs.empty());
        CHECK(ivs.begin() != ivs.end());
        CHECK(std::distance(ivs.begin(), ivs.end()) == 1);
        CHECK(ivs == ivs);
        CHECK((ivs <=> ivs) == std::strong_ordering::equal);
    }
}

TEST_CASE("interval_set: extent")
{
    CHECK(IVS{}.extent() == interval{0, 0});
    CHECK(IVS{{0, 0}}.extent() == interval{0, 0});
    CHECK(IVS{{1, 1}}.extent() == interval{0, 0});

    CHECK(IVS{{2, 5}}.extent() == interval{2, 5});

    CHECK(IVS{{2, 5}, {0, 1}}.extent() == interval{0, 5});
    CHECK(IVS{{2, 5}, {0, 2}}.extent() == interval{0, 5});
    CHECK(IVS{{2, 5}, {0, 3}}.extent() == interval{0, 5});
    CHECK(IVS{{2, 5}, {0, 4}}.extent() == interval{0, 5});
    CHECK(IVS{{2, 5}, {0, 5}}.extent() == interval{0, 5});
    CHECK(IVS{{2, 5}, {0, 6}}.extent() == interval{0, 6});
    CHECK(IVS{{2, 5}, {0, 7}}.extent() == interval{0, 7});

    CHECK(IVS{{2, 5}, {1, 2}}.extent() == interval{1, 5});
    CHECK(IVS{{2, 5}, {1, 3}}.extent() == interval{1, 5});
    CHECK(IVS{{2, 5}, {1, 4}}.extent() == interval{1, 5});
    CHECK(IVS{{2, 5}, {1, 5}}.extent() == interval{1, 5});
    CHECK(IVS{{2, 5}, {1, 6}}.extent() == interval{1, 6});
    CHECK(IVS{{2, 5}, {1, 7}}.extent() == interval{1, 7});

    CHECK(IVS{{2, 5}, {2, 3}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {2, 4}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {2, 5}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {2, 6}}.extent() == interval{2, 6});
    CHECK(IVS{{2, 5}, {2, 7}}.extent() == interval{2, 7});

    CHECK(IVS{{2, 5}, {3, 4}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {3, 5}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {3, 6}}.extent() == interval{2, 6});
    CHECK(IVS{{2, 5}, {3, 7}}.extent() == interval{2, 7});

    CHECK(IVS{{2, 5}, {4, 5}}.extent() == interval{2, 5});
    CHECK(IVS{{2, 5}, {4, 6}}.extent() == interval{2, 6});
    CHECK(IVS{{2, 5}, {4, 7}}.extent() == interval{2, 7});

    CHECK(IVS{{2, 5}, {5, 6}}.extent() == interval{2, 6});
    CHECK(IVS{{2, 5}, {5, 7}}.extent() == interval{2, 7});

    CHECK(IVS{{2, 5}, {6, 7}}.extent() == interval{2, 7});

    CHECK(IVS{{-5, -3}, {4, 10}}.extent() == interval{-5, 10});
    CHECK(IVS{{-5, -3}, {-1, 2}, {4, 10}}.extent() == interval{-5, 10});
}

#define IRIS_CHECK_REL(rel, a0, a1, b0, b1) \
    CHECK(interval a0, a1 .rel(b0, b1) == IVS{a0, a1}.rel(b0, b1))

#define IRIS_CHECK_REL_P(rel, a0, a1, p) \
    CHECK(interval a0, a1 .rel(p) == IVS{a0, a1}.rel(p))

TEST_CASE("interval_set: relationship")
{
    IRIS_CHECK_REL(intersects, {2, 5}, {0, 1});
    IRIS_CHECK_REL(intersects, {2, 5}, {0, 2});
    IRIS_CHECK_REL(intersects, {2, 5}, {0, 3});
    IRIS_CHECK_REL(intersects, {2, 5}, {4, 7});
    IRIS_CHECK_REL(intersects, {2, 5}, {5, 7});
    IRIS_CHECK_REL(intersects, {2, 5}, {6, 7});

    // b = empty interval
    IRIS_CHECK_REL(intersects, {2, 5}, {0, 0});
    IRIS_CHECK_REL(intersects, {2, 5}, {1, 1});
    IRIS_CHECK_REL(intersects, {2, 5}, {2, 2});
    IRIS_CHECK_REL(intersects, {2, 5}, {3, 3});
    IRIS_CHECK_REL(intersects, {2, 5}, {4, 4});
    IRIS_CHECK_REL(intersects, {2, 5}, {5, 5});
    IRIS_CHECK_REL(intersects, {2, 5}, {6, 6});

    // ----------------------------------------------------

    IRIS_CHECK_REL(covers, {2, 5}, {0, 1});
    IRIS_CHECK_REL(covers, {2, 5}, {0, 2});
    IRIS_CHECK_REL(covers, {2, 5}, {0, 3});

    IRIS_CHECK_REL(covers, {2, 5}, {1, 2});
    IRIS_CHECK_REL(covers, {2, 5}, {1, 3});

    IRIS_CHECK_REL(covers, {2, 5}, {2, 3});
    IRIS_CHECK_REL(covers, {2, 5}, {2, 4});
    IRIS_CHECK_REL(covers, {2, 5}, {2, 5});
    IRIS_CHECK_REL(covers, {2, 5}, {2, 6});

    IRIS_CHECK_REL(covers, {2, 5}, {3, 4});
    IRIS_CHECK_REL(covers, {2, 5}, {3, 5});
    IRIS_CHECK_REL(covers, {2, 5}, {3, 6});

    IRIS_CHECK_REL(covers, {2, 5}, {4, 5});
    IRIS_CHECK_REL(covers, {2, 5}, {4, 6});

    IRIS_CHECK_REL(covers, {2, 5}, {5, 6});

    IRIS_CHECK_REL(covers, {2, 5}, {6, 7});

    // b = empty interval
    IRIS_CHECK_REL(covers, {2, 5}, {0, 0});
    IRIS_CHECK_REL(covers, {2, 5}, {1, 1});
    IRIS_CHECK_REL(covers, {2, 5}, {2, 2});
    IRIS_CHECK_REL(covers, {2, 5}, {3, 3});
    IRIS_CHECK_REL(covers, {2, 5}, {4, 4});
    IRIS_CHECK_REL(covers, {2, 5}, {5, 5});
    IRIS_CHECK_REL(covers, {2, 5}, {6, 6});

    // ----------------------------------------------------

    IRIS_CHECK_REL_P(contains, {2, 5}, 0);
    IRIS_CHECK_REL_P(contains, {2, 5}, 1);
    IRIS_CHECK_REL_P(contains, {2, 5}, 2);
    IRIS_CHECK_REL_P(contains, {2, 5}, 3);
    IRIS_CHECK_REL_P(contains, {2, 5}, 4);
    IRIS_CHECK_REL_P(contains, {2, 5}, 5);
    IRIS_CHECK_REL_P(contains, {2, 5}, 6);
}

TEST_CASE("interval_set: insertion")
{
    // Insertion of empty interval is no-op
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 0});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 1});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 2});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({3, 3});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({4, 4});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({5, 5});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({6, 6});
        CHECK(ivs == IVS{{2, 5}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 1});
        CHECK(ivs == IVS{{0, 1}, {2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 2});
        CHECK(ivs == IVS{{0, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 3});
        CHECK(ivs == IVS{{0, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 4});
        CHECK(ivs == IVS{{0, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 5});
        CHECK(ivs == IVS{{0, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 6});
        CHECK(ivs == IVS{{0, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({0, 7});
        CHECK(ivs == IVS{{0, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 2});
        CHECK(ivs == IVS{{1, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 3});
        CHECK(ivs == IVS{{1, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 4});
        CHECK(ivs == IVS{{1, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 5});
        CHECK(ivs == IVS{{1, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 6});
        CHECK(ivs == IVS{{1, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({1, 7});
        CHECK(ivs == IVS{{1, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 3});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 4});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 5});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 6});
        CHECK(ivs == IVS{{2, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({2, 7});
        CHECK(ivs == IVS{{2, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({3, 4});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({3, 5});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({3, 6});
        CHECK(ivs == IVS{{2, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({3, 7});
        CHECK(ivs == IVS{{2, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({4, 5});
        CHECK(ivs == IVS{{2, 5}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({4, 6});
        CHECK(ivs == IVS{{2, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({4, 7});
        CHECK(ivs == IVS{{2, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({5, 6});
        CHECK(ivs == IVS{{2, 6}});
    }
    {
        IVS ivs{{2, 5}}; ivs.insert({5, 7});
        CHECK(ivs == IVS{{2, 7}});
    }
    // --------------------------------------------------------------
    {
        IVS ivs{{2, 5}}; ivs.insert({6, 7});
        CHECK(ivs == IVS{{2, 5}, {6, 7}});
    }
}

TEST_CASE("interval_set: format")
{
    CHECK(std::format("{}", IVS{}) == "{}"sv);
    CHECK(std::format("{}", IVS{{0, 1}}) == "{[0,1)}"sv);
    CHECK(std::format("{}", IVS{{0, 1}, {2, 3}}) == "{[0,1) [2,3)}"sv);
    CHECK(std::format("{:2d,3d}", IVS{{0, 1}, {2, 3}}) == "{[ 0,  1) [ 2,  3)}"sv);
}

// NOLINTEND(readability-container-size-empty)

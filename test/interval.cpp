// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/interval.hpp>

#include <format>
#include <utility>
#include <concepts>
#include <type_traits>

using namespace std::string_view_literals;

using iris::interval;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("interval: type traits")
{
    STATIC_CHECK(std::is_trivially_copyable_v<interval<int>>);

    STATIC_CHECK(std::is_nothrow_default_constructible_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_constructible_v<interval<int>, int, int>);
    STATIC_CHECK(std::is_nothrow_copy_constructible_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_move_constructible_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_copy_assignable_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_move_assignable_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_destructible_v<interval<int>>);
    STATIC_CHECK(std::is_nothrow_swappable_v<interval<int>>);
}

TEST_CASE("interval: members")
{
    // Check in constexpr context to detect uninitialized value and other UBs

    // [x, x)
    {
        constexpr interval<int> iv{};
        STATIC_CHECK(iv.lower == 0);
        STATIC_CHECK(iv.upper == 0);
        STATIC_CHECK(iv.length() == 0);
        STATIC_CHECK(iv.empty());
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }

    // [-, -)
    {
        constexpr interval iv{-5, -2};
        STATIC_CHECK(iv.lower == -5);
        STATIC_CHECK(iv.upper == -2);
        STATIC_CHECK(iv.length() == 3);
        STATIC_CHECK(!iv.empty());
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }

    // [-, +)
    {
        constexpr interval iv{-5, 2};
        STATIC_CHECK(iv.lower == -5);
        STATIC_CHECK(iv.upper == 2);
        STATIC_CHECK(iv.length() == 7);
        STATIC_CHECK(!iv.empty());
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }

    // [+, +)
    {
        constexpr interval iv{2, 5};
        STATIC_CHECK(iv.lower == 2);
        STATIC_CHECK(iv.upper == 5);
        STATIC_CHECK(iv.length() == 3);
        STATIC_CHECK(!iv.empty());
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }
}

TEST_CASE("interval: relationship")
{
    STATIC_CHECK(interval{2, 5}.equals(interval{2, 5}));
    STATIC_CHECK(interval{2, 5} == interval{2, 5});
    STATIC_CHECK((interval{2, 5} <=> interval{2, 5}) == std::strong_ordering::equal);

    // b = empty interval
    STATIC_CHECK(interval{0, 0}.equals(interval{0, 0}));
    STATIC_CHECK(interval{0, 0}.equals(interval{1, 1}));
    STATIC_CHECK(interval{0, 0} == interval{0, 0});
    STATIC_CHECK(interval{0, 0} != interval{1, 1});
    STATIC_CHECK((interval{0, 0} <=> interval{0, 0}) == std::strong_ordering::equal);
    STATIC_CHECK((interval{0, 0} <=> interval{1, 1}) == std::strong_ordering::less);

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.intersects({0, 1})); STATIC_CHECK( interval{2, 5}.disjoint({0, 1}));
    STATIC_CHECK(!interval{2, 5}.intersects({0, 2})); STATIC_CHECK( interval{2, 5}.disjoint({0, 2}));
    STATIC_CHECK( interval{2, 5}.intersects({0, 3})); STATIC_CHECK(!interval{2, 5}.disjoint({0, 3}));
    STATIC_CHECK( interval{2, 5}.intersects({4, 7})); STATIC_CHECK(!interval{2, 5}.disjoint({4, 7}));
    STATIC_CHECK(!interval{2, 5}.intersects({5, 7})); STATIC_CHECK( interval{2, 5}.disjoint({5, 7}));
    STATIC_CHECK(!interval{2, 5}.intersects({6, 7})); STATIC_CHECK( interval{2, 5}.disjoint({6, 7}));

    // b = empty interval
    STATIC_CHECK(!interval{2, 5}.intersects({0, 0})); STATIC_CHECK( interval{2, 5}.disjoint({0, 0}));
    STATIC_CHECK(!interval{2, 5}.intersects({1, 1})); STATIC_CHECK( interval{2, 5}.disjoint({1, 1}));
    STATIC_CHECK(!interval{2, 5}.intersects({2, 2})); STATIC_CHECK( interval{2, 5}.disjoint({2, 2}));
    STATIC_CHECK(!interval{2, 5}.intersects({3, 3})); STATIC_CHECK( interval{2, 5}.disjoint({3, 3}));
    STATIC_CHECK(!interval{2, 5}.intersects({4, 4})); STATIC_CHECK( interval{2, 5}.disjoint({4, 4}));
    STATIC_CHECK(!interval{2, 5}.intersects({5, 5})); STATIC_CHECK( interval{2, 5}.disjoint({5, 5}));
    STATIC_CHECK(!interval{2, 5}.intersects({6, 6})); STATIC_CHECK( interval{2, 5}.disjoint({6, 6}));

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.touches({0, 1}));
    STATIC_CHECK( interval{2, 5}.touches({0, 2}));
    STATIC_CHECK(!interval{2, 5}.touches({0, 3}));
    STATIC_CHECK(!interval{2, 5}.touches({4, 7}));
    STATIC_CHECK( interval{2, 5}.touches({5, 7}));
    STATIC_CHECK(!interval{2, 5}.touches({6, 7}));

    // b = empty interval
    STATIC_CHECK(!interval{2, 5}.touches({0, 0}));
    STATIC_CHECK(!interval{2, 5}.touches({1, 1}));
    STATIC_CHECK(!interval{2, 5}.touches({2, 2}));
    STATIC_CHECK(!interval{2, 5}.touches({3, 3}));
    STATIC_CHECK(!interval{2, 5}.touches({4, 4}));
    STATIC_CHECK(!interval{2, 5}.touches({5, 5}));
    STATIC_CHECK(!interval{2, 5}.touches({6, 6}));

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.connected({0, 1}));
    STATIC_CHECK( interval{2, 5}.connected({0, 2}));
    STATIC_CHECK( interval{2, 5}.connected({0, 3}));
    STATIC_CHECK( interval{2, 5}.connected({4, 7}));
    STATIC_CHECK( interval{2, 5}.connected({5, 7}));
    STATIC_CHECK(!interval{2, 5}.connected({6, 7}));

    // b = empty interval
    STATIC_CHECK(!interval{2, 5}.connected({0, 0}));
    STATIC_CHECK(!interval{2, 5}.connected({1, 1}));
    STATIC_CHECK(!interval{2, 5}.connected({2, 2}));
    STATIC_CHECK(!interval{2, 5}.connected({3, 3}));
    STATIC_CHECK(!interval{2, 5}.connected({4, 4}));
    STATIC_CHECK(!interval{2, 5}.connected({5, 5}));
    STATIC_CHECK(!interval{2, 5}.connected({6, 6}));

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.covers({0, 1}));
    STATIC_CHECK(!interval{2, 5}.covers({0, 2}));
    STATIC_CHECK(!interval{2, 5}.covers({0, 3}));

    STATIC_CHECK(!interval{2, 5}.covers({1, 2}));
    STATIC_CHECK(!interval{2, 5}.covers({1, 3}));

    STATIC_CHECK( interval{2, 5}.covers({2, 3}));
    STATIC_CHECK( interval{2, 5}.covers({2, 4}));
    STATIC_CHECK( interval{2, 5}.covers({2, 5}));
    STATIC_CHECK(!interval{2, 5}.covers({2, 6}));

    STATIC_CHECK( interval{2, 5}.covers({3, 4}));
    STATIC_CHECK( interval{2, 5}.covers({3, 5}));
    STATIC_CHECK(!interval{2, 5}.covers({3, 6}));

    STATIC_CHECK( interval{2, 5}.covers({4, 5}));
    STATIC_CHECK(!interval{2, 5}.covers({4, 6}));

    STATIC_CHECK(!interval{2, 5}.covers({5, 6}));

    STATIC_CHECK(!interval{2, 5}.covers({6, 7}));

    // b = empty interval
    STATIC_CHECK( interval{2, 5}.covers({0, 0}));
    STATIC_CHECK( interval{2, 5}.covers({1, 1}));
    STATIC_CHECK( interval{2, 5}.covers({2, 2}));
    STATIC_CHECK( interval{2, 5}.covers({3, 3}));
    STATIC_CHECK( interval{2, 5}.covers({4, 4}));
    STATIC_CHECK( interval{2, 5}.covers({5, 5}));
    STATIC_CHECK( interval{2, 5}.covers({6, 6}));

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.contains(0));
    STATIC_CHECK(!interval{2, 5}.contains(1));
    STATIC_CHECK( interval{2, 5}.contains(2));
    STATIC_CHECK( interval{2, 5}.contains(3));
    STATIC_CHECK( interval{2, 5}.contains(4));
    STATIC_CHECK(!interval{2, 5}.contains(5));
    STATIC_CHECK(!interval{2, 5}.contains(6));
}

TEST_CASE("interval: tuple")
{
    {
        interval const iv{1, 2};
        auto const [lower, upper] = iv; // structured bindings
        CHECK(lower == 1);
        CHECK(upper == 2);
    }

    {
        interval iv{1, 2};
        auto&& lower = iris::get<0>(iv);
        STATIC_CHECK(std::same_as<decltype(lower), int&>);
        CHECK(lower == 1);
    }
    {
        interval const iv{1, 2};
        auto&& lower = iris::get<0>(iv);
        STATIC_CHECK(std::same_as<decltype(lower), int const&>);
        CHECK(lower == 1);
    }
    {
        interval iv{1, 2};
        auto&& lower = iris::get<0>(std::move(iv));
        STATIC_CHECK(std::same_as<decltype(lower), int&&>);
        CHECK(lower == 1);
    }
    {
        interval const iv{1, 2};
        auto&& lower = iris::get<0>(std::move(iv));
        STATIC_CHECK(std::same_as<decltype(lower), int const&&>);
        CHECK(lower == 1);
    }
}

TEST_CASE("interval: format")
{
    CHECK(std::format("{}", interval<int>{}) == "[0,0)"sv);
    CHECK(std::format("{}", interval<int>{1, 2}) == "[1,2)"sv);
    CHECK(std::format("{:2d,}", interval<int>{1, 2}) == "[ 1,2)"sv);
    CHECK(std::format("{:2d,3d}", interval<int>{1, 2}) == "[ 1,  2)"sv);
}

// NOLINTEND(readability-container-size-empty)

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/interval.hpp>

#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <vector>
#include <format>
#include <utility>
#include <concepts>
#include <type_traits>

using namespace std::string_literals;
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
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }
    {
        constexpr interval<int> iv{1, 1};
        STATIC_CHECK(iv.lower == 1);
        STATIC_CHECK(iv.upper == 1);
        STATIC_CHECK(iv.length() == 0);
        STATIC_CHECK(iv.empty());
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }
    {
        constexpr interval<int> iv{-1, -1};
        STATIC_CHECK(iv.lower == -1);
        STATIC_CHECK(iv.upper == -1);
        STATIC_CHECK(iv.length() == 0);
        STATIC_CHECK(iv.empty());
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }
    {
        constexpr interval<int> iv{5, -1}; // malformed
        STATIC_CHECK(iv.lower == 5);
        STATIC_CHECK(iv.upper == -1);
        STATIC_CHECK(iv.length() == 0);
        STATIC_CHECK(iv.empty());
        STATIC_CHECK(!iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
        STATIC_CHECK(iv == iv);
        STATIC_CHECK((iv <=> iv) == std::strong_ordering::equal);
    }
    {
        constexpr interval<int> iv{-1, -5}; // malformed
        STATIC_CHECK(iv.lower == -1);
        STATIC_CHECK(iv.upper == -5);
        STATIC_CHECK(iv.length() == 0);
        STATIC_CHECK(iv.empty());
        STATIC_CHECK(!iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
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
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
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
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
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
        STATIC_CHECK(iv.is_proper());
        STATIC_CHECK(iv.equals(iv));
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

    STATIC_CHECK(interval{2, 5}.encloses({0, 1}) == interval{2, 5}.covers({0, 1}));
    STATIC_CHECK(interval{2, 5}.encloses({0, 2}) == interval{2, 5}.covers({0, 2}));
    STATIC_CHECK(interval{2, 5}.encloses({0, 3}) == interval{2, 5}.covers({0, 3}));

    STATIC_CHECK(interval{2, 5}.encloses({1, 2}) == interval{2, 5}.covers({1, 2}));
    STATIC_CHECK(interval{2, 5}.encloses({1, 3}) == interval{2, 5}.covers({1, 3}));

    STATIC_CHECK(interval{2, 5}.encloses({2, 3}) == interval{2, 5}.covers({2, 3}));
    STATIC_CHECK(interval{2, 5}.encloses({2, 4}) == interval{2, 5}.covers({2, 4}));
    STATIC_CHECK(interval{2, 5}.encloses({2, 5}) == interval{2, 5}.covers({2, 5}));
    STATIC_CHECK(interval{2, 5}.encloses({2, 6}) == interval{2, 5}.covers({2, 6}));

    STATIC_CHECK(interval{2, 5}.encloses({3, 4}) == interval{2, 5}.covers({3, 4}));
    STATIC_CHECK(interval{2, 5}.encloses({3, 5}) == interval{2, 5}.covers({3, 5}));
    STATIC_CHECK(interval{2, 5}.encloses({3, 6}) == interval{2, 5}.covers({3, 6}));

    STATIC_CHECK(interval{2, 5}.encloses({4, 5}) == interval{2, 5}.covers({4, 5}));
    STATIC_CHECK(interval{2, 5}.encloses({4, 6}) == interval{2, 5}.covers({4, 6}));

    STATIC_CHECK(interval{2, 5}.encloses({5, 6}) == interval{2, 5}.covers({5, 6}));

    STATIC_CHECK(interval{2, 5}.encloses({6, 7}) == interval{2, 5}.covers({6, 7}));

    // b = empty interval
    STATIC_CHECK(!interval{2, 5}.encloses({0, 0}));
    STATIC_CHECK(!interval{2, 5}.encloses({1, 1}));
    STATIC_CHECK( interval{2, 5}.encloses({2, 2}));
    STATIC_CHECK( interval{2, 5}.encloses({3, 3}));
    STATIC_CHECK( interval{2, 5}.encloses({4, 4}));
    STATIC_CHECK( interval{2, 5}.encloses({5, 5}));
    STATIC_CHECK(!interval{2, 5}.encloses({6, 6}));

    // ----------------------------------------------------

    STATIC_CHECK(!interval{2, 5}.contains(0));
    STATIC_CHECK(!interval{2, 5}.contains(1));
    STATIC_CHECK( interval{2, 5}.contains(2));
    STATIC_CHECK( interval{2, 5}.contains(3));
    STATIC_CHECK( interval{2, 5}.contains(4));
    STATIC_CHECK(!interval{2, 5}.contains(5));
    STATIC_CHECK(!interval{2, 5}.contains(6));

    // ----------------------------------------------------

    STATIC_CHECK( interval{0, 5}.within("abcdef"));
    STATIC_CHECK( interval{0, 6}.within("abcdef"));
    STATIC_CHECK(!interval{0, 7}.within("abcdef"));
    STATIC_CHECK( interval{3, 6}.within("abcdef"));
    STATIC_CHECK( interval{4, 6}.within("abcdef"));
    STATIC_CHECK( interval{5, 6}.within("abcdef"));
    STATIC_CHECK( interval{6, 6}.within("abcdef"));
    STATIC_CHECK(!interval{8, 9}.within("abcdef"));

    STATIC_CHECK( interval{0, 5}.within("abcdef"sv));
    STATIC_CHECK( interval{0, 6}.within("abcdef"sv));
    STATIC_CHECK(!interval{0, 7}.within("abcdef"sv));
    STATIC_CHECK( interval{3, 6}.within("abcdef"sv));
    STATIC_CHECK( interval{4, 6}.within("abcdef"sv));
    STATIC_CHECK( interval{5, 6}.within("abcdef"sv));
    STATIC_CHECK( interval{6, 6}.within("abcdef"sv));
    STATIC_CHECK(!interval{8, 9}.within("abcdef"sv));

    // Empty interval
    STATIC_CHECK( interval{0, 0}.within("abcdef"));
    STATIC_CHECK( interval{1, 1}.within("abcdef"));
    STATIC_CHECK( interval{2, 2}.within("abcdef"));
    STATIC_CHECK( interval{3, 3}.within("abcdef"));
    STATIC_CHECK( interval{4, 4}.within("abcdef"));
    STATIC_CHECK( interval{5, 5}.within("abcdef"));
    STATIC_CHECK( interval{6, 6}.within("abcdef"));
    STATIC_CHECK(!interval{7, 7}.within("abcdef"));

    STATIC_CHECK( interval{0, 0}.within("abcdef"sv));
    STATIC_CHECK( interval{1, 1}.within("abcdef"sv));
    STATIC_CHECK( interval{2, 2}.within("abcdef"sv));
    STATIC_CHECK( interval{3, 3}.within("abcdef"sv));
    STATIC_CHECK( interval{4, 4}.within("abcdef"sv));
    STATIC_CHECK( interval{5, 5}.within("abcdef"sv));
    STATIC_CHECK( interval{6, 6}.within("abcdef"sv));
    STATIC_CHECK(!interval{7, 7}.within("abcdef"sv));

    // Malformed interval
    STATIC_CHECK(!interval{-1, -5}.within("abcdef"));
    STATIC_CHECK(!interval{ 5,  2}.within("abcdef"));
    STATIC_CHECK(!interval{ 2, -5}.within("abcdef"));
}

TEST_CASE("interval: intersection")
{
    STATIC_CHECK(interval(2, 5).intersection({0, 1}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({0, 2}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({0, 3}) == interval{2, 3});
    STATIC_CHECK(interval(2, 5).intersection({0, 4}) == interval{2, 4});
    STATIC_CHECK(interval(2, 5).intersection({0, 5}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({0, 6}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({0, 7}) == interval{2, 5});

    STATIC_CHECK(interval(2, 5).intersection({1, 2}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({1, 3}) == interval{2, 3});
    STATIC_CHECK(interval(2, 5).intersection({1, 4}) == interval{2, 4});
    STATIC_CHECK(interval(2, 5).intersection({1, 5}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({1, 6}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({1, 7}) == interval{2, 5});

    STATIC_CHECK(interval(2, 5).intersection({2, 3}) == interval{2, 3});
    STATIC_CHECK(interval(2, 5).intersection({2, 4}) == interval{2, 4});
    STATIC_CHECK(interval(2, 5).intersection({2, 5}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({2, 6}) == interval{2, 5});
    STATIC_CHECK(interval(2, 5).intersection({2, 7}) == interval{2, 5});

    STATIC_CHECK(interval(2, 5).intersection({3, 4}) == interval{3, 4});
    STATIC_CHECK(interval(2, 5).intersection({3, 5}) == interval{3, 5});
    STATIC_CHECK(interval(2, 5).intersection({3, 6}) == interval{3, 5});
    STATIC_CHECK(interval(2, 5).intersection({3, 7}) == interval{3, 5});

    STATIC_CHECK(interval(2, 5).intersection({4, 5}) == interval{4, 5});
    STATIC_CHECK(interval(2, 5).intersection({4, 6}) == interval{4, 5});
    STATIC_CHECK(interval(2, 5).intersection({4, 7}) == interval{4, 5});

    STATIC_CHECK(interval(2, 5).intersection({5, 6}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({5, 7}) == interval{0, 0});

    STATIC_CHECK(interval(2, 5).intersection({6, 7}) == interval{0, 0});

    // Empty
    STATIC_CHECK(interval(2, 5).intersection({0, 0}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({1, 1}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({2, 2}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({3, 3}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({4, 4}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({5, 5}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({6, 6}) == interval{0, 0});
    STATIC_CHECK(interval(2, 5).intersection({7, 7}) == interval{0, 0});

    // -----------------------------------------------------

    STATIC_CHECK(interval(-5, -2).intersection({-7, -6}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-7, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-7, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, -2).intersection({-7, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, -2).intersection({-7, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-7, -1}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-7, 0}) == interval{-5, -2});

    STATIC_CHECK(interval(-5, -2).intersection({-6, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-6, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, -2).intersection({-6, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, -2).intersection({-6, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-6, -1}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-6, 0}) == interval{-5, -2});

    STATIC_CHECK(interval(-5, -2).intersection({-5, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, -2).intersection({-5, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, -2).intersection({-5, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-5, -1}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-5, 0}) == interval{-5, -2});

    STATIC_CHECK(interval(-5, -2).intersection({-4, -3}) == interval{-4, -3});
    STATIC_CHECK(interval(-5, -2).intersection({-4, -2}) == interval{-4, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-4, -1}) == interval{-4, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-4, 0}) == interval{-4, -2});

    STATIC_CHECK(interval(-5, -2).intersection({-3, -2}) == interval{-3, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-3, -1}) == interval{-3, -2});
    STATIC_CHECK(interval(-5, -2).intersection({-3, 0}) == interval{-3, -2});

    STATIC_CHECK(interval(-5, -2).intersection({-2, -1}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-2, 0}) == interval{0, 0});

    STATIC_CHECK(interval(-5, -2).intersection({-1, 0}) == interval{0, 0});

    // Empty
    STATIC_CHECK(interval(-5, -2).intersection({-7, -7}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-6, -6}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-5, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-4, -4}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-3, -3}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-2, -2}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({-1, -1}) == interval{0, 0});
    STATIC_CHECK(interval(-5, -2).intersection({0, 0}) == interval{0, 0});

    // -----------------------------------------------------

    STATIC_CHECK(interval(-5, 2).intersection({-7, -6}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-7, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-7, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, 2).intersection({-7, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, 2).intersection({-7, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, 2).intersection({-7, -1}) == interval{-5, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-7, 0}) == interval{-5, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-7, 1}) == interval{-5, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-7, 2}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-7, 3}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-7, 4}) == interval{-5, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-6, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-6, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, 2).intersection({-6, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, 2).intersection({-6, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, 2).intersection({-6, -1}) == interval{-5, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-6, 0}) == interval{-5, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-6, 1}) == interval{-5, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-6, 2}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-6, 3}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-6, 4}) == interval{-5, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-5, -4}) == interval{-5, -4});
    STATIC_CHECK(interval(-5, 2).intersection({-5, -3}) == interval{-5, -3});
    STATIC_CHECK(interval(-5, 2).intersection({-5, -2}) == interval{-5, -2});
    STATIC_CHECK(interval(-5, 2).intersection({-5, -1}) == interval{-5, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-5, 0}) == interval{-5, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-5, 1}) == interval{-5, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-5, 2}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-5, 3}) == interval{-5, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-5, 4}) == interval{-5, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-4, -3}) == interval{-4, -3});
    STATIC_CHECK(interval(-5, 2).intersection({-4, -2}) == interval{-4, -2});
    STATIC_CHECK(interval(-5, 2).intersection({-4, -1}) == interval{-4, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-4, 0}) == interval{-4, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-4, 1}) == interval{-4, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-4, 2}) == interval{-4, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-4, 3}) == interval{-4, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-4, 4}) == interval{-4, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-3, -2}) == interval{-3, -2});
    STATIC_CHECK(interval(-5, 2).intersection({-3, -1}) == interval{-3, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-3, 0}) == interval{-3, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-3, 1}) == interval{-3, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-3, 2}) == interval{-3, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-3, 3}) == interval{-3, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-3, 4}) == interval{-3, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-2, -1}) == interval{-2, -1});
    STATIC_CHECK(interval(-5, 2).intersection({-2, 0}) == interval{-2, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-2, 1}) == interval{-2, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-2, 2}) == interval{-2, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-2, 3}) == interval{-2, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-2, 4}) == interval{-2, 2});

    STATIC_CHECK(interval(-5, 2).intersection({-1, 0}) == interval{-1, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-1, 1}) == interval{-1, 1});
    STATIC_CHECK(interval(-5, 2).intersection({-1, 2}) == interval{-1, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-1, 3}) == interval{-1, 2});
    STATIC_CHECK(interval(-5, 2).intersection({-1, 4}) == interval{-1, 2});

    STATIC_CHECK(interval(-5, 2).intersection({0, 1}) == interval{0, 1});
    STATIC_CHECK(interval(-5, 2).intersection({0, 2}) == interval{0, 2});
    STATIC_CHECK(interval(-5, 2).intersection({0, 3}) == interval{0, 2});
    STATIC_CHECK(interval(-5, 2).intersection({0, 4}) == interval{0, 2});

    STATIC_CHECK(interval(-5, 2).intersection({1, 2}) == interval{1, 2});
    STATIC_CHECK(interval(-5, 2).intersection({1, 3}) == interval{1, 2});
    STATIC_CHECK(interval(-5, 2).intersection({1, 4}) == interval{1, 2});

    STATIC_CHECK(interval(-5, 2).intersection({2, 3}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({2, 4}) == interval{0, 0});

    STATIC_CHECK(interval(-5, 2).intersection({3, 4}) == interval{0, 0});

    // Empty
    STATIC_CHECK(interval(-5, 2).intersection({-7, -7}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-6, -6}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-5, -5}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-4, -4}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-3, -3}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-2, -2}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({-1, -1}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({0, 0}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({1, 1}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({2, 2}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({3, 3}) == interval{0, 0});
    STATIC_CHECK(interval(-5, 2).intersection({4, 4}) == interval{0, 0});
}

TEST_CASE("interval: subview")
{
    {
        CHECK(interval<int>{}.as_subview_of("abcdefg"sv) == ""sv);
        CHECK(interval(2, 5).as_subview_of("abcdefg"sv) == "cde"sv);
        CHECK(interval(2, 5).as_subview_of("abcdefg") == "cde"sv);   // array adapter, N-1

        std::string const str = "abcdefg";                            // lvalue: dangling guard
        CHECK(interval(2, 5).as_subview_of(str) == "cde"sv);

        std::vector const ivec{0, 1, 2, 3, 4, 5, 6, 7};
        CHECK(interval<int>{}.as_subview_of(ivec).empty());
        CHECK(std::ranges::equal(interval(2, 5).as_subview_of(ivec), std::array{2, 3, 4}));
    }

    // null character boundary
    {
        // raw char array
        CHECK(interval(0, 2).as_subview_of("abc") == "ab"sv);
        CHECK(interval(0, 3).as_subview_of("abc") == "abc"sv);
        CHECK_THROWS_AS(interval(0, 4).as_subview_of("abc"), std::out_of_range);

        // string_view
        CHECK(interval(0, 2).as_subview_of("abc"sv) == "ab"sv);
        CHECK(interval(0, 3).as_subview_of("abc"sv) == "abc"sv);
        CHECK_THROWS_AS(interval(0, 4).as_subview_of("abc"sv), std::out_of_range);
    }

    // Malformed
    {
        CHECK_THROWS_AS(interval(0, -2).as_subview_of("abcdefg"sv), std::domain_error);
        CHECK_THROWS_AS(interval(5, 4).as_subview_of("abcdefg"sv), std::domain_error);
        CHECK_THROWS_AS(interval(-4, -5).as_subview_of("abcdefg"sv), std::domain_error);
        CHECK_THROWS_AS(interval(0, 50).as_subview_of("abcdefg"sv), std::out_of_range);
        CHECK_THROWS_AS(interval(4, 50).as_subview_of("abcdefg"sv), std::out_of_range);
        CHECK_THROWS_AS(interval(50, 51).as_subview_of("abcdefg"sv), std::out_of_range);

        std::vector const ivec{0, 1, 2, 3, 4, 5, 6};
        CHECK_THROWS_AS(interval(0, -2).as_subview_of(ivec), std::domain_error);
        CHECK_THROWS_AS(interval(5, 4).as_subview_of(ivec), std::domain_error);
        CHECK_THROWS_AS(interval(-4, -5).as_subview_of(ivec), std::domain_error);
        CHECK_THROWS_AS(interval(0, 50).as_subview_of(ivec), std::out_of_range);
        CHECK_THROWS_AS(interval(4, 50).as_subview_of(ivec), std::out_of_range);
        CHECK_THROWS_AS(interval(50, 51).as_subview_of(ivec), std::out_of_range);
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

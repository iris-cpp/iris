// SPDX-License-Identifier: MIT

#include "iris_rvariant_test.hpp"

#include <iris/rvariant.hpp>
#include <iris/type_traits.hpp>

#include <concepts>
#include <string_view>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include <cstddef>

namespace unit_test {

namespace {

template<std::size_t I>
struct Index
{
    constexpr ~Index() noexcept  // NOLINT(modernize-use-equals-default)
    {
        // non-trivial
    }

    int value = I * 2;
};

template<std::size_t Size, class Seq = std::make_index_sequence<Size>>
struct many_V_impl;

template<std::size_t Size, std::size_t... Is>
struct many_V_impl<Size, std::index_sequence<Is...>>
{
    using type = iris::rvariant<Index<Is>...>;
};

template<std::size_t Size>
using many_V_t = many_V_impl<Size>::type;

} // anonymous

TEST_CASE("raw_get", "[detail]")
{
    using iris::detail::raw_get;
    using Storage = iris::detail::make_variadic_union_t<int>;

    STATIC_REQUIRE(std::is_same_v<decltype(raw_get<0>(std::declval<Storage&>())), int&>);
    STATIC_REQUIRE(std::is_same_v<decltype(raw_get<0>(std::declval<Storage const&>())), int const&>);
    STATIC_REQUIRE(std::is_same_v<decltype(raw_get<0>(std::declval<Storage&&>())), int&&>);
    STATIC_REQUIRE(std::is_same_v<decltype(raw_get<0>(std::declval<Storage const&&>())), int const&&>);

    {
        using V = many_V_t<1>;
        constexpr V v(std::in_place_index<0>);
        STATIC_CHECK(iris::get<0>(v).value == 0 * 2);
    }

    {
        using V = many_V_t<33>;
        STATIC_CHECK(iris::get< 0>(V(std::in_place_index< 0>)).value ==  0 * 2);
        STATIC_CHECK(iris::get< 1>(V(std::in_place_index< 1>)).value ==  1 * 2);
        STATIC_CHECK(iris::get< 2>(V(std::in_place_index< 2>)).value ==  2 * 2);
        STATIC_CHECK(iris::get< 3>(V(std::in_place_index< 3>)).value ==  3 * 2);
        STATIC_CHECK(iris::get< 4>(V(std::in_place_index< 4>)).value ==  4 * 2);
        STATIC_CHECK(iris::get< 5>(V(std::in_place_index< 5>)).value ==  5 * 2);
        STATIC_CHECK(iris::get< 6>(V(std::in_place_index< 6>)).value ==  6 * 2);
        STATIC_CHECK(iris::get< 7>(V(std::in_place_index< 7>)).value ==  7 * 2);
        STATIC_CHECK(iris::get< 8>(V(std::in_place_index< 8>)).value ==  8 * 2);
        STATIC_CHECK(iris::get< 9>(V(std::in_place_index< 9>)).value ==  9 * 2);

        STATIC_CHECK(iris::get<10>(V(std::in_place_index<10>)).value == 10 * 2);
        STATIC_CHECK(iris::get<11>(V(std::in_place_index<11>)).value == 11 * 2);
        STATIC_CHECK(iris::get<12>(V(std::in_place_index<12>)).value == 12 * 2);
        STATIC_CHECK(iris::get<13>(V(std::in_place_index<13>)).value == 13 * 2);
        STATIC_CHECK(iris::get<14>(V(std::in_place_index<14>)).value == 14 * 2);
        STATIC_CHECK(iris::get<15>(V(std::in_place_index<15>)).value == 15 * 2);
        STATIC_CHECK(iris::get<16>(V(std::in_place_index<16>)).value == 16 * 2);
        STATIC_CHECK(iris::get<17>(V(std::in_place_index<17>)).value == 17 * 2);
        STATIC_CHECK(iris::get<18>(V(std::in_place_index<18>)).value == 18 * 2);
        STATIC_CHECK(iris::get<19>(V(std::in_place_index<19>)).value == 19 * 2);

        STATIC_CHECK(iris::get<20>(V(std::in_place_index<20>)).value == 20 * 2);
        STATIC_CHECK(iris::get<21>(V(std::in_place_index<21>)).value == 21 * 2);
        STATIC_CHECK(iris::get<22>(V(std::in_place_index<22>)).value == 22 * 2);
        STATIC_CHECK(iris::get<23>(V(std::in_place_index<23>)).value == 23 * 2);
        STATIC_CHECK(iris::get<24>(V(std::in_place_index<24>)).value == 24 * 2);
        STATIC_CHECK(iris::get<25>(V(std::in_place_index<25>)).value == 25 * 2);
        STATIC_CHECK(iris::get<26>(V(std::in_place_index<26>)).value == 26 * 2);
        STATIC_CHECK(iris::get<27>(V(std::in_place_index<27>)).value == 27 * 2);
        STATIC_CHECK(iris::get<28>(V(std::in_place_index<28>)).value == 28 * 2);
        STATIC_CHECK(iris::get<29>(V(std::in_place_index<29>)).value == 29 * 2);

        STATIC_CHECK(iris::get<30>(V(std::in_place_index<30>)).value == 30 * 2);
        STATIC_CHECK(iris::get<31>(V(std::in_place_index<31>)).value == 31 * 2);
        STATIC_CHECK(iris::get<32>(V(std::in_place_index<32>)).value == 32 * 2);
    }
    {
        using V = many_V_t<66>;

        STATIC_CHECK(iris::get<62>(V(std::in_place_index<62>)).value == 62 * 2);
        STATIC_CHECK(iris::get<63>(V(std::in_place_index<63>)).value == 63 * 2);
        STATIC_CHECK(iris::get<64>(V(std::in_place_index<64>)).value == 64 * 2);
        STATIC_CHECK(iris::get<65>(V(std::in_place_index<65>)).value == 65 * 2);
    }
}

// Required for suppressing std::move(const&)
// NOLINTBEGIN(performance-move-const-arg)
TEST_CASE("get")
{
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(iris::get<0>(std::as_const(var)) == 42);
        REQUIRE(iris::get<0>(var) == 42);
        REQUIRE(iris::get<0>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::get<0>(std::move(var)) == 42);
        REQUIRE_THROWS(iris::get<1>(var));
    }
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(iris::get<int>(std::as_const(var)) == 42);
        REQUIRE(iris::get<int>(var) == 42);
        REQUIRE(iris::get<int>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::get<int>(std::move(var)) == 42);
        REQUIRE_THROWS(iris::get<float>(var));
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        REQUIRE_THROWS(iris::get<int>(valueless));
        REQUIRE_THROWS(iris::get<MC_Thrower>(valueless));
    }
}

TEST_CASE("get", "[wrapper]")
{
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(iris::get<0>(std::as_const(var)) == 42);
        REQUIRE(iris::get<0>(var) == 42);
        REQUIRE(iris::get<0>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::get<0>(std::move(var)) == 42);
    }
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(iris::get<int>(std::as_const(var)) == 42);
        REQUIRE(iris::get<int>(var) == 42);
        REQUIRE(iris::get<int>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::get<int>(std::move(var)) == 42);
    }
}

TEST_CASE("unsafe_get")
{
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(iris::unsafe_get<0>(std::as_const(var)) == 42);
        REQUIRE(iris::unsafe_get<0>(var) == 42);
        REQUIRE(iris::unsafe_get<0>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::unsafe_get<0>(std::move(var)) == 42);
    }
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(iris::unsafe_get<int>(std::as_const(var)) == 42);
        REQUIRE(iris::unsafe_get<int>(var) == 42);
        REQUIRE(iris::unsafe_get<int>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::unsafe_get<int>(std::move(var)) == 42);
    }
}

TEST_CASE("unsafe_get", "[wrapper]")
{
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(iris::unsafe_get<0>(std::as_const(var)) == 42);
        REQUIRE(iris::unsafe_get<0>(var) == 42);
        REQUIRE(iris::unsafe_get<0>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::unsafe_get<0>(std::move(var)) == 42);
    }
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(iris::unsafe_get<int>(std::as_const(var)) == 42);
        REQUIRE(iris::unsafe_get<int>(var) == 42);
        REQUIRE(iris::unsafe_get<int>(std::move(std::as_const(var))) == 42);
        REQUIRE(iris::unsafe_get<int>(std::move(var)) == 42);
    }
}
// NOLINTEND(performance-move-const-arg)

TEST_CASE("get_if")
{
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(*iris::get_if<0>(&var) == 42);
        REQUIRE(*iris::get_if<0>(&std::as_const(var)) == 42);
        REQUIRE(iris::get_if<1>(&var) == nullptr);
        REQUIRE(iris::get_if<1>(&std::as_const(var)) == nullptr);
    }
    {
        iris::rvariant<int, float> var = 42;
        REQUIRE(*iris::get_if<int>(&var) == 42);
        REQUIRE(*iris::get_if<int>(&std::as_const(var)) == 42);
        REQUIRE(iris::get_if<float>(&var) == nullptr);
        REQUIRE(iris::get_if<float>(&std::as_const(var)) == nullptr);
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        REQUIRE(iris::get_if<0>(&valueless) == nullptr);
        REQUIRE(iris::get_if<1>(&valueless) == nullptr);
        REQUIRE(iris::get_if<int>(&valueless) == nullptr);
        REQUIRE(iris::get_if<MC_Thrower>(&valueless) == nullptr);
    }
}

TEST_CASE("get_if", "[wrapper]")
{
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(*iris::get_if<0>(&var) == 42);
        REQUIRE(*iris::get_if<0>(&std::as_const(var)) == 42);
    }
    {
        iris::rvariant<iris::recursive_wrapper<int>, float> var = 42;
        REQUIRE(*iris::get_if<int>(&var) == 42);
        REQUIRE(*iris::get_if<int>(&std::as_const(var)) == 42);
    }
}

namespace {

template<bool IsNoexcept, class R, class F, class Visitor, class Storage>
constexpr bool is_noexcept_invocable_r_v = std::conditional_t<
    IsNoexcept,
    std::conjunction<std::is_invocable_r<R, F, Visitor, Storage>, std::is_nothrow_invocable_r<R, F, Visitor, Storage>>,
    std::conjunction<std::is_invocable_r<R, F, Visitor, Storage>, std::negation<std::is_nothrow_invocable_r<R, F, Visitor, Storage>>>
>::value;

} // anonymous

TEST_CASE("raw_visit", "[detail]")
{
    using iris::detail::raw_visit_dispatch;
    using iris::detail::raw_visit_table;
    using iris::detail::raw_visit_noexcept_all;
    using Storage = iris::detail::make_variadic_union_t<int, double>;
    using iris::detail::valueless_t;

    // NOLINTBEGIN(cppcoreguidelines-rvalue-reference-param-not-moved)

    // invoke(overload{noexcept(true), noexcept(true)}, &)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t&) noexcept { return true; },
            [](std::in_place_index_t<0>, int&) noexcept(true) { return true; },
            [](std::in_place_index_t<1>, double&) noexcept(true) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(raw_visit_noexcept_all<Visitor const&, Storage&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[0 + 1]), Visitor const&, Storage&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[1 + 1]), Visitor const&, Storage&>);
    }
    // invoke(overload{noexcept(true), noexcept(false)}, &)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t&) noexcept { return true; },
            [](std::in_place_index_t<0>, int&) noexcept(false) { return true; },
            [](std::in_place_index_t<1>, double&) noexcept(false) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(!raw_visit_noexcept_all<bool, Visitor const&, Storage&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[0 + 1]), Visitor const&, Storage&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[1 + 1]), Visitor const&, Storage&>);
    }

    // invoke(overload{noexcept(true), noexcept(true)}, const&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t const&) noexcept { return true; },
            [](std::in_place_index_t<0>, int const&) noexcept(true) { return true; },
            [](std::in_place_index_t<1>, double const&) noexcept(true) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(raw_visit_noexcept_all<Visitor const&, Storage const&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage const&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage const&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[0 + 1]), Visitor const&, Storage const&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[1 + 1]), Visitor const&, Storage const&>);
    }
    // invoke(overload{noexcept(true), noexcept(false)}, const&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t const&) noexcept { return true; },
            [](std::in_place_index_t<0>, int const&) noexcept(false) { return true; },
            [](std::in_place_index_t<1>, double const&) noexcept(false) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(!raw_visit_noexcept_all<Visitor const&, Storage const&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage const&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage const&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[0 + 1]), Visitor const&, Storage const&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[1 + 1]), Visitor const&, Storage const&>);
    }

    // invoke(overload{noexcept(true), noexcept(true)}, &&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t&&) noexcept { return true; },
            [](std::in_place_index_t<0>, int&&) noexcept(true) { return true; },
            [](std::in_place_index_t<1>, double&&) noexcept(true) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(raw_visit_noexcept_all<Visitor const&, Storage&&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage&&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[0 + 1]), Visitor const&, Storage&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[1 + 1]), Visitor const&, Storage&&>);
    }
    // invoke(overload{noexcept(true), noexcept(false)}, &&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t&&) noexcept { return true; },
            [](std::in_place_index_t<0>, int&&) noexcept(false) { return true; },
            [](std::in_place_index_t<1>, double&&) noexcept(false) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(!raw_visit_noexcept_all<Visitor const&, Storage&&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage&&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[0 + 1]), Visitor const&, Storage&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[1 + 1]), Visitor const&, Storage&&>);
    }

    // invoke(overload{noexcept(true), noexcept(true)}, const&&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t const&&) noexcept { return true; },
            [](std::in_place_index_t<0>, int const&&) noexcept(true) { return true; },
            [](std::in_place_index_t<1>, double const&&) noexcept(true) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(raw_visit_noexcept_all<Visitor const&, Storage const&&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage const&&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage const&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[0 + 1]), Visitor const&, Storage const&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<true, bool, decltype(table[1 + 1]), Visitor const&, Storage const&&>);
    }
    // invoke(overload{noexcept(true), noexcept(false)}, const&&)
    {
        [[maybe_unused]] auto const vis = iris::overloaded{
            [](std::in_place_index_t<std::variant_npos>, valueless_t const&&) noexcept { return true; },
            [](std::in_place_index_t<0>, int const&&) noexcept(false) { return true; },
            [](std::in_place_index_t<1>, double const&&) noexcept(false) { return true; },
        };
        using Visitor = decltype(vis);
        STATIC_REQUIRE(!raw_visit_noexcept_all<Visitor const&, Storage const&&>);

        constexpr auto const& table = raw_visit_table<Visitor const&, Storage const&&>::table;
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[std::variant_npos + 1]), Visitor const&, Storage const&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[0 + 1]), Visitor const&, Storage const&&>);
        STATIC_REQUIRE(is_noexcept_invocable_r_v<false, bool, decltype(table[1 + 1]), Visitor const&, Storage const&&>);
    }

    // NOLINTEND(cppcoreguidelines-rvalue-reference-param-not-moved)
}

TEST_CASE("flat_index", "[detail]")
{
    using iris::detail::flat_index;
    using std::index_sequence;
    {
        STATIC_REQUIRE(flat_index<index_sequence<1>, true>::get(0) == 0);

        STATIC_REQUIRE(flat_index<index_sequence<2>, true>::get(0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2>, true>::get(1) == 1);

        STATIC_REQUIRE(flat_index<index_sequence<1, 1>, true, true>::get(0, 0) == 0);

        STATIC_REQUIRE(flat_index<index_sequence<2, 1>, true, true>::get(0, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 1>, true, true>::get(1, 0) == 1);

        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, true, true>::get(0, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, true, true>::get(0, 1) == 1);

        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, true, true>::get(0, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, true, true>::get(0, 1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, true, true>::get(1, 0) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, true, true>::get(1, 1) == 3);

        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(0, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(0, 1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(0, 2) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(1, 0) == 3);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(1, 1) == 4);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, true, true>::get(1, 2) == 5);

        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 0, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 0, 1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 0, 2) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 0, 3) == 3);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 1, 0) == 4);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 1, 1) == 5);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 1, 2) == 6);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 1, 3) == 7);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 2, 0) == 8);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 2, 1) == 9);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 2, 2) == 10);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(0, 2, 3) == 11);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 0, 0) == 12);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 0, 1) == 13);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 0, 2) == 14);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 0, 3) == 15);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 1, 0) == 16);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 1, 1) == 17);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 1, 2) == 18);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 1, 3) == 19);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 2, 0) == 20);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 2, 1) == 21);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 2, 2) == 22);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3, 4>, true, true, true>::get(1, 2, 3) == 23);
    }
    {
        STATIC_REQUIRE(flat_index<index_sequence<1>, false>::get(-1) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<1>, false>::get( 0) == 1);

        STATIC_REQUIRE(flat_index<index_sequence<2>, false>::get(-1) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2>, false>::get( 0) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2>, false>::get( 1) == 2);

        STATIC_REQUIRE(flat_index<index_sequence<1, 1>, false, true>::get(-1, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<1, 1>, false, true>::get( 0, 0) == 1);

        STATIC_REQUIRE(flat_index<index_sequence<2, 1>, false, true>::get(-1, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 1>, false, true>::get( 0, 0) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 1>, false, true>::get( 1, 0) == 2);

        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, false, true>::get(-1, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, false, true>::get(-1, 1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, false, true>::get( 0, 0) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<1, 2>, false, true>::get( 0, 1) == 3);

        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get(-1,  0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get(-1,  1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get( 0,  0) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get( 0,  1) == 3);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get( 1,  0) == 4);
        STATIC_REQUIRE(flat_index<index_sequence<2, 2>, false, true>::get( 1,  1) == 5);

        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get(-1, 0) == 0);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get(-1, 1) == 1);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get(-1, 2) == 2);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 0, 0) == 3);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 0, 1) == 4);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 0, 2) == 5);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 1, 0) == 6);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 1, 1) == 7);
        STATIC_REQUIRE(flat_index<index_sequence<2, 3>, false, true>::get( 1, 2) == 8);

        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 0, -1) ==  0);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 0,  0) ==  1);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 0,  1) ==  2);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 1, -1) ==  3);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 1,  0) ==  4);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get(-1, 1,  1) ==  5);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 0, -1) ==  6);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 0,  0) ==  7);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 0,  1) ==  8);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 1, -1) ==  9);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 1,  0) == 10);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 0, 1,  1) == 11);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 0, -1) == 12);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 0,  0) == 13);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 0,  1) == 14);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 1, -1) == 15);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 1,  0) == 16);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 1, 1,  1) == 17);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 0, -1) == 18);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 0,  0) == 19);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 0,  1) == 20);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 1, -1) == 21);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 1,  0) == 22);
        STATIC_REQUIRE(flat_index<index_sequence<3, 2, 2>, false, true, false>::get( 2, 1,  1) == 23);
    }
}

namespace {

template<class T>
struct strong
{
    T value;
};

namespace not_a_variant_ADL {

template<class... Ts>
struct DerivedVariant : iris::rvariant<Ts...>
{
    using DerivedVariant::rvariant::rvariant;
};

template<class... Ts>
struct not_a_variant {};

template<class R, class Foo, class... Bar>
    requires (!std::disjunction_v<iris::is_ttp_specialization_of<std::remove_cvref_t<Bar>, DerivedVariant>...>)
constexpr R visit(Foo&&, Bar&&...)
{
    return R{"not_a_variant"};
}

template<class Foo, class... Bar>
    requires (!std::disjunction_v<iris::is_ttp_specialization_of<std::remove_cvref_t<Bar>, DerivedVariant>...>)
constexpr decltype(auto) visit(Foo&&, Bar&&...)
{
    return std::string_view{"not_a_variant"};
}

} // not_a_variant_ADL

namespace SFINAE_context {

using ::iris::visit;
using not_a_variant_ADL::visit;

template<class Visitor, class Variant, class Enabled = void>
struct overload_resolvable : std::false_type {};

template<class Visitor, class Variant>
struct overload_resolvable<
    Visitor,
    Variant,
    std::void_t<decltype(
        visit<std::string_view>(std::declval<Visitor>(), std::declval<Variant>()
    ))>
> : std::true_type {};

} // SFINAE_context_ns

} // anonymous

TEST_CASE("visit (Constraints)")
{
    [[maybe_unused]] constexpr auto vis = iris::overloaded{
        [](int const&) -> std::string_view { return "variant"; },
        [](float const&) -> std::string_view { return "variant"; },
    };
    using Visitor = decltype(vis);

    [[maybe_unused]] constexpr auto different_R_vis = iris::overloaded{
        [](int const&) -> std::string_view { return "variant"; },
        [](float const&) -> std::string /* different type */ { return "variant"; },
    };
    using DifferentRVisitor = decltype(different_R_vis);

    STATIC_REQUIRE(std::is_invocable_v<Visitor, int>);
    STATIC_REQUIRE(std::is_invocable_v<Visitor, float>);
    STATIC_REQUIRE(!std::is_invocable_v<Visitor, double>); // ambiguous

    // for `visit(...)`
    {
        using IntChecker = iris::detail::visit_check_impl<std::string_view, Visitor, iris::type_list<int>>;
        using DoubleChecker = iris::detail::visit_check_impl<std::string_view, Visitor, iris::type_list<double>>;

        STATIC_REQUIRE(IntChecker::accepts_all_alternatives);
        STATIC_REQUIRE(IntChecker::value);
        STATIC_REQUIRE(!DoubleChecker::accepts_all_alternatives);
        STATIC_REQUIRE(!DoubleChecker::value);

        {
            using Check = iris::detail::visit_check<std::string_view, Visitor, iris::rvariant<int, float>&&>;
            STATIC_REQUIRE(Check::accepts_all_alternatives);
            STATIC_REQUIRE(Check::same_return_type);
            STATIC_REQUIRE(Check::value);
        }
        {
            using Check = iris::detail::visit_check<std::string_view, DifferentRVisitor, iris::rvariant<int, float>&&>;
            STATIC_REQUIRE(Check::accepts_all_alternatives);
            STATIC_REQUIRE(!Check::same_return_type);
            STATIC_REQUIRE(!Check::value);
        }
        {
            using Check = iris::detail::visit_check<std::string_view, Visitor, iris::rvariant<int, double>&&>;
            STATIC_REQUIRE(!Check::accepts_all_alternatives);
            STATIC_REQUIRE(!Check::value);
        }
    }
    // for `visit<R>(...)`
    {
        using IntChecker = iris::detail::visit_R_check_impl<std::string_view, Visitor, iris::type_list<int>>;
        using DoubleChecker = iris::detail::visit_R_check_impl<std::string_view, Visitor, iris::type_list<double>>;

        STATIC_REQUIRE(IntChecker::accepts_all_alternatives);
        STATIC_REQUIRE(IntChecker::return_type_convertible_to_R);
        STATIC_REQUIRE(IntChecker::value);
        STATIC_REQUIRE(!DoubleChecker::accepts_all_alternatives);
        STATIC_REQUIRE(!DoubleChecker::value);

        {
            using Check = iris::detail::visit_R_check<std::string_view, Visitor, iris::rvariant<int, float>&&>;
            STATIC_REQUIRE(Check::accepts_all_alternatives);
            STATIC_REQUIRE(Check::return_type_convertible_to_R);
            STATIC_REQUIRE(Check::value);
        }
        {
            using Check = iris::detail::visit_R_check<std::string, DifferentRVisitor, iris::rvariant<int, float>&&>;
            STATIC_REQUIRE(Check::accepts_all_alternatives);
            STATIC_REQUIRE(!Check::return_type_convertible_to_R);
            STATIC_REQUIRE(!Check::value);
        }
        {
            using Check = iris::detail::visit_R_check<std::string_view, Visitor, iris::rvariant<int, double>&&>;
            STATIC_REQUIRE(!Check::accepts_all_alternatives);
            STATIC_REQUIRE(!Check::value);
        }
    }

    {
        // Asserts the "Constraints:" is implemented correctly
        // https://eel.is/c++draft/variant.visit#2
        STATIC_REQUIRE(requires {
            requires std::same_as<std::true_type, SFINAE_context::overload_resolvable<Visitor, not_a_variant_ADL::not_a_variant<int, float>>::type>;
        });

        using std::visit;
        STATIC_CHECK(visit<std::string_view>(vis, not_a_variant_ADL::not_a_variant<int, float>{}) == "not_a_variant");
        STATIC_CHECK(visit<std::string_view>(vis, std::variant<int, float>{}) == "variant");
    }
    {
        // Asserts the "Constraints:" is implemented correctly
        // https://eel.is/c++draft/variant.visit#2
        STATIC_REQUIRE(requires {
            requires std::same_as<std::true_type, SFINAE_context::overload_resolvable<Visitor, not_a_variant_ADL::not_a_variant<int, float>>::type>;
        });

        using ::iris::visit;
        STATIC_CHECK(visit<std::string_view>(vis, not_a_variant_ADL::not_a_variant<int, float>{}) == "not_a_variant");
        STATIC_CHECK(visit<std::string_view>(vis, iris::rvariant<int, float>{}) == "variant");
        STATIC_CHECK(visit(vis, not_a_variant_ADL::not_a_variant<int, float>{}) == "not_a_variant");
        STATIC_CHECK(visit(vis, iris::rvariant<int, float>{}) == "variant");
        STATIC_CHECK(iris::rvariant<int, float>{}.visit<std::string_view>(vis) == "variant");
        STATIC_CHECK(iris::rvariant<int, float>{}.visit(vis) == "variant");

        // Asserts as-variant is working
        STATIC_CHECK(visit<std::string_view>(vis, not_a_variant_ADL::DerivedVariant<int, float>{42}) == "variant");
        STATIC_CHECK(visit(vis, not_a_variant_ADL::DerivedVariant<int, float>{42}) == "variant");
        STATIC_CHECK(not_a_variant_ADL::DerivedVariant<int, float>{42}.visit<std::string_view>(vis) == "variant");
        STATIC_CHECK(not_a_variant_ADL::DerivedVariant<int, float>{42}.visit(vis) == "variant");
    }
    {
        // Not permitted (hard error); std::string_view -> std::string is not implicitly convertible
        //STATIC_CHECK(std::visit<std::string>(different_R_vis, std::variant<int>{}) == "variant");
        //STATIC_CHECK(iris::visit<std::string>(different_R_vis, iris::rvariant<int>{}) == "variant");
    }
    {
        // Implicit cast from different types shall be permitted as per INVOKE<R>(...)
        STATIC_CHECK(std::visit<std::string_view>(different_R_vis, std::variant<int, float>{}) == "variant");
        STATIC_CHECK(iris::visit<std::string_view>(different_R_vis, iris::rvariant<int, float>{}) == "variant");
    }
    {
        // Case for T0 leading to ill-formed invocation.
        // In such case, a naive call to `std::invoke_result_t` will result in
        // a hard error. We try to avoid that situation, correctly engaging
        // `static_assert` error instead of numerous hard errors.

        // ill-formed (not even a `static_assert` error on MSVC)
        //std::visit<std::string_view>(vis, std::variant<double>{});

        // expected static_assert error
        //iris::visit<std::string_view>(vis, iris::rvariant<double>{});
    }

    // TODO: add test for this
    //std::visit(vis, std::variant<double>{}); // no matching overload
    //iris::visit(vis, iris::rvariant<double>{}); // no matching overload
    // Make sure "no matching overload" is SFINAE-friendly.
}

TEST_CASE("visit")
{
    using SI = strong<int>;
    using SD = strong<double>;
    using SC = strong<char>;
    using SW = strong<wchar_t>;

    {
        constexpr auto vis = iris::overloaded{
            [](SI const&) { return 0; },
            [](SD const&) { return 1; },
        };
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SI{}}) == 0);
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SD{}}) == 1);

        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SI{}}) == 0);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SD{}}) == 1);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SI{}}) == 0);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SD{}}) == 1);

        STATIC_CHECK(iris::rvariant<SI, SD>{SI{}}.visit<int>(vis) == 0);
        STATIC_CHECK(iris::rvariant<SI, SD>{SD{}}.visit<int>(vis) == 1);
        STATIC_CHECK(iris::rvariant<SI, SD>{SI{}}.visit(vis) == 0);
        STATIC_CHECK(iris::rvariant<SI, SD>{SD{}}.visit(vis) == 1);
    }
    {
        constexpr auto vis = iris::overloaded{
            [](SI const&, SC const&) { return 0; },
            [](SI const&, SW const&) { return 1; },
            [](SD const&, SC const&) { return 2; },
            [](SD const&, SW const&) { return 3; },
        };
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SI{}}, std::variant<SC, SW>{SC{}}) == 0);
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SI{}}, std::variant<SC, SW>{SW{}}) == 1);
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SD{}}, std::variant<SC, SW>{SC{}}) == 2);
        STATIC_CHECK(std::visit<int>(vis, std::variant<SI, SD>{SD{}}, std::variant<SC, SW>{SW{}}) == 3);

        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SI{}}, iris::rvariant<SC, SW>{SC{}}) == 0);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SI{}}, iris::rvariant<SC, SW>{SW{}}) == 1);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SD{}}, iris::rvariant<SC, SW>{SC{}}) == 2);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<SI, SD>{SD{}}, iris::rvariant<SC, SW>{SW{}}) == 3);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SI{}}, iris::rvariant<SC, SW>{SC{}}) == 0);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SI{}}, iris::rvariant<SC, SW>{SW{}}) == 1);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SD{}}, iris::rvariant<SC, SW>{SC{}}) == 2);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<SI, SD>{SD{}}, iris::rvariant<SC, SW>{SW{}}) == 3);
    }

    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        auto const vis = [](auto&&) {};
        CHECK_THROWS(iris::visit<void>(vis, valueless));
        CHECK_THROWS(iris::visit(vis, valueless));
        CHECK_THROWS(valueless.visit<void>(vis));
        CHECK_THROWS(valueless.visit(vis));
    }
    {
        iris::rvariant<int> never_valueless;
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        auto const vis = [](auto&&, auto&&, auto&&) {};
        CHECK_THROWS(iris::visit<void>(vis, never_valueless, valueless, never_valueless));
        CHECK_THROWS(iris::visit(vis, never_valueless, valueless, never_valueless));
        CHECK_THROWS(iris::visit<void>(vis, valueless, never_valueless, valueless));
        CHECK_THROWS(iris::visit(vis, valueless, never_valueless, valueless));
    }
}

// Equivalent to the "visit" test case, except that `SI` is wrapped
TEST_CASE("visit", "[wrapper]")
{
    using SI = strong<int>;
    using SD = strong<double>;
    using SC = strong<char>;
    using SW = strong<wchar_t>;

    {
        constexpr auto vis = iris::overloaded{
            [](SI /* unwrapped */ const&) { return 0; },
            [](SD const&) { return 1; },
        };
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}) == 0);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}) == 1);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}) == 0);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}) == 1);

        STATIC_CHECK(iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}.visit<int>(vis) == 0);
        STATIC_CHECK(iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}.visit<int>(vis) == 1);
        STATIC_CHECK(iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}.visit(vis) == 0);
        STATIC_CHECK(iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}.visit(vis) == 1);
    }
    {
        constexpr auto vis = iris::overloaded{
            [](SI /* unwrapped */ const&, SC const&) { return 0; },
            [](SI /* unwrapped */ const&, SW const&) { return 1; },
            [](SD const&, SC const&) { return 2; },
            [](SD const&, SW const&) { return 3; },
        };
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}, iris::rvariant<SC, SW>{SC{}}) == 0);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}, iris::rvariant<SC, SW>{SW{}}) == 1);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}, iris::rvariant<SC, SW>{SC{}}) == 2);
        STATIC_CHECK(iris::visit<int>(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}, iris::rvariant<SC, SW>{SW{}}) == 3);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}, iris::rvariant<SC, SW>{SC{}}) == 0);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SI{}}, iris::rvariant<SC, SW>{SW{}}) == 1);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}, iris::rvariant<SC, SW>{SC{}}) == 2);
        STATIC_CHECK(iris::visit(vis, iris::rvariant<iris::recursive_wrapper<SI>, SD>{SD{}}, iris::rvariant<SC, SW>{SW{}}) == 3);
    }
}

} // unit_test

// SPDX-License-Identifier: MIT

#if IRIS_CI
#include "iris_rvariant_test.hpp"


namespace unit_test {

namespace {

template<std::size_t I>
struct Index
{
    ~Index() noexcept  // NOLINT(modernize-use-equals-default)
    {
        // non-trivial
    }
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


TEST_CASE("many alternatives (32)")
{
    // 31
    {
        using V = many_V_t<31>;

        V a{std::in_place_type<Index<29>>};
        CHECK(a.index() == 29);
        CHECK(iris::holds_alternative<Index<29>>(a));
        CHECK_NOTHROW((void)iris::get<Index<29>>(a));
        CHECK(a.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 29);

        V b{std::in_place_type<Index<30>>};
        CHECK(b.index() == 30);
        CHECK(iris::holds_alternative<Index<30>>(b));
        CHECK_NOTHROW((void)iris::get<Index<30>>(b));
        CHECK(b.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 30);

        IRIS_REQUIRE_STATIC_NOTHROW(a.swap(b));
        CHECK(iris::holds_alternative<Index<30>>(a));
        CHECK(iris::holds_alternative<Index<29>>(b));
    }
    // 32
    {
        using V = many_V_t<32>;

        V a{std::in_place_type<Index<30>>};
        CHECK(a.index() == 30);
        CHECK(iris::holds_alternative<Index<30>>(a));
        CHECK_NOTHROW((void)iris::get<Index<30>>(a));
        CHECK(a.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 30);

        V b{std::in_place_type<Index<31>>};
        CHECK(b.index() == 31);
        CHECK(iris::holds_alternative<Index<31>>(b));
        CHECK_NOTHROW((void)iris::get<Index<31>>(b));
        CHECK(b.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 31);

        IRIS_REQUIRE_STATIC_NOTHROW(a.swap(b));
        CHECK(iris::holds_alternative<Index<31>>(a));
        CHECK(iris::holds_alternative<Index<30>>(b));
    }
    // 33
    {
        using V = many_V_t<33>;

        V a{std::in_place_type<Index<31>>};
        CHECK(a.index() == 31);
        CHECK(iris::holds_alternative<Index<31>>(a));
        CHECK_NOTHROW((void)iris::get<Index<31>>(a));
        CHECK(a.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 31);

        V b{std::in_place_type<Index<32>>};
        CHECK(b.index() == 32);
        CHECK(iris::holds_alternative<Index<32>>(b));
        CHECK_NOTHROW((void)iris::get<Index<32>>(b));
        CHECK(b.visit([]<std::size_t I>(Index<I> const&) { return I; }) == 32);

        IRIS_REQUIRE_STATIC_NOTHROW(a.swap(b));
        CHECK(iris::holds_alternative<Index<32>>(a));
        CHECK(iris::holds_alternative<Index<31>>(b));
    }
}

} // unit_test

#endif // IRIS_CI

// SPDX-License-Identifier: MIT

#include "iris_rvariant_test.hpp"

#include <iris/rvariant.hpp>

#include <type_traits>
#include <utility>
#include <variant>

namespace unit_test {

namespace {

template<class A, class B, class Expected>
constexpr bool is_same_alt = std::is_same_v<iris::compact_alternative_t<iris::rvariant, A, B>, Expected>;

} // anonymous

TEST_CASE("alternative", "[flexible]")
{
    STATIC_REQUIRE(is_same_alt<iris::rvariant<int>, iris::rvariant<int>, int>);
    STATIC_REQUIRE(is_same_alt<iris::rvariant<int>, iris::rvariant<double>, iris::rvariant<int, double>>);

    STATIC_REQUIRE(is_same_alt<iris::rvariant<int, double>, iris::rvariant<char, wchar_t>, iris::rvariant<int, double, char, wchar_t>>);
}

TEST_CASE("flexible", "[flexible]")
{
    struct X {};
    struct NonExistent {};
    // ------------------------------------------
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int>, int>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int>, NonExistent>);

    // ==========================================
    // <int, X> order for the LHS
    // ==========================================
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, int>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, X>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, NonExistent>);
    // ------------------------------------------
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X, int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, X, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X,      NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X, int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<X, int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<X, int>>);
    // ------------------------------------------

    // ==========================================
    // <X, int> order for the LHS
    // ==========================================
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, int>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, X>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, NonExistent>);
    // ------------------------------------------
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X, int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, X, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X,      NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X, int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<X, int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<X, int>>);
    // ------------------------------------------
}

TEST_CASE("flexible", "[flexible][wrapper]")
{
    struct X {};
    struct NonExistent {};
    using RW_int = iris::recursive_wrapper<int>;
    // ------------------------------------------
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int>, RW_int>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int>, RW_int>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int>, int>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int>, NonExistent>);

    // ==========================================
    // <int, X> order for the LHS
    // ==========================================
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X, int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, X, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X,      NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X, int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<X, int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<X, int>>);
    // ------------------------------------------
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X, RW_int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<NonExistent, X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<RW_int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X,         NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<RW_int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X>, iris::rvariant<X, RW_int, NonExistent>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, int, X>, iris::rvariant<X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<int, X, NonExistent>, iris::rvariant<X, RW_int>>);
    // ------------------------------------------
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X, RW_int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<NonExistent, X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<RW_int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X,         NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<RW_int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<RW_int, X>, iris::rvariant<X, RW_int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, RW_int, X>, iris::rvariant<X, RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<RW_int, X, NonExistent>, iris::rvariant<X, RW_int>>);

    // ==========================================
    // <X, int> order for the LHS
    // ==========================================
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X, int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, X, int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<int, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X, int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<X, int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<X, int>>);
    // ------------------------------------------
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X, RW_int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<NonExistent, X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<RW_int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X,         NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<RW_int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int>, iris::rvariant<X, RW_int, NonExistent>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<NonExistent, X, int>, iris::rvariant<X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, int, NonExistent>, iris::rvariant<X, RW_int>>);
    // ------------------------------------------
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X, RW_int>>);

    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, RW_int, X>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<NonExistent, X, RW_int>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<RW_int,    NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X,         NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<RW_int, X, NonExistent>>);
    STATIC_REQUIRE(!std::is_constructible_v<iris::rvariant<X, RW_int>, iris::rvariant<X, RW_int, NonExistent>>);

    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<NonExistent, X, RW_int>, iris::rvariant<X, RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<RW_int>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<RW_int, X>>);
    STATIC_REQUIRE( std::is_constructible_v<iris::rvariant<X, RW_int, NonExistent>, iris::rvariant<X, RW_int>>);
}

TEST_CASE("ambiguous flexible", "[flexible]")
{
    {
        struct A {};
        struct B {};

        {
            using V = iris::rvariant<std::monostate, A, B, iris::rvariant<A, B>>;
            // should call generic constructor
            V v{iris::rvariant<A, B>{}};
            CHECK(iris::holds_alternative<iris::rvariant<A, B>>(v) == true);
        }
        {
            using V = iris::rvariant<std::monostate, A, B, iris::rvariant<A, B>, iris::rvariant<A, B>>;
            // ill-formed; flexible constructor is not eligible, and
            // generic constructor's imaginary function has ambiguous overload
            STATIC_CHECK(!std::is_constructible_v<V, iris::rvariant<A, B>>);
        }

        {
            using V = iris::rvariant<std::monostate, A, B, iris::rvariant<A, B>>;
            // should call generic assignment operator
            V v;
            v = iris::rvariant<A, B>{};
            CHECK(iris::holds_alternative<iris::rvariant<A, B>>(v) == true);
        }
        {
            using V = iris::rvariant<std::monostate, A, B, iris::rvariant<A, B>, iris::rvariant<A, B>>;
            // ill-formed; flexible assignment operator is not eligible, and
            // generic assignment operator's imaginary function has ambiguous overload
            STATIC_CHECK(!std::is_assignable_v<V&, iris::rvariant<A, B>>);
        }
    }
}

TEST_CASE("flexible copy construction", "[flexible]")
{
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);
        iris::rvariant<int, float> b = a;
        CHECK(b.index() == 0);
        iris::rvariant<int, float, double> c = b;
        CHECK(c.index() == 0);
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);
        iris::rvariant<float, int> b = a;
        CHECK(b.index() == 1);
        iris::rvariant<double, float, int> c = b;
        CHECK(c.index() == 2);
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        iris::rvariant<int, float, MC_Thrower> a(valueless);
        CHECK(a.valueless_by_exception());
    }
}

TEST_CASE("flexible move construction", "[flexible]")
{
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);
        iris::rvariant<int, float> b = std::move(a);
        CHECK(b.index() == 0);
        iris::rvariant<int, float, double> c = std::move(b);
        CHECK(c.index() == 0);
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);
        iris::rvariant<float, int> b = std::move(a);
        CHECK(b.index() == 1);
        iris::rvariant<double, float, int> c = std::move(b);
        CHECK(c.index() == 2);
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        iris::rvariant<int, float, MC_Thrower> a(std::move(valueless));
        CHECK(a.valueless_by_exception());
    }
}

TEST_CASE("flexible copy assignment", "[flexible]")
{
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float> b = 3.14f;
        CHECK(b.index() == 1);
        CHECK_NOTHROW(b = a);
        CHECK(b.index() == 0);
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float, int> b(std::in_place_index<2>, 33 - 4);
        CHECK(b.index() == 2);
        CHECK_NOTHROW(b = a);
        CHECK(b.index() == 2);  // b's contained value is directly assigned from a's contained value, no alternative changed
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float, int> b(std::in_place_index<0>, 33 - 4);
        CHECK(b.index() == 0);
        CHECK_NOTHROW(b = a);
        CHECK(b.index() == 0);  // b's contained value is directly assigned from a's contained value, no alternative changed
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        iris::rvariant<int, float, MC_Thrower> a;
        a = valueless;
        CHECK(a.valueless_by_exception());
    }
}

TEST_CASE("flexible move assignment", "[flexible]")
{
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float> b = 3.14f;
        CHECK(b.index() == 1);
        CHECK_NOTHROW(b = std::move(a));
        CHECK(b.index() == 0);
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float, int> b(std::in_place_index<2>, 33 - 4);
        CHECK(b.index() == 2);
        CHECK_NOTHROW(b = std::move(a));
        CHECK(b.index() == 2);  // b's contained value is directly assigned from a's contained value, no alternative changed
    }
    {
        iris::rvariant<int> a = 42;
        CHECK(a.index() == 0);

        iris::rvariant<int, float, int> b(std::in_place_index<0>, 33 - 4);
        CHECK(b.index() == 0);
        CHECK_NOTHROW(b = std::move(a));
        CHECK(b.index() == 0);  // b's contained value is directly assigned from a's contained value, no alternative changed
    }
    {
        iris::rvariant<int, MC_Thrower> valueless = make_valueless<int>();
        iris::rvariant<int, float, MC_Thrower> a;
        a = std::move(valueless);
        CHECK(a.valueless_by_exception());
    }
}

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4244) // implicit numeric conversion
#endif

TEST_CASE("subset", "[flexible]")
{
    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<int, float>>(0) == 0);
    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<int, float>>(1) == 1);

    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<float, int>>(0) == 1);
    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<float, int>>(1) == 0);

    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<int, float, double>>(0) == 0);
    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<int, float, double>>(1) == 1);

    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<float, double, int>>(0) == 2);
    STATIC_REQUIRE(iris::detail::subset_reindex<iris::rvariant<int, float>, iris::rvariant<float, double, int>>(1) == 0);

    {
        iris::rvariant<int> a{42};
        CHECK_NOTHROW(iris::rvariant<int>{a.subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::as_const(a).subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::move(std::as_const(a)).subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::move(a).subset<int>()});
    }
    {
        iris::rvariant<int, float> a{42};
        CHECK_NOTHROW(iris::rvariant<int>{a.subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::as_const(a).subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::move(std::as_const(a)).subset<int>()});
        CHECK_NOTHROW(iris::rvariant<int>{std::move(a).subset<int>()});
    }
    {
        iris::rvariant<int, float> a{42};
        REQUIRE_THROWS(a.subset<float>());
    }
    {
        iris::rvariant<int, float, MC_Thrower> valueless = make_valueless<int, float>();
        iris::rvariant<int, MC_Thrower> a = valueless.subset<int, MC_Thrower>();
        CHECK(a.valueless_by_exception());
    }
}

#ifdef _MSC_VER
# pragma warning(pop)
#endif

} // unit_test

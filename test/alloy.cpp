// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/io_fwd.hpp>

#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/adapted/std_pair.hpp>

#include <iris/alloy/adapt.hpp>
#include <iris/alloy/io.hpp>
#include <iris/alloy/tuple.hpp>
#include <iris/alloy/utility.hpp>

#include <concepts>

namespace alloy = iris::alloy;

struct NonAdaptedStruct
{};

struct AdaptedStruct
{
    int x;
    double y;
};

IRIS_ALLOY_ADAPT_STRUCT(AdaptedStruct, x, y)

struct OldStyle
{
    int i;
    std::string str;
    int get_int() const { return i; }
    std::string const& get_string() const { return str; }
};

template<>
struct alloy::adaptor<OldStyle>
{
    using getters_list = iris::constant_list<&OldStyle::get_int, &OldStyle::get_string>;
};

template<std::size_t I, class T>
using alloy_get_t = decltype(alloy::get<I>(std::declval<T>()));

namespace {

void swap() = delete; // poison-pill

}

TEST_CASE("adapt_struct")
{

    {
        STATIC_CHECK(!alloy::TupleLike<NonAdaptedStruct>);
    }

    {
        STATIC_CHECK(alloy::TupleLike<AdaptedStruct>);

        STATIC_CHECK(alloy::tuple_size_v<AdaptedStruct> == 2);

        STATIC_CHECK(std::same_as<alloy_get_t<0, AdaptedStruct&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, AdaptedStruct const&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, AdaptedStruct&&>, int&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, AdaptedStruct const&&>, int const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, AdaptedStruct&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, AdaptedStruct const&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, AdaptedStruct&&>, double&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, AdaptedStruct const&&>, double const&&>);

        constexpr AdaptedStruct a{42, 3.14};

        STATIC_CHECK(alloy::get<0>(a) == 42);
        STATIC_CHECK(alloy::get<1>(a) == 3.14);
    }

    {
        STATIC_CHECK(alloy::TupleLike<OldStyle>);

        STATIC_CHECK(alloy::tuple_size_v<OldStyle> == 2);

        STATIC_CHECK(std::same_as<alloy_get_t<0, OldStyle&>, int>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, OldStyle&&>, int>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, OldStyle&>, std::string const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, OldStyle&&>, std::string const&>);
    }
}

TEST_CASE("adapt_std_pair")
{
    {
        using Pair = std::pair<int, double>;

        STATIC_CHECK(alloy::TupleLike<Pair>);

        STATIC_CHECK(alloy::tuple_size_v<Pair> == 2);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair const&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair&&>, int&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair const&&>, int const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair const&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair&&>, double&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair const&&>, double const&&>);

        constexpr Pair p(42, 3.14);

        STATIC_CHECK(alloy::get<0>(p) == 42);
        STATIC_CHECK(alloy::get<1>(p) == 3.14);
    }
    {
        using Pair = std::pair<int&, double&>;

        STATIC_CHECK(alloy::TupleLike<Pair>);

        STATIC_CHECK(alloy::tuple_size_v<Pair> == 2);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair const&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair&&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Pair const&&>, int&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair const&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair&&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Pair const&&>, double&>);

        int i = -1;
        double d = -2;
        Pair p(i, d);
        alloy::get<0>(p) = 42;
        alloy::get<1>(p) = 3.14;
        CHECK(alloy::get<0>(p) == 42);
        CHECK(alloy::get<1>(p) == 3.14);
    }
}

TEST_CASE("adapt_std_tuple")
{
    {
        using Tuple = std::tuple<int, double, char>;

        STATIC_CHECK(alloy::TupleLike<Tuple>);

        STATIC_CHECK(alloy::tuple_size_v<Tuple> == 3);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&&>, int&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&&>, int const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&&>, double&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&&>, double const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&>, char&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&>, char const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&&>, char&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&&>, char const&&>);

        constexpr Tuple p(42, 3.14, 'A');

        STATIC_CHECK(alloy::get<0>(p) == 42);
        STATIC_CHECK(alloy::get<1>(p) == 3.14);
        STATIC_CHECK(alloy::get<2>(p) == 'A');
    }
}

TEST_CASE("tuple")
{
    {
        STATIC_CHECK(std::is_trivially_default_constructible_v<alloy::tuple<>>);

        using Tuple = alloy::tuple<int, double, char>;

        STATIC_CHECK(alloy::TupleLike<Tuple>);

        STATIC_CHECK(alloy::tuple_size_v<Tuple> == 3);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&&>, int&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&&>, int const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&&>, double&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&&>, double const&&>);

        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&>, char&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&>, char const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&&>, char&&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&&>, char const&&>);

        constexpr Tuple t(42, 3.14, 'A');

        STATIC_CHECK(alloy::get<0>(t) == 42);
        STATIC_CHECK(alloy::get<1>(t) == 3.14);
        STATIC_CHECK(alloy::get<2>(t) == 'A');
    }

    {
        using Tuple = alloy::tuple<int&, double&, char&>;

        STATIC_CHECK(alloy::TupleLike<Tuple>);
        STATIC_CHECK(alloy::TupleLikeView<Tuple>);

        STATIC_CHECK(alloy::tuple_size_v<Tuple> == 3);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&&>, int&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&&>, int&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&&>, double&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&&>, double&>);

        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&>, char&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&>, char&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&&>, char&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&&>, char&>);

        int x = 42;
        double y = 3.14;
        char z = 'A';
        Tuple const t(x, y, z);

        CHECK(alloy::get<0>(t) == 42);
        CHECK(alloy::get<1>(t) == 3.14);
        CHECK(alloy::get<2>(t) == 'A');
    }

    {
        using Tuple = alloy::tuple<int const&, double const&, char const&>;

        STATIC_CHECK(alloy::TupleLike<Tuple>);
        STATIC_CHECK(alloy::TupleLikeView<Tuple>);

        STATIC_CHECK(alloy::tuple_size_v<Tuple> == 3);

        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple&&>, int const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<0, Tuple const&&>, int const&>);

        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple&&>, double const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<1, Tuple const&&>, double const&>);

        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&>, char const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&>, char const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple&&>, char const&>);
        STATIC_CHECK(std::same_as<alloy_get_t<2, Tuple const&&>, char const&>);

        int const x = 42;
        double const y = 3.14;
        char const z = 'A';
        Tuple const t(x, y, z);

        CHECK(alloy::get<0>(t) == 42);
        CHECK(alloy::get<1>(t) == 3.14);
        CHECK(alloy::get<2>(t) == 'A');
    }

    {
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int>&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int> const&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int>&&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int> const&&>);

        STATIC_CHECK(std::is_nothrow_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int>&>);
        STATIC_CHECK(std::is_nothrow_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int> const&>);
        STATIC_CHECK(std::is_nothrow_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int>&&>);
        STATIC_CHECK(std::is_nothrow_constructible_v<alloy::tuple<int, float>, alloy::tuple<float, int> const&&>);

        STATIC_CHECK(std::is_convertible_v<alloy::tuple<float, int>&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_convertible_v<alloy::tuple<float, int> const&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_convertible_v<alloy::tuple<float, int>&&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_convertible_v<alloy::tuple<float, int> const&&, alloy::tuple<int, float>>);

        STATIC_CHECK(std::is_nothrow_convertible_v<alloy::tuple<float, int>&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_nothrow_convertible_v<alloy::tuple<float, int> const&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_nothrow_convertible_v<alloy::tuple<float, int>&&, alloy::tuple<int, float>>);
        STATIC_CHECK(std::is_nothrow_convertible_v<alloy::tuple<float, int> const&&, alloy::tuple<int, float>>);

        struct NeedExplicitConversion
        {
            explicit NeedExplicitConversion(int) {}
        };

        STATIC_CHECK(std::is_constructible_v<alloy::tuple<NeedExplicitConversion>, alloy::tuple<int>&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<NeedExplicitConversion>, alloy::tuple<int> const&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<NeedExplicitConversion>, alloy::tuple<int>&&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<NeedExplicitConversion>, alloy::tuple<int> const&&>);

        STATIC_CHECK(!std::is_convertible_v<alloy::tuple<int>&, alloy::tuple<NeedExplicitConversion>>);
        STATIC_CHECK(!std::is_convertible_v<alloy::tuple<int> const&, alloy::tuple<NeedExplicitConversion>>);
        STATIC_CHECK(!std::is_convertible_v<alloy::tuple<int>&&, alloy::tuple<NeedExplicitConversion>>);
        STATIC_CHECK(!std::is_convertible_v<alloy::tuple<int> const&&, alloy::tuple<NeedExplicitConversion>>);

        struct PotentiallyThrowing
        {
            PotentiallyThrowing(int) noexcept(false) {}
        };

        STATIC_CHECK(std::is_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int>&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int> const&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int>&&>);
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int> const&&>);

        STATIC_CHECK(!std::is_nothrow_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int>&>);
        STATIC_CHECK(!std::is_nothrow_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int> const&>);
        STATIC_CHECK(!std::is_nothrow_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int>&&>);
        STATIC_CHECK(!std::is_nothrow_constructible_v<alloy::tuple<PotentiallyThrowing>, alloy::tuple<int> const&&>);

        alloy::tuple<int> a(42);
        alloy::tuple<long long> b(a);
        CHECK(alloy::get<0>(b) == 42L);
    }

    {
        STATIC_CHECK(std::is_constructible_v<alloy::tuple<int, double>, AdaptedStruct>);

        constexpr AdaptedStruct a{42, 3.14};
        constexpr alloy::tuple<int, double> t(a);
        STATIC_CHECK(alloy::get<0>(t) == 42);
        STATIC_CHECK(alloy::get<1>(t) == 3.14);
    }

    {
        STATIC_CHECK(std::is_nothrow_copy_assignable_v<alloy::tuple<int>>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<alloy::tuple<int>>);

        alloy::tuple<int> a(33), b(4);
        a = b;
        a = std::move(b);
        CHECK(alloy::get<0>(a) == 4);
    }

    {
        STATIC_CHECK(std::is_nothrow_copy_assignable_v<alloy::tuple<int&>>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<alloy::tuple<int&>>);

        int x = 33, y = 4;
        alloy::tuple<int&> a(x);
        alloy::tuple<int&> b(y);
        a = b;
        a = std::move(b);
        CHECK(alloy::get<0>(a) == 4);
    }

    {
        STATIC_CHECK(std::is_assignable_v<alloy::tuple<int>&, alloy::tuple<double> const&>);
        STATIC_CHECK(std::is_assignable_v<alloy::tuple<int>&, alloy::tuple<double>&&>);
        STATIC_CHECK(std::is_nothrow_assignable_v<alloy::tuple<int>&, alloy::tuple<double> const&>);
        STATIC_CHECK(std::is_nothrow_assignable_v<alloy::tuple<int>&, alloy::tuple<double>&&>);

        alloy::tuple<long> a(33L);
        alloy::tuple<int> b(4);
        a = b;
        a = std::move(b);
        CHECK(alloy::get<0>(a) == 4L);
    }

    {
        STATIC_CHECK(std::is_assignable_v<alloy::tuple<int, double>&, AdaptedStruct const&>);
        STATIC_CHECK(std::is_assignable_v<alloy::tuple<int, double>&, AdaptedStruct&&>);

        alloy::tuple<int, double> a(33, 3.14);
        AdaptedStruct b{4, 2.18};
        a = b;
        a = std::move(b);
        CHECK(alloy::get<0>(a) == 4);
        CHECK(alloy::get<1>(a) == 2.18);
    }

    {
        alloy::tuple<int> a(33), b(4);
        a.swap(b);
        swap(a, b);
        CHECK(alloy::get<0>(a) == 33);
        CHECK(alloy::get<0>(b) == 4);
    }

    {
        alloy::tuple<int, double> a(42, 3.14), b = a;
        CHECK(a == b);
    }

    {
        struct Empty
        {};
        struct OnlyChar
        {
            char c;
        };
        [[maybe_unused]] constexpr alloy::tuple<Empty, OnlyChar> a = {{}, {'A'}};
        [[maybe_unused]] constexpr alloy::tuple<OnlyChar, Empty> b = {{'A'}, {}};
        STATIC_CHECK(sizeof(a) == sizeof(OnlyChar));
        STATIC_CHECK(sizeof(b) == sizeof(OnlyChar));
    }

    STATIC_CHECK(std::same_as<std::common_reference_t<alloy::tuple<int&, int&>, alloy::tuple<int, int>&>, alloy::tuple<int&, int&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<AdaptedStruct&, alloy::tuple<int, double>&>, alloy::tuple<int&, double&>>);

#if __cpp_lib_reference_from_temporary >= 202202L
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, double>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, double&>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, double&&>);

    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double>&>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double&>&>);

    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double> const&>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double&> const&>);

    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double>&&>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double&>&&>);

    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double> const&&>);
    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<int const&>, alloy::tuple<double&> const&&>);

    STATIC_CHECK(!std::is_constructible_v<alloy::tuple<double const&, int const&>, AdaptedStruct>);
#endif
}

TEST_CASE("utility")
{
    {
        constexpr alloy::tuple<int> a(42);
        constexpr alloy::tuple<double> b(3.14);
        constexpr auto c = alloy::tuple_cat(a, b);
        STATIC_CHECK(alloy::get<0>(c) == 42);
        STATIC_CHECK(alloy::get<1>(c) == 3.14);
    }

    {
        constexpr alloy::tuple<int> a(12);
        constexpr AdaptedStruct b{34, 3.14};
        constexpr auto c = alloy::tuple_cat(a, b);
        STATIC_CHECK(alloy::get<0>(c) == 12);
        STATIC_CHECK(alloy::get<1>(c) == 34);
        STATIC_CHECK(alloy::get<2>(c) == 3.14);
    }

    {
        constexpr AdaptedStruct a{12, 3.14};
        constexpr alloy::tuple<int> b(34);
        constexpr auto c = alloy::tuple_cat(a, b);
        STATIC_CHECK(alloy::get<0>(c) == 12);
        STATIC_CHECK(alloy::get<1>(c) == 3.14);
        STATIC_CHECK(alloy::get<2>(c) == 34);
    }

    {
        constexpr AdaptedStruct a{12, 3.14};
        constexpr AdaptedStruct b{34, 2.18};
        constexpr auto c = alloy::tuple_cat(a, b);
        STATIC_CHECK(alloy::get<0>(c) == 12);
        STATIC_CHECK(alloy::get<1>(c) == 3.14);
        STATIC_CHECK(alloy::get<2>(c) == 34);
        STATIC_CHECK(alloy::get<3>(c) == 2.18);
    }

    {
        OldStyle const a{12, "foo"};
        OldStyle const b{34, "bar"};
        auto const c = alloy::tuple_cat(a, b);
        CHECK(alloy::get<0>(c) == 12);
        CHECK(alloy::get<1>(c) == "foo");
        CHECK(alloy::get<2>(c) == 34);
        CHECK(alloy::get<3>(c) == "bar");
    }

    {
        constexpr alloy::tuple<int, float, double> a(42, 3.14f, 2.18);
        constexpr auto b = alloy::tuple_split<1, 2>(a);
        STATIC_CHECK(alloy::get<0>(alloy::get<0>(b)) == 42);
        STATIC_CHECK(alloy::get<0>(alloy::get<1>(b)) == 3.14f);
        STATIC_CHECK(alloy::get<1>(alloy::get<1>(b)) == 2.18);
    }

    {
        alloy::tuple<int, double> const from(33, 3.14);
        alloy::tuple<int, double> to(4, 2.18);
        alloy::tuple_assign(from, to);
        CHECK(alloy::get<0>(to) == 33);
        CHECK(alloy::get<1>(to) == 3.14);
    }

    {
        alloy::tuple<int, double> from(33, 3.14);
        alloy::tuple<int, double> to(4, 2.18);
        alloy::tuple_assign(std::move(from), to);
        CHECK(alloy::get<0>(to) == 33);
        CHECK(alloy::get<1>(to) == 3.14);
    }

    {
        alloy::tuple<int, double> tuple(42, 3.14);
        auto view = alloy::tuple_ref(tuple);
        CHECK(alloy::get<0>(view) == 42);
        CHECK(alloy::get<1>(view) == 3.14);
    }

    {
        alloy::tuple<int, double> tuple(42, 3.14);
        alloy::for_each(tuple, [](auto& elem) { elem = 33 - 4; });
        CHECK(alloy::get<0>(tuple) == 29);
        CHECK(alloy::get<1>(tuple) == 29.);
    }
}

TEST_CASE("io")
{
    {
        STATIC_CHECK(iris::req::ADL_ostreamable_v<alloy::tuple<int>>);

        struct NotStreamable {};
        STATIC_CHECK(!iris::req::ADL_ostreamable_v<NotStreamable>);
        STATIC_CHECK(!iris::req::ADL_ostreamable_v<alloy::tuple<NotStreamable>>);
    }
    {
        {
            std::stringstream ss;
            ss << alloy::tuple<>();
            CHECK(ss.str() == "()");
        }
        {
            std::stringstream ss;
            ss << alloy::tuple<int>(42);
            CHECK(ss.str() == "(42)");
        }
        {
            std::stringstream ss;
            ss << alloy::tuple<int, double>(42, 3.14);
            CHECK(ss.str() == "(42, 3.14)");
        }
    }
}

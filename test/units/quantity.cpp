// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/units/quantity.hpp>
#include <iris/requirements.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <bit>
#include <compare>
#include <concepts>
#include <limits>
#include <type_traits>

//using iris::units::quantity;
using iris::units::quantity_class;
using iris::units::unit_traits;

template<class A, class B>
concept has_plus = requires(A a, B b) { a + b; };

template<class A, class B>
concept has_minus = requires(A a, B b) { a - b; };

template<class A, class B>
concept has_multiply = requires(A a, B b) { a * b; };

template<class A, class B>
concept has_divide = requires(A a, B b) { a / b; };

template<class A, class B>
concept has_modulo = requires(A a, B b) { a % b; };

template<class A, class B>
concept has_plus_assign = requires(A& a, B b) { a += b; };

template<class A, class B>
concept has_minus_assign = requires(A& a, B b) { a -= b; };

template<class A, class B>
concept has_multiply_assign = requires(A& a, B b) { a *= b; };

template<class A, class B>
concept has_divide_assign = requires(A& a, B b) { a /= b; };

template<class A, class B>
concept has_modulo_assign = requires(A& a, B b) { a %= b; };

template<class A, class B>
concept has_equal = requires(A const& a, B const& b) { { a == b } -> iris::req::boolean_testable; };

template<class A, class B>
concept has_less = requires(A const& a, B const& b) { { a < b } -> iris::req::boolean_testable; };

template<class A>
concept has_increment = requires(A& a) { ++a; a++; --a; a--; };

template<class A, class B>
// ReSharper disable once CppUseTypeTraitAlias
concept has_common_type = requires { typename std::common_type<A, B>::type; };

template<class A, class B>
concept has_min = requires(A const& a, B const& b) { iris::units::min(a, b); };

template<class A, class B>
concept has_max = requires(A const& a, B const& b) { iris::units::max(a, b); };

template<class Q, class Lo, class Hi>
concept has_clamp = requires(Q const& q, Lo const& lo, Hi const& hi) { iris::units::clamp(q, lo, hi); };

template<class A, class B, class C, class Comp>
concept has_clamp_comp = requires(A const& a, B const& b, C const& c, Comp comp) { iris::units::clamp(a, b, c, comp); };

template<class A, class B>
concept has_midpoint = requires(A a, B b) { iris::units::midpoint(a, b); };

template<class A, class B, class T>
concept has_lerp = requires(A a, B b, T t) { iris::units::lerp(a, b, t); };

// ---------------------------------------------------

template<iris::numeric_arithmetic T>
struct my_quantity final : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};
IRIS_QUANTITY_DEDUCTION_GUIDE(my_quantity);

// ---------------------------------------------------

TEST_CASE("basic type traits", "[units]")
{
    STATIC_CHECK(!std::is_constructible_v<iris::units::quantity<int>>); // must be derived

    STATIC_CHECK(quantity_class<my_quantity<int>>);
    STATIC_CHECK(quantity_class<my_quantity<double>>);
    STATIC_CHECK(std::same_as<unit_traits<my_quantity<float>>::value_type, float>);
    STATIC_CHECK(std::same_as<unit_traits<my_quantity<float>>::rebind<double>, my_quantity<double>>);

    STATIC_CHECK(!quantity_class<int>);
    STATIC_CHECK(!quantity_class<double>);
    STATIC_CHECK(!quantity_class<my_quantity<int>*>);
    STATIC_CHECK(quantity_class<my_quantity<int> const>);
    STATIC_CHECK(quantity_class<my_quantity<int>&>);

    STATIC_CHECK(!quantity_class<std::numeric_limits<int>>);

    // -------------------------------------------------

    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<float>, my_quantity<double>>, my_quantity<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<double>, my_quantity<float>>, my_quantity<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<int>, my_quantity<unsigned>>, my_quantity<unsigned>>);
    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<int>, my_quantity<int>>, my_quantity<int>>);
    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<short>, my_quantity<short>>, my_quantity<short>>);

    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<short>, my_quantity<unsigned short>>, my_quantity<int>>);
    STATIC_CHECK(!std::is_convertible_v<my_quantity<short>, my_quantity<unsigned short>>);
    STATIC_CHECK(!std::is_convertible_v<my_quantity<unsigned short>, my_quantity<short>>);

    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<float> const, my_quantity<double>&>, my_quantity<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<int>&&, my_quantity<int> const&>, my_quantity<int>>);

    STATIC_CHECK(std::same_as<
        std::common_type_t<my_quantity<int>, my_quantity<double>>,
        decltype(my_quantity<int>{} + my_quantity<double>{})
    >);
    STATIC_CHECK(std::same_as<
        std::common_type_t<my_quantity<short>, my_quantity<short>>,
        decltype(my_quantity<short>{} + my_quantity<short>{})
    >);

    STATIC_CHECK(std::common_with<my_quantity<float>, my_quantity<double>>);
    STATIC_CHECK(std::common_with<my_quantity<int>, my_quantity<long long>>);
    STATIC_CHECK(std::common_with<my_quantity<short>, my_quantity<unsigned short>>);
    STATIC_CHECK(std::same_as<
        std::common_reference_t<my_quantity<float>&, my_quantity<double>&>,
        my_quantity<double>
    >);
    STATIC_CHECK(std::same_as<
        std::common_reference_t<my_quantity<short> const&, my_quantity<unsigned short> const&>,
        my_quantity<int>
    >);

    STATIC_CHECK(!std::common_with<my_quantity<int>, int>);
    STATIC_CHECK(!std::common_with<my_quantity<double>, double>);
}

TEMPLATE_TEST_CASE(
    "type traits", "[units][quantity]",
    int, long long, unsigned, float, double
) {
    using Q = my_quantity<TestType>;

    STATIC_CHECK(std::same_as<typename Q::value_type, TestType>);
    STATIC_CHECK(sizeof(Q) == sizeof(TestType));
    STATIC_CHECK(std::is_trivially_copyable_v<Q>);
    STATIC_CHECK(std::is_standard_layout_v<Q>);
    STATIC_CHECK(std::is_nothrow_default_constructible_v<Q>);
    STATIC_CHECK(std::is_nothrow_copy_constructible_v<Q>);
    STATIC_CHECK(std::is_nothrow_copy_assignable_v<Q>);
    STATIC_CHECK(!std::is_aggregate_v<Q>);
    STATIC_CHECK(std::regular<Q>);
    STATIC_CHECK(std::totally_ordered<Q>);
}

// ---------------------------------------------------

TEMPLATE_TEST_CASE(
    "construction (same type)", "[units][quantity]",
    int, long long, unsigned, float, double
) {
    using Q = my_quantity<TestType>;

    {
        constexpr Q q;
        STATIC_CHECK(q.value == TestType{});
    }
    {
        constexpr Q q{TestType{7}};
        STATIC_CHECK(q.value == TestType{7});
        STATIC_CHECK(!std::is_convertible_v<TestType, Q>);
        STATIC_CHECK(std::is_constructible_v<Q, TestType>);
    }
    {
        constexpr auto q = my_quantity{TestType{7}};
        STATIC_CHECK(std::same_as<decltype(q), my_quantity<TestType> const>);
    }
    {
        constexpr auto q = my_quantity{Q{TestType{7}}};
        STATIC_CHECK(std::same_as<decltype(q), Q const>);
    }
}

TEST_CASE("construction", "[units][quantity]")
{
    STATIC_CHECK(std::is_constructible_v<my_quantity<double>, float>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<double>, int>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<long long>, int>);
    STATIC_CHECK(!std::is_convertible_v<float, my_quantity<double>>);
    STATIC_CHECK(!std::is_convertible_v<double, my_quantity<double>>);

    STATIC_CHECK(std::is_constructible_v<my_quantity<float>, double>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<int>, double>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<int>, long long>);

    STATIC_CHECK(std::is_constructible_v<my_quantity<unsigned>, int>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<int>, unsigned>);

    STATIC_CHECK(std::is_constructible_v<my_quantity<int>, bool>);
    STATIC_CHECK(!std::is_constructible_v<my_quantity<int>, std::nullptr_t>);
    STATIC_CHECK(!std::is_constructible_v<my_quantity<double>, my_quantity<int>*>);

    {
        constexpr my_quantity<double> q{1.5f};
        STATIC_CHECK(q.value == 1.5);
    }
}

TEST_CASE("conversion", "[units][quantity]")
{
    STATIC_CHECK(std::is_convertible_v<my_quantity<float>, my_quantity<double>>);
    STATIC_CHECK(std::is_convertible_v<my_quantity<int>, my_quantity<double>>);
    STATIC_CHECK(std::is_convertible_v<my_quantity<int>, my_quantity<long long>>);
    STATIC_CHECK(std::is_convertible_v<my_quantity<int>, my_quantity<unsigned>>);

    {
        constexpr my_quantity<double> d = my_quantity<float>{1.5f};
        STATIC_CHECK(d.value == 1.5);
    }

    STATIC_CHECK(!std::is_convertible_v<my_quantity<double>, my_quantity<float>>);
    STATIC_CHECK(std::is_constructible_v<my_quantity<double>, my_quantity<float>>);

    STATIC_CHECK(!std::is_convertible_v<my_quantity<double>, my_quantity<int>>);

    STATIC_CHECK(std::is_constructible_v<my_quantity<int>, my_quantity<double>>);
    STATIC_CHECK(!std::is_convertible_v<my_quantity<unsigned>, my_quantity<int>>);

    {
        constexpr my_quantity<int> i{my_quantity<double>{2.75}};
        STATIC_CHECK(i.value == 2); // static_cast truncates toward zero
    }
}

// ---------------------------------------------------

TEST_CASE("comparison", "[units][quantity]")
{
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} <=> my_quantity<int>{}), std::strong_ordering>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<double>{} <=> my_quantity<double>{}), std::partial_ordering>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} <=> my_quantity<double>{}), std::partial_ordering>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} <=> my_quantity<long long>{}), std::strong_ordering>);
}

// ---------------------------------------------------

TEMPLATE_TEST_CASE(
    "unary (same type)", "[units][quantity]",
    short, int, long long, float, double
) {
    using Q = my_quantity<TestType>;

    constexpr Q q{TestType{3}};

    STATIC_CHECK(std::same_as<decltype(+q), Q>);
    STATIC_CHECK(std::same_as<decltype(-q), Q>);
    STATIC_CHECK((+q).value == TestType{3});
    STATIC_CHECK((-q).value == TestType{-3});
    STATIC_CHECK(-(-q) == q);
}

TEMPLATE_TEST_CASE(
    "increment / decrement", "[units][quantity]",
    int, long long, unsigned
) {
    using Q = my_quantity<TestType>;

    STATIC_CHECK(has_increment<Q>);

    Q q{TestType{5}};
    {
        Q& ref = ++q;
        REQUIRE(&ref == &q);
        REQUIRE(q.value == TestType{6});
        --q;
        REQUIRE(q.value == TestType{5});
    }
    {
        Q const old = q++;
        REQUIRE(old.value == TestType{5});
        REQUIRE(q.value == TestType{6});
        Q const older = q--;
        REQUIRE(older.value == TestType{6});
        REQUIRE(q.value == TestType{5});
    }
}

TEST_CASE("increment / decrement (floating point)", "[units][quantity]")
{
    STATIC_CHECK(!has_increment<my_quantity<float>>);
    STATIC_CHECK(!has_increment<my_quantity<double>>);
}

// ---------------------------------------------------

TEMPLATE_TEST_CASE(
    "addition / subtraction (same type)", "[units][quantity]",
    int, long long, unsigned, float, double
) {
    using Q = my_quantity<TestType>;

    constexpr Q a{TestType{5}};
    constexpr Q b{TestType{3}};

    STATIC_CHECK(std::same_as<decltype(a + b), Q>);
    STATIC_CHECK(std::same_as<decltype(a - b), Q>);
    STATIC_CHECK((a + b).value == TestType{8});
    STATIC_CHECK((a - b).value == TestType{2});

    {
        Q q = a;
        Q& ref = (q += b);
        REQUIRE(&ref == &q);
        REQUIRE(q.value == TestType{8});
        q -= b;
        REQUIRE(q.value == TestType{5});
    }
}

TEST_CASE("addition / subtraction", "[units][quantity]")
{
    STATIC_CHECK(std::same_as<decltype(my_quantity<float>{} + my_quantity<double>{}), my_quantity<double>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<double>{} - my_quantity<float>{}), my_quantity<double>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} + my_quantity<long long>{}), my_quantity<long long>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} + my_quantity<unsigned>{}), my_quantity<unsigned>>);

    STATIC_CHECK((my_quantity<float>{1.5f} + my_quantity<double>{2.25}).value == 3.75);
    STATIC_CHECK((my_quantity<int>{1} - my_quantity<long long>{3}).value == -2LL);

    STATIC_CHECK(std::same_as<decltype(my_quantity<short>{} + my_quantity<short>{}), my_quantity<short>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<unsigned short>{} - my_quantity<unsigned short>{}), my_quantity<unsigned short>>);
    {
        constexpr auto sum = my_quantity<short>{short{1}} + my_quantity<short>{short{2}};
        STATIC_CHECK(sum.value == short{3});
    }

    STATIC_CHECK(has_plus_assign<my_quantity<double>, my_quantity<float>>);
    STATIC_CHECK(has_plus_assign<my_quantity<long long>, my_quantity<int>>);
    STATIC_CHECK(!has_plus_assign<my_quantity<float>, my_quantity<double>>);
    STATIC_CHECK(!has_plus_assign<my_quantity<int>, my_quantity<long long>>);
    STATIC_CHECK(!has_minus_assign<my_quantity<int>, my_quantity<double>>);
    {
        my_quantity<double> d{1.0};
        d += my_quantity<float>{0.5f};
        REQUIRE(d.value == 1.5);
        d -= my_quantity<int>{1};
        REQUIRE(d.value == 0.5);
    }

    STATIC_CHECK(!has_plus<my_quantity<int>, int>);
    STATIC_CHECK(!has_plus<int, my_quantity<int>>);
    STATIC_CHECK(!has_minus<my_quantity<double>, double>);
    STATIC_CHECK(!has_plus_assign<my_quantity<double>, double>);
}

// ---------------------------------------------------

TEMPLATE_TEST_CASE(
    "multiplication / division (scalar)", "[units][quantity]",
    int, long long, unsigned, float, double
) {
    using Q = my_quantity<TestType>;

    constexpr Q q{TestType{6}};

    STATIC_CHECK(std::same_as<decltype(q * TestType{2}), Q>);
    STATIC_CHECK(std::same_as<decltype(TestType{2} * q), Q>);
    STATIC_CHECK(std::same_as<decltype(q / TestType{2}), Q>);
    STATIC_CHECK((q * TestType{2}).value == TestType{12});
    STATIC_CHECK((TestType{2} * q).value == TestType{12});
    STATIC_CHECK((q / TestType{2}).value == TestType{3});

    {
        Q r = q;
        r *= TestType{2};
        REQUIRE(r.value == TestType{12});
        r /= TestType{4};
        REQUIRE(r.value == TestType{3});
    }
}

TEST_CASE("arithmetic ops (scalar)", "[units][quantity]")
{
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} * 2.5), my_quantity<double>>);
    STATIC_CHECK(std::same_as<decltype(2.5f * my_quantity<int>{}), my_quantity<float>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<double>{} * 2), my_quantity<double>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<float>{} / 2.0), my_quantity<double>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<short>{} * short{2}), my_quantity<short>>);

    STATIC_CHECK((my_quantity<int>{3} * 2.5).value == 7.5);
    STATIC_CHECK((my_quantity<double>{7.5} / 2).value == 3.75);
    STATIC_CHECK((my_quantity<int>{7} / 2).value == 3);

    STATIC_CHECK(has_multiply_assign<my_quantity<double>, int>);
    STATIC_CHECK(has_multiply_assign<my_quantity<double>, float>);
    STATIC_CHECK(!has_multiply_assign<my_quantity<int>, double>);
    STATIC_CHECK(!has_multiply_assign<my_quantity<float>, double>);
    STATIC_CHECK(!has_divide_assign<my_quantity<int>, float>);
    {
        my_quantity<double> d{3.0};
        d *= 2;
        REQUIRE(d.value == 6.0);
        d /= 4.0f;
        REQUIRE(d.value == 1.5);
    }

    STATIC_CHECK(!has_divide<int, my_quantity<int>>);
    STATIC_CHECK(!has_divide<double, my_quantity<double>>);

    STATIC_CHECK(!has_multiply<my_quantity<int>, bool>);
    STATIC_CHECK(!has_multiply<bool, my_quantity<int>>);
    STATIC_CHECK(!has_divide<my_quantity<int>, bool>);
    STATIC_CHECK(!has_multiply_assign<my_quantity<int>, bool>);
}

// ---------------------------------------------------

TEST_CASE("quantity / quantity", "[units][quantity]")
{
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} / my_quantity<int>{}), int>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<float>{} / my_quantity<double>{}), double>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} / my_quantity<double>{}), double>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<short>{} / my_quantity<short>{}), short>);

    STATIC_CHECK(my_quantity<double>{7.5} / my_quantity<double>{2.5} == 3.0);
    STATIC_CHECK(my_quantity<int>{7} / my_quantity<int>{2} == 3);
    STATIC_CHECK(my_quantity<int>{3} / my_quantity<double>{2.0} == 1.5);

    STATIC_CHECK(!has_divide_assign<my_quantity<double>, my_quantity<double>>);
}

TEST_CASE("quantity * quantity", "[units][quantity]")
{
    STATIC_CHECK(!has_multiply<my_quantity<int>, my_quantity<int>>);
    STATIC_CHECK(!has_multiply<my_quantity<float>, my_quantity<double>>);
    STATIC_CHECK(!has_multiply_assign<my_quantity<double>, my_quantity<double>>);
}

// ---------------------------------------------------

TEMPLATE_TEST_CASE(
    "mod (integral)", "[units][quantity]",
    int, long long, unsigned
) {
    using Q = my_quantity<TestType>;

    constexpr Q q{TestType{7}};

    STATIC_CHECK(std::same_as<decltype(q % TestType{3}), Q>);
    STATIC_CHECK(std::same_as<decltype(q % Q{TestType{3}}), Q>);
    STATIC_CHECK((q % TestType{3}).value == TestType{1});
    STATIC_CHECK((q % Q{TestType{3}}).value == TestType{1});

    {
        Q r = q;
        r %= TestType{4};
        REQUIRE(r.value == TestType{3});
        r %= Q{TestType{2}};
        REQUIRE(r.value == TestType{1});
    }
}

TEST_CASE("mod", "[units][quantity]")
{
    STATIC_CHECK((my_quantity<int>{-7} % 3).value == -1);
    STATIC_CHECK((my_quantity<int>{7} % -3).value == 1);
    STATIC_CHECK((my_quantity<int>{-7} % my_quantity<int>{3}).value == -1);

    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} % 3LL), my_quantity<long long>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<int>{} % my_quantity<long long>{}), my_quantity<long long>>);
    STATIC_CHECK(std::same_as<decltype(my_quantity<short>{} % short{3}), my_quantity<short>>);
    STATIC_CHECK(has_modulo_assign<my_quantity<long long>, int>);
    STATIC_CHECK(!has_modulo_assign<my_quantity<int>, long long>);

    STATIC_CHECK(!has_modulo<my_quantity<double>, int>);
    STATIC_CHECK(!has_modulo<my_quantity<double>, my_quantity<double>>);
    STATIC_CHECK(!has_modulo<my_quantity<int>, double>);
    STATIC_CHECK(!has_modulo<my_quantity<int>, my_quantity<double>>);
    STATIC_CHECK(!has_modulo_assign<my_quantity<double>, int>);
    STATIC_CHECK(!has_modulo_assign<my_quantity<int>, double>);

    STATIC_CHECK(!has_modulo<int, my_quantity<int>>);

    STATIC_CHECK(!has_modulo<my_quantity<int>, bool>);
    STATIC_CHECK(!has_modulo_assign<my_quantity<int>, bool>);
}

// -----------------------------------------------

TEMPLATE_TEST_CASE("numeric_limits", "[units][quantity]", int, unsigned, double)
{
    using Q = my_quantity<TestType>;
    using limits = std::numeric_limits<Q>;
    using base = std::numeric_limits<TestType>;

    STATIC_CHECK(limits::is_specialized);
    STATIC_CHECK(limits::digits == base::digits);
    STATIC_CHECK(std::same_as<decltype(limits::max()), Q>);
    STATIC_CHECK(limits::max().value == base::max());
    STATIC_CHECK(limits::lowest().value == base::lowest());
    STATIC_CHECK(std::numeric_limits<Q const>::max() == limits::max());
}

// -----------------------------------------------

template<std::floating_point T>
struct RelativeLength final : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};
IRIS_QUANTITY_DEDUCTION_GUIDE(RelativeLength);

template<std::floating_point T>
struct AbsoluteLength final : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};
IRIS_QUANTITY_DEDUCTION_GUIDE(AbsoluteLength);

template<std::integral T>
struct Count final : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};
IRIS_QUANTITY_DEDUCTION_GUIDE(Count);

TEST_CASE("derived: type traits", "[units][quantity]")
{
    STATIC_CHECK(quantity_class<RelativeLength<double>>);
    STATIC_CHECK(quantity_class<Count<int>>);
    STATIC_CHECK(std::same_as<unit_traits<RelativeLength<float>>::rebind<double>, RelativeLength<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<RelativeLength<float>, RelativeLength<double>>, RelativeLength<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<RelativeLength<double>, RelativeLength<double>>, RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(std::numeric_limits<RelativeLength<double>>::max()), RelativeLength<double>>);
    STATIC_CHECK(std::numeric_limits<RelativeLength<double>>::is_specialized);
    STATIC_CHECK(std::numeric_limits<RelativeLength<double>>::max().value == std::numeric_limits<double>::max());
    STATIC_CHECK(std::regular<RelativeLength<double>>);
    STATIC_CHECK(std::totally_ordered<RelativeLength<double>>);
    STATIC_CHECK(std::equality_comparable_with<RelativeLength<float>, RelativeLength<double>>);
    STATIC_CHECK(std::totally_ordered_with<RelativeLength<float>, RelativeLength<double>>);
    STATIC_CHECK(std::common_with<RelativeLength<float>, RelativeLength<double>>);
    STATIC_CHECK(sizeof(RelativeLength<double>) == sizeof(double));
    STATIC_CHECK(std::is_trivially_copyable_v<RelativeLength<double>>);
    STATIC_CHECK(!std::is_aggregate_v<RelativeLength<double>>);
}

TEST_CASE("derived: construction / conversion", "[units][quantity]")
{
    constexpr auto c = RelativeLength{1.5};
    STATIC_CHECK(std::same_as<decltype(c), RelativeLength<double> const>);
    STATIC_CHECK(std::same_as<decltype(RelativeLength{c}), RelativeLength<double>>);

    STATIC_CHECK(std::is_convertible_v<RelativeLength<float>, RelativeLength<double>>);
    STATIC_CHECK(!std::is_convertible_v<RelativeLength<double>, RelativeLength<float>>);
    STATIC_CHECK(std::is_constructible_v<RelativeLength<float>, RelativeLength<double>>);

    // -------------------------------------------

    // other family: never
    STATIC_CHECK(!std::is_constructible_v<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!std::is_constructible_v<RelativeLength<double>, AbsoluteLength<float>>);

    // within a family: common_type direction implicit, the other explicit
    STATIC_CHECK(std::is_convertible_v<RelativeLength<float>, RelativeLength<double>>);
    STATIC_CHECK(!std::is_convertible_v<RelativeLength<double>, RelativeLength<float>>);
    STATIC_CHECK(std::is_constructible_v<RelativeLength<float>, RelativeLength<double>>);
}

TEST_CASE("derived: operators", "[units][quantity]")
{
    STATIC_CHECK(!has_plus<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!has_minus<AbsoluteLength<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_equal<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!has_less<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!has_divide<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!has_plus_assign<RelativeLength<double>, AbsoluteLength<double>>);

    STATIC_CHECK(!has_plus<RelativeLength<double>, my_quantity<double>>);
    STATIC_CHECK(!has_plus<my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_equal<RelativeLength<double>, my_quantity<double>>);
    STATIC_CHECK(!has_equal<my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_less<my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_divide<my_quantity<double>, RelativeLength<double>>);

    STATIC_CHECK(!has_multiply<RelativeLength<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_multiply<RelativeLength<double>, AbsoluteLength<double>>);
    STATIC_CHECK(!has_divide<double, RelativeLength<double>>);
    STATIC_CHECK(!has_plus<RelativeLength<double>, double>);
    STATIC_CHECK(!has_equal<RelativeLength<double>, double>);

    constexpr RelativeLength<double> a{5.0};
    constexpr RelativeLength<double> b{3.0};
    constexpr RelativeLength<float> f{0.5f};

    STATIC_CHECK(std::same_as<decltype(+a), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(-a), RelativeLength<double>>);
    STATIC_CHECK((-a).value == -5.0);

    STATIC_CHECK(std::same_as<decltype(a + b), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(a - b), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(a + f), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(f + a), RelativeLength<double>>);
    STATIC_CHECK((a + f).value == 5.5);
    STATIC_CHECK((f - a).value == -4.5);

    STATIC_CHECK(std::same_as<decltype(a * 2), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(2 * a), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(f * 2.0), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(a / 2), RelativeLength<double>>);
    STATIC_CHECK((a * 2).value == 10.0);
    STATIC_CHECK((2 * a).value == 10.0);
    STATIC_CHECK((a / 2).value == 2.5);

    STATIC_CHECK(std::same_as<decltype(a / b), double>);
    STATIC_CHECK(std::same_as<decltype(a / f), double>);
    STATIC_CHECK(a / f == 10.0);

    STATIC_CHECK(a == RelativeLength<double>{5.0});
    STATIC_CHECK(a > b);
    STATIC_CHECK(f < a);
    STATIC_CHECK(a > f);
    STATIC_CHECK(std::same_as<decltype(a <=> f), std::partial_ordering>);

    RelativeLength<double> m = a;
    STATIC_CHECK(std::same_as<decltype(m += b), RelativeLength<double>&>);
    m += f;
    REQUIRE(m.value == 5.5);
    m *= 2;
    REQUIRE(m.value == 11.0);
    m /= 4.0f;
    REQUIRE(m.value == 2.75);
    STATIC_CHECK(!has_plus_assign<RelativeLength<float>, RelativeLength<double>>);
}

TEST_CASE("derived: integral", "[units][quantity]")
{
    constexpr Count<int> n{7};
    STATIC_CHECK(has_increment<Count<int>>);
    STATIC_CHECK(!has_increment<RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(n % 3), Count<int>>);
    STATIC_CHECK(std::same_as<decltype(n % Count<long long>{2}), Count<long long>>);
    STATIC_CHECK((n % 3).value == 1);
    STATIC_CHECK(!has_modulo<int, Count<int>>);

    Count<int> m = n;
    STATIC_CHECK(std::same_as<decltype(++m), Count<int>&>);
    STATIC_CHECK(std::same_as<decltype(m++), Count<int>>);
    ++m;
    REQUIRE(m.value == 8);
    m %= 3;
    REQUIRE(m.value == 2);
}

TEST_CASE("derived: common_type(quantity & derived)", "[units][quantity]")
{
    STATIC_CHECK(!has_common_type<my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_common_type<RelativeLength<double>, my_quantity<double>>);
    STATIC_CHECK(!has_common_type<my_quantity<double> const, RelativeLength<double>&>);
    STATIC_CHECK(!std::common_with<RelativeLength<double>, my_quantity<double>>);

    STATIC_CHECK(std::same_as<std::common_type_t<my_quantity<double>, my_quantity<float>>, my_quantity<double>>);
    STATIC_CHECK(std::same_as<std::common_type_t<RelativeLength<float>, RelativeLength<double>>, RelativeLength<double>>);
    STATIC_CHECK(std::common_with<my_quantity<double>, my_quantity<float>>);
}

// -------------------------------------------------------

TEMPLATE_TEST_CASE("abs", "[units][quantity]", int, long long, float, double)
{
    using Q = my_quantity<TestType>;

    STATIC_CHECK(std::same_as<decltype(abs(Q{})), Q>);
    CHECK(abs(Q{TestType{3}}) == Q{TestType{3}});
    CHECK(abs(Q{TestType{-3}}) == Q{TestType{3}});
    CHECK(abs(Q{}) == Q{});
    CHECK(abs(std::numeric_limits<Q>::lowest() + Q{TestType{1}}) == std::numeric_limits<Q>::max());
}

TEMPLATE_TEST_CASE("abs (unsigned)", "[units][quantity]", unsigned, unsigned long long)
{
    using Q = my_quantity<TestType>;

    CHECK(abs(Q{TestType{3}}) == Q{TestType{3}});
    CHECK(abs(std::numeric_limits<Q>::max()) == std::numeric_limits<Q>::max());
}

TEMPLATE_TEST_CASE("abs (integral promotion)", "[units][quantity]", short, unsigned short)
{
    using Q = my_quantity<TestType>;

    STATIC_CHECK(std::same_as<decltype(abs(Q{})), Q>);
    STATIC_CHECK(std::same_as<decltype(abs(Q{}).value), TestType>);
    CHECK(abs(Q{TestType{3}}) == Q{TestType{3}});
    CHECK(abs(std::numeric_limits<Q>::max()) == std::numeric_limits<Q>::max());
}

TEST_CASE("abs (short)", "[units][quantity]")
{
    using Q = my_quantity<short>;

    CHECK(abs(Q{short{-3}}) == Q{short{3}});
    CHECK(abs(std::numeric_limits<Q>::lowest() + Q{short{1}}) == std::numeric_limits<Q>::max());
}

TEST_CASE("abs (floating point)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    CHECK(std::bit_cast<std::uint64_t>(abs(Q{-0.0}).value) == 0);
    CHECK(abs(std::numeric_limits<Q>::lowest()) == std::numeric_limits<Q>::max());
    CHECK(abs(-std::numeric_limits<Q>::infinity()) == std::numeric_limits<Q>::infinity());
    CHECK(std::isnan(abs(std::numeric_limits<Q>::quiet_NaN()).value));

    STATIC_CHECK(std::same_as<decltype(abs(RelativeLength<double>{})), RelativeLength<double>>);
    STATIC_CHECK(std::same_as<decltype(iris::units::abs(RelativeLength<double>{})), RelativeLength<double>>);
}

TEMPLATE_TEST_CASE("trunc (floating point)", "[units][quantity]", float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(std::same_as<decltype(trunc(Q{})), Q>);

    CHECK(trunc(Q{T{2.7}}) == Q{T{2}});
    CHECK(trunc(Q{T{2.2}}) == Q{T{2}});
    CHECK(trunc(Q{T{-2.7}}) == Q{T{-2}});
    CHECK(trunc(Q{T{-2.2}}) == Q{T{-2}});
    CHECK(trunc(Q{T{2}}) == Q{T{2}});
    CHECK(trunc(Q{}) == Q{});

    CHECK(trunc(Q{T{-2.7}}) == Q{static_cast<T>(static_cast<my_quantity<int>>(Q{T{-2.7}}).value)});

    CHECK(trunc(std::numeric_limits<Q>::infinity()) == std::numeric_limits<Q>::infinity());
    CHECK(trunc(-std::numeric_limits<Q>::infinity()) == -std::numeric_limits<Q>::infinity());
    CHECK(std::isnan(trunc(std::numeric_limits<Q>::quiet_NaN()).value));
}

TEST_CASE("trunc (negative zero)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    CHECK(std::bit_cast<std::uint64_t>(trunc(Q{-0.5}).value) == std::bit_cast<std::uint64_t>(-0.0));
    CHECK(std::bit_cast<std::uint64_t>(trunc(Q{0.5}).value) == std::bit_cast<std::uint64_t>(0.0));
}

TEMPLATE_TEST_CASE("trunc (integral)", "[units][quantity]", int, long long, unsigned, short)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(std::same_as<decltype(trunc(Q{})), Q>);
    STATIC_CHECK(trunc(Q{T{7}}) == Q{T{7}});
    STATIC_CHECK(trunc(std::numeric_limits<Q>::max()) == std::numeric_limits<Q>::max());
    STATIC_CHECK(trunc(std::numeric_limits<Q>::lowest()) == std::numeric_limits<Q>::lowest());
}

TEST_CASE("trunc (qualified)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    CHECK(iris::units::trunc(Q{1.9}) == Q{1.0});
}

TEMPLATE_TEST_CASE("floor / ceil / round (floating point)", "[units][quantity]", float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(std::same_as<decltype(floor(Q{})), Q>);
    STATIC_CHECK(std::same_as<decltype(ceil(Q{})), Q>);
    STATIC_CHECK(std::same_as<decltype(round(Q{})), Q>);

    CHECK(floor(Q{T{2.5}}) == Q{T{2}});
    CHECK(floor(Q{T{-2.5}}) == Q{T{-3}});
    CHECK(floor(Q{T{2}}) == Q{T{2}});

    CHECK(ceil(Q{T{2.5}}) == Q{T{3}});
    CHECK(ceil(Q{T{-2.5}}) == Q{T{-2}});
    CHECK(ceil(Q{T{2}}) == Q{T{2}});

    CHECK(round(Q{T{2.5}}) == Q{T{3}});
    CHECK(round(Q{T{-2.5}}) == Q{T{-3}});
    CHECK(round(Q{T{2.4}}) == Q{T{2}});
    CHECK(round(Q{T{-2.6}}) == Q{T{-3}});
    CHECK(round(Q{T{2}}) == Q{T{2}});

    CHECK(floor(Q{}) == Q{});
    CHECK(ceil(Q{}) == Q{});
    CHECK(round(Q{}) == Q{});

    CHECK(floor(std::numeric_limits<Q>::infinity()) == std::numeric_limits<Q>::infinity());
    CHECK(ceil(-std::numeric_limits<Q>::infinity()) == -std::numeric_limits<Q>::infinity());
    CHECK(std::isnan(round(std::numeric_limits<Q>::quiet_NaN()).value));
}

TEMPLATE_TEST_CASE("floor / ceil / round (integral)", "[units][quantity]", int, long long, unsigned, short)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(std::same_as<decltype(floor(Q{})), Q>);
    STATIC_CHECK(std::same_as<decltype(ceil(Q{})), Q>);
    STATIC_CHECK(std::same_as<decltype(round(Q{})), Q>);

    STATIC_CHECK(floor(Q{T{7}}) == Q{T{7}});
    STATIC_CHECK(ceil(Q{T{7}}) == Q{T{7}});
    STATIC_CHECK(round(Q{T{7}}) == Q{T{7}});
    STATIC_CHECK(floor(std::numeric_limits<Q>::max()) == std::numeric_limits<Q>::max());
    STATIC_CHECK(ceil(std::numeric_limits<Q>::lowest()) == std::numeric_limits<Q>::lowest());
}

TEST_CASE("floor / ceil / round (qualified)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    CHECK(iris::units::floor(Q{1.5}) == Q{1.0});
    CHECK(iris::units::ceil(Q{1.5}) == Q{2.0});
    CHECK(iris::units::round(Q{1.5}) == Q{2.0});
}

// -------------------------------------------------------

TEMPLATE_TEST_CASE("min / max", "[units][quantity]", int, long long, unsigned, float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    constexpr Q a{T{2}};
    constexpr Q b{T{5}};

    STATIC_CHECK(std::same_as<decltype(min(a, b)), Q>);
    STATIC_CHECK(std::same_as<decltype(max(a, b)), Q>);
    STATIC_CHECK(min(a, b) == a);
    STATIC_CHECK(min(b, a) == a);
    STATIC_CHECK(max(a, b) == b);
    STATIC_CHECK(max(b, a) == b);
    STATIC_CHECK(min(a, a) == a);
    STATIC_CHECK(max(a, a) == a);

    // std::min / std::max keep working for a single representation.
    STATIC_CHECK(std::min(a, b) == a);
    STATIC_CHECK(std::max(a, b) == b);
}

TEST_CASE("min / max (mixed representations)", "[units][quantity]")
{
    using F = my_quantity<float>;
    using D = my_quantity<double>;
    using I = my_quantity<int>;

    STATIC_CHECK(std::same_as<decltype(min(F{}, D{})), D>);
    STATIC_CHECK(std::same_as<decltype(max(I{}, F{})), F>);
    STATIC_CHECK(std::same_as<decltype(min(I{}, I{})), I>);
    STATIC_CHECK(min(I{3}, D{2.5}) == D{2.5});
    STATIC_CHECK(max(I{3}, D{2.5}) == D{3.0});
    STATIC_CHECK(min(F{1.5f}, D{2.0}) == D{1.5});
    STATIC_CHECK(max(D{2.0}, F{1.5f}) == D{2.0});
}

TEST_CASE("min / max (ties)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    // Same as std::min / std::max: the first argument is returned on a tie.
    STATIC_CHECK(std::bit_cast<std::uint64_t>(min(Q{-0.0}, Q{0.0}).value) == std::bit_cast<std::uint64_t>(-0.0));
    STATIC_CHECK(std::bit_cast<std::uint64_t>(min(Q{0.0}, Q{-0.0}).value) == std::bit_cast<std::uint64_t>(0.0));
    STATIC_CHECK(std::bit_cast<std::uint64_t>(max(Q{-0.0}, Q{0.0}).value) == std::bit_cast<std::uint64_t>(-0.0));
    STATIC_CHECK(std::bit_cast<std::uint64_t>(max(Q{0.0}, Q{-0.0}).value) == std::bit_cast<std::uint64_t>(0.0));

    // NaN: comparisons are false, so the first argument is returned.
    constexpr auto nan = std::numeric_limits<Q>::quiet_NaN();
    CHECK(std::isnan(min(nan, Q{1.0}).value));
    CHECK(min(Q{1.0}, nan) == Q{1.0});
    CHECK(std::isnan(max(nan, Q{1.0}).value));
    CHECK(max(Q{1.0}, nan) == Q{1.0});
}

TEST_CASE("min / max (comp)", "[units][quantity]")
{
    using Q = my_quantity<int>;
    using D = my_quantity<double>;

    // Qualified: a std:: comparator makes std an associated namespace, and for a
    // single representation std::min / std::max would win overload resolution.
    STATIC_CHECK(iris::units::min(Q{2}, Q{5}, std::greater<>{}) == Q{5});
    STATIC_CHECK(iris::units::max(Q{2}, Q{5}, std::greater<>{}) == Q{2});

    // The comparator receives the common representation.
    constexpr auto by_magnitude = [](D const& a, D const& b) noexcept { return abs(a) < abs(b); };
    CHECK(min(D{-7.0}, D{2.0}, by_magnitude) == D{2.0});
    CHECK(max(D{-7.0}, D{2.0}, by_magnitude) == D{-7.0});
    CHECK(max(Q{-7}, D{2.0}, by_magnitude) == D{-7.0});
}

TEST_CASE("min / max (families)", "[units][quantity]")
{
    STATIC_CHECK(has_min<my_quantity<double>, my_quantity<float>>);
    STATIC_CHECK(!has_min<my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_max<RelativeLength<double>, my_quantity<double>>);
    STATIC_CHECK(!has_max<my_quantity<double>, double>);
    STATIC_CHECK(iris::units::max(my_quantity<int>{1}, my_quantity<int>{2}) == my_quantity<int>{2});
}

// -------------------------------------------------------

TEMPLATE_TEST_CASE("clamp", "[units][quantity]", int, long long, unsigned, float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    constexpr Q lo{T{2}};
    constexpr Q hi{T{5}};

    STATIC_CHECK(std::same_as<decltype(clamp(Q{}, lo, hi)), Q>);
    STATIC_CHECK(clamp(Q{T{1}}, lo, hi) == lo);
    STATIC_CHECK(clamp(Q{T{2}}, lo, hi) == lo);
    STATIC_CHECK(clamp(Q{T{3}}, lo, hi) == Q{T{3}});
    STATIC_CHECK(clamp(Q{T{5}}, lo, hi) == hi);
    STATIC_CHECK(clamp(Q{T{7}}, lo, hi) == hi);
    STATIC_CHECK(clamp(Q{T{3}}, lo, lo) == lo);

    STATIC_CHECK(std::clamp(Q{T{7}}, lo, hi) == hi);
}

TEST_CASE("clamp (mixed)", "[units][quantity]")
{
    using F = my_quantity<float>;
    using D = my_quantity<double>;
    using I = my_quantity<int>;

    STATIC_CHECK(std::same_as<decltype(clamp(F{}, D{}, D{})), D>);
    STATIC_CHECK(std::same_as<decltype(clamp(I{}, I{}, D{})), D>);
    STATIC_CHECK(std::same_as<decltype(clamp(D{}, F{}, F{})), D>);
    STATIC_CHECK(clamp(I{1}, D{2.5}, D{4.5}) == D{2.5});
    STATIC_CHECK(clamp(F{3.0f}, D{2.5}, D{4.5}) == D{3.0});
    STATIC_CHECK(clamp(D{9.0}, F{2.5f}, I{4}) == D{4.0});
}

TEST_CASE("clamp (floating point)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    constexpr Q lo{-1.0};
    constexpr Q hi{1.0};
    CHECK(std::isnan(clamp(std::numeric_limits<Q>::quiet_NaN(), lo, hi).value));
    STATIC_CHECK(clamp(std::numeric_limits<Q>::infinity(), lo, hi) == hi);
    STATIC_CHECK(clamp(-std::numeric_limits<Q>::infinity(), lo, hi) == lo);
}

TEST_CASE("clamp (families)", "[units][quantity]")
{
    STATIC_CHECK(has_clamp<my_quantity<double>, my_quantity<float>, my_quantity<int>>);
    STATIC_CHECK(!has_clamp<my_quantity<double>, RelativeLength<double>, my_quantity<double>>);
    STATIC_CHECK(!has_clamp<my_quantity<double>, my_quantity<double>, RelativeLength<double>>);
    STATIC_CHECK(!has_clamp<my_quantity<double>, double, double>);
    STATIC_CHECK(iris::units::clamp(my_quantity<int>{9}, my_quantity<int>{0}, my_quantity<int>{5}) == my_quantity<int>{5});
}

TEMPLATE_TEST_CASE("clamp (comp)", "[units][quantity]", int, long long, float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    constexpr Q lo{T{2}};
    constexpr Q hi{T{5}};

    STATIC_CHECK(std::same_as<decltype(iris::units::clamp(Q{}, hi, lo, std::greater{})), Q>);
    STATIC_CHECK(iris::units::clamp(Q{T{1}}, hi, lo, std::greater{}) == lo);
    STATIC_CHECK(iris::units::clamp(Q{T{3}}, hi, lo, std::greater{}) == Q{T{3}});
    STATIC_CHECK(iris::units::clamp(Q{T{7}}, hi, lo, std::greater{}) == hi);

    constexpr auto by_magnitude = [](Q const& a, Q const& b) { return abs(a) < abs(b); };
    CHECK(clamp(Q{T{-7}}, Q{T{-2}}, Q{T{5}}, by_magnitude) == Q{T{5}});
    CHECK(clamp(Q{T{-1}}, Q{T{-2}}, Q{T{5}}, by_magnitude) == Q{T{-2}});
    CHECK(clamp(Q{T{3}}, Q{T{-2}}, Q{T{5}}, by_magnitude) == Q{T{3}});
}

TEST_CASE("clamp (comp, mixed)", "[units][quantity]")
{
    using F = my_quantity<float>;
    using D = my_quantity<double>;
    using I = my_quantity<int>;

    constexpr auto less_d = [](D const& a, D const& b) { return a < b; };
    STATIC_CHECK(std::same_as<decltype(clamp(I{}, F{}, D{}, less_d)), D>);
    STATIC_CHECK(clamp(I{1}, F{2.5f}, D{4.5}, less_d) == D{2.5});
    STATIC_CHECK(clamp(I{9}, F{2.5f}, D{4.5}, less_d) == D{4.5});
    STATIC_CHECK(has_clamp_comp<I, F, D, std::less<>>);
    STATIC_CHECK(!has_clamp_comp<I, F, RelativeLength<double>, std::less<>>);
    STATIC_CHECK(!has_clamp_comp<I, F, D, int>);
}

// -------------------------------------------------------

TEMPLATE_TEST_CASE("midpoint", "[units][quantity]", int, long long, unsigned, short, float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(std::same_as<decltype(midpoint(Q{}, Q{})), Q>);
    STATIC_CHECK(midpoint(Q{T{2}}, Q{T{6}}) == Q{T{4}});
    STATIC_CHECK(midpoint(Q{T{6}}, Q{T{2}}) == Q{T{4}});
    STATIC_CHECK(midpoint(Q{T{3}}, Q{T{3}}) == Q{T{3}});
    STATIC_CHECK(midpoint(Q{}, Q{}) == Q{});

    STATIC_CHECK(midpoint(std::numeric_limits<Q>::max(), std::numeric_limits<Q>::max()) == std::numeric_limits<Q>::max());
    STATIC_CHECK(midpoint(std::numeric_limits<Q>::lowest(), std::numeric_limits<Q>::lowest()) == std::numeric_limits<Q>::lowest());
}

TEMPLATE_TEST_CASE("midpoint (integral halfway)", "[units][quantity]", int, long long, short)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    STATIC_CHECK(midpoint(Q{T{2}}, Q{T{5}}) == Q{T{3}});
    STATIC_CHECK(midpoint(Q{T{5}}, Q{T{2}}) == Q{T{4}});
    STATIC_CHECK(midpoint(Q{T{-2}}, Q{T{-5}}) == Q{T{-3}});
    STATIC_CHECK(midpoint(Q{T{-5}}, Q{T{-2}}) == Q{T{-4}});
    STATIC_CHECK(midpoint(std::numeric_limits<Q>::lowest(), std::numeric_limits<Q>::max()) == Q{T{-1}});
    STATIC_CHECK(midpoint(std::numeric_limits<Q>::max(), std::numeric_limits<Q>::lowest()) == Q{});
}

TEST_CASE("midpoint (mixed)", "[units][quantity]")
{
    using F = my_quantity<float>;
    using D = my_quantity<double>;
    using I = my_quantity<int>;

    STATIC_CHECK(std::same_as<decltype(midpoint(I{}, D{})), D>);
    STATIC_CHECK(std::same_as<decltype(midpoint(F{}, I{})), F>);
    STATIC_CHECK(midpoint(I{2}, D{5.0}) == D{3.5});
    STATIC_CHECK(midpoint(F{1.0f}, D{2.0}) == D{1.5});

    STATIC_CHECK(has_midpoint<D, F>);
    STATIC_CHECK(!has_midpoint<D, RelativeLength<double>>);
    STATIC_CHECK(!has_midpoint<D, double>);
    STATIC_CHECK(iris::units::midpoint(I{1}, I{3}) == I{2});
}

TEMPLATE_TEST_CASE("lerp", "[units][quantity]", float, double)
{
    using Q = my_quantity<TestType>;
    using T = TestType;

    constexpr Q a{T{1}};
    constexpr Q b{T{5}};

    STATIC_CHECK(std::same_as<decltype(lerp(a, b, T{0.5})), Q>);
    STATIC_CHECK(lerp(a, b, T{0}) == a);
    STATIC_CHECK(lerp(a, b, T{1}) == b);
    STATIC_CHECK(lerp(a, b, T{0.5}) == Q{T{3}});
    STATIC_CHECK(lerp(a, b, T{0.25}) == Q{T{2}});
    STATIC_CHECK(lerp(b, a, T{0.25}) == Q{T{4}});

    STATIC_CHECK(lerp(a, b, T{2}) == Q{T{9}});
    STATIC_CHECK(lerp(a, b, T{-1}) == Q{T{-3}});

    STATIC_CHECK(std::same_as<decltype(lerp(a, b, 0.5)), Q>);
    STATIC_CHECK(std::same_as<decltype(lerp(a, b, 1)), Q>);
    STATIC_CHECK(lerp(a, b, 1) == b);
}

TEST_CASE("lerp (mixed)", "[units][quantity]")
{
    using F = my_quantity<float>;
    using D = my_quantity<double>;
    using I = my_quantity<int>;

    STATIC_CHECK(std::same_as<decltype(lerp(F{}, D{}, 0.5f)), D>);
    STATIC_CHECK(std::same_as<decltype(lerp(I{}, D{}, 0.5)), D>);
    STATIC_CHECK(lerp(I{0}, D{10.0}, 0.5) == D{5.0});
    STATIC_CHECK(lerp(F{0.0f}, D{10.0}, 0.25) == D{2.5});

    STATIC_CHECK(has_lerp<I, D, double>);
    STATIC_CHECK(!has_lerp<I, I, double>);
    STATIC_CHECK(!has_lerp<my_quantity<long long>, my_quantity<short>, float>);
    STATIC_CHECK(!has_lerp<D, RelativeLength<double>, double>);
    STATIC_CHECK(!has_lerp<D, D, D>);
    STATIC_CHECK(iris::units::lerp(D{0.0}, D{2.0}, 0.5) == D{1.0});
}

TEST_CASE("lerp (floating point)", "[units][quantity]")
{
    using Q = my_quantity<double>;
    CHECK(lerp(Q{1.0}, std::numeric_limits<Q>::infinity(), 0.5) == std::numeric_limits<Q>::infinity());
}

// -------------------------------------------------------

TEST_CASE("hash", "[units][quantity]")
{
    using Q = my_quantity<double>;
    CHECK(std::hash<Q>{}(Q{3.14}) == std::hash<double>{}(3.14));
}

// -------------------------------------------------------

TEMPLATE_TEST_CASE("format", "[units][quantity]", int, long long, unsigned, short, float, double)
{
    using Q = my_quantity<TestType>;

    STATIC_CHECK(std::formattable<Q, char>);
    STATIC_CHECK(std::formattable<Q, wchar_t>);

    CHECK(std::format("{}", Q{TestType{7}}) == std::format("{}", TestType{7}));
    CHECK(std::format("{:>6}", Q{TestType{7}}) == std::format("{:>6}", TestType{7}));
    CHECK(std::format("{:<6}|", Q{TestType{7}}) == std::format("{:<6}|", TestType{7}));
    CHECK(std::format("{:+}", Q{TestType{7}}) == std::format("{:+}", TestType{7}));
    CHECK(std::format(L"{}", Q{TestType{7}}) == std::format(L"{}", TestType{7}));
}

TEST_CASE("format (integral specs)", "[units][quantity]")
{
    using Q = my_quantity<int>;

    CHECK(std::format("{:04}", Q{7}) == "0007");
    CHECK(std::format("{:x}", Q{255}) == "ff");
    CHECK(std::format("{:#b}", Q{5}) == "0b101");
    CHECK(std::format("{}", Q{-3}) == "-3");
}

TEST_CASE("format (floating point specs)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    CHECK(std::format("{}", Q{1.5}) == "1.5");
    CHECK(std::format("{:.2f}", Q{1.5}) == "1.50");
    CHECK(std::format("{:e}", Q{1.5}) == "1.500000e+00");
    CHECK(std::format("{:8.3f}|", Q{1.5}) == "   1.500|");
    CHECK(std::format("{}", Q{-0.0}) == "-0");
}

TEST_CASE("format (argument forms)", "[units][quantity]")
{
    using Q = my_quantity<double>;

    Q const q{2.5};
    CHECK(std::format("{0} {0}", q) == "2.5 2.5");
    CHECK(std::format("{:.1f} and {}", q, Q{1.0}) == "2.5 and 1");
    CHECK(std::vformat("{:.1f}", std::make_format_args(q)) == "2.5");
}

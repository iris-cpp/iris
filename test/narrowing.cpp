// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/type_traits.hpp>

#include <initializer_list>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <cstddef> // IWYU pragma: keep

enum class scoped_enum {};
enum unscoped_enum {};
enum class scoped_enum_uint8 : unsigned char {};
enum unscoped_enum_short : short {};

struct base {};
struct derived : base {};

struct implicit_conversion_op
{
    operator int() const;
};

struct explicit_conversion_op
{
    explicit operator int() const;
};

struct member_ptr_test
{
};

TEST_CASE("is_convertible_without_narrowing", "[type_traits]")
{
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<double, double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, float>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, bool>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char, char>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<signed char, signed char>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned char, unsigned char>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<wchar_t, wchar_t>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char8_t, char8_t>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char16_t, char16_t>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char32_t, char32_t>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<short, short>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned short, unsigned short>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<long, long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned long, unsigned long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<long long, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned long long, unsigned long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<long double, long double>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<short, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<short, long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<short, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<signed char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned char, unsigned int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned short, unsigned int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned, unsigned long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned short, unsigned long long>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, signed char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unsigned char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, unsigned>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned int, unsigned short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned int, unsigned char>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unsigned>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long, unsigned long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long, long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, unsigned long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<short, unsigned short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned short, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<signed char, unsigned char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned char, signed char>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, long double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<double, long double>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, double>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long, float>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<float, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<float, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, short>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, unsigned>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, bool>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char16_t, char8_t>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char32_t, char16_t>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char32_t, char8_t>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, scoped_enum>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum, scoped_enum_uint8>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum_uint8, scoped_enum>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unscoped_enum>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum_short, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum_short, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unscoped_enum_short>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void*, void*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived*, base*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<base*, derived*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, void*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<base*, void*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void*, int*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, int const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int const*, int*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::nullptr_t, int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::nullptr_t, void const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, std::nullptr_t>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<std::nullptr_t, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, bool>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, int*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int member_ptr_test::*, int member_ptr_test::*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int member_ptr_test::*, int*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int member_ptr_test::*>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void (member_ptr_test::*)(), int>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int const, float>);

    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2026/p0870r8.html#ch5.9
    // The paper wants true in this case.
    // TODO: GCC/Clang and MSVC(2026) disagree; unfixable
    //STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::integral_constant<int, 42>, float>);

    struct convertible_from_int
    {
        convertible_from_int(int);
    };

    struct not_convertible_from_int {};

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<convertible_from_int, int>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, not_convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<not_convertible_from_int, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, std::string>);

    struct explicit_from_int
    {
        explicit explicit_from_int() = default;
        explicit explicit_from_int(int) {}
        explicit_from_int& operator=(int) { return *this; }
    };

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, explicit_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_from_int, int>);

    struct convertible_to_double
    {
        operator double();
    };

    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<convertible_to_double, float>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<implicit_conversion_op, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, implicit_conversion_op>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_conversion_op, int>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived, base>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<base, derived>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived&, base>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived const&, base>);

    {
        struct S
        {
            union
            {
                int x;
                float y;
            } u;
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S
        {
            int x[1];
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S
        {
            struct
            {
                int x;
            } inner;
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S
        {
            int x;
            double y;
        };
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<S, int>);
    }

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void, void>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void, void const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void const, void const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void const, void>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, void>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, base>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, int const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long const, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int volatile, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const volatile, int>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long&, int>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&&, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long&&, int>);

    STATIC_CHECK(std::is_convertible_v<int&, int&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, int&>);

    STATIC_CHECK(std::is_convertible_v<int&, int const&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, int const&>);

    STATIC_CHECK(!std::is_convertible_v<int const&, int&>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int const&, int&>);

    STATIC_CHECK(!std::is_convertible_v<int, int&>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, int&>);

    STATIC_CHECK(std::is_convertible_v<float, double const&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, double const&>);

    using F = void();

    STATIC_CHECK(std::is_convertible_v<F&, F&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<F&, F&>);

    STATIC_CHECK(std::is_convertible_v<F, F&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<F, F&>);

    using fp = void(*)();
    using fp2 = int(*)(double);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<fp, fp>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, fp2>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, fp>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, bool>);

    // Function pointer to void*: not allowed as per standard, but MSVC accepts this conversion
    // STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, void*>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int[3]>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, int[3]>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char[5], std::string>);

    using fn = void();
    using fnp = void(*)();
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<fn, fnp>);
}

// ----------------------------------------------

struct port
{
    port(int);
};

struct explicit_port
{
    explicit explicit_port(int);
};

struct to_long_long
{
    operator long long() const;
};

struct aggregate
{
    [[maybe_unused]] int i;
    [[maybe_unused]] double d;
};

struct aggregate_port
{
    [[maybe_unused]] port p;
};

struct takes_port_int
{
    takes_port_int(port, int);
};

struct value
{
    value(long long);
    value(std::string);
    value(std::initializer_list<value>);
};

struct assign_int
{
    assign_int& operator=(int);
};

TEST_CASE("is_convertible_without_any_narrowing", "[type_traits]")
{
    STATIC_CHECK(iris::is_convertible_without_any_narrowing_v<int, long long>);
    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<long long, int>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<to_long_long, int>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<long long, port>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<long long, port>);
    STATIC_CHECK(iris::is_convertible_without_any_narrowing_v<int, port>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<long long, port const&>);
    STATIC_CHECK(iris::is_convertible_without_any_narrowing_v<long long, std::optional<int>>); // narrowing in the body
    STATIC_CHECK(iris::is_convertible_without_any_narrowing_v<char const*, std::string>);

    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<long long, value>); // initializer-list constructor
    STATIC_CHECK(!iris::is_convertible_without_any_narrowing_v<std::string, value>); // initializer-list constructor
}

TEST_CASE("is_constructible_without_any_narrowing", "[type_traits]")
{
    STATIC_CHECK(iris::is_constructible_without_any_narrowing_v<std::vector<int>>);
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<int, long long>);
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<bool, std::nullptr_t>);
    STATIC_CHECK(iris::is_constructible_without_any_narrowing_v<explicit_port, int>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<explicit_port, long long>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<port const&, long long>);
    STATIC_CHECK(iris::is_constructible_without_any_narrowing_v<std::optional<int>, long long>); // narrowing in the body

    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<aggregate_port, long long>);
    STATIC_CHECK(iris::is_constructible_without_any_narrowing_v<aggregate_port, int>);
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<aggregate, long long, double>);
    STATIC_CHECK(iris::is_constructible_without_any_narrowing_v<aggregate, int, double>);
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<takes_port_int, int, int>); // limitation

    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<std::vector<int>, std::size_t>); // initializer-list constructor
    STATIC_CHECK(!iris::is_constructible_without_any_narrowing_v<value, std::string>); // initializer-list constructor
}

TEST_CASE("is_assignable_without_any_narrowing", "[type_traits]")
{
    STATIC_CHECK(!iris::is_assignable_without_any_narrowing_v<int&, long long>);
    STATIC_CHECK(iris::is_assignable_without_any_narrowing_v<long long&, int>);
    STATIC_CHECK(!iris::is_assignable_without_any_narrowing_v<assign_int&, long long>);
    STATIC_CHECK(iris::is_assignable_without_any_narrowing_v<assign_int&, int>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_assignable_without_any_narrowing_v<port&, long long>);
    STATIC_CHECK(iris::is_assignable_without_any_narrowing_v<port&, int>);

    STATIC_CHECK(!iris::is_assignable_without_any_narrowing_v<std::string&, char>); // initializer-list assignment
}

TEST_CASE("is_assignable_without_narrowing", "[type_traits]")
{
    STATIC_CHECK(iris::is_assignable_without_narrowing_v<int&, int>);
    STATIC_CHECK(iris::is_assignable_without_narrowing_v<long long&, int>);
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<int&, long long>);
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<double&, long long>);
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<bool&, int*>);
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<int&, to_long_long>); // after the conversion function
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<int const&, int>);

    // a class takes the value as the parameter of its assignment operator
    STATIC_CHECK(iris::is_assignable_without_narrowing_v<port&, long long>);
    STATIC_CHECK(iris::is_assignable_without_narrowing_v<assign_int&, long long>);
    STATIC_CHECK(!iris::is_assignable_without_narrowing_v<assign_int&, std::string>);
}

TEST_CASE("conversion_overloads", "[type_traits]")
{
    using int_or_long_long = iris::conversion_overloads<iris::conversion_overload<3, int>, iris::conversion_overload<5, long long>>;
    STATIC_CHECK(iris::conversion_selects<int_or_long_long, int, 3>);
    STATIC_CHECK(iris::conversion_selects<int_or_long_long, long long, 5>);
    STATIC_CHECK(!iris::conversion_selects<int_or_long_long, int, 5>);
    STATIC_CHECK(iris::conversion_selects<int_or_long_long, short, 3>); // a promotion
    STATIC_CHECK(!iris::conversion_resolves<int_or_long_long, unsigned>); // ambiguous: both are conversions
    STATIC_CHECK(!iris::conversion_resolves<int_or_long_long, std::string>);

    using port_only = iris::conversion_overloads<iris::conversion_overload<0, port>>;
    STATIC_CHECK(iris::conversion_selects<port_only, long long, 0>); // no narrowing check

    STATIC_CHECK(!iris::conversion_resolves<iris::conversion_overloads<>, int>);
}

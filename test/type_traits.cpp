// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/type_traits.hpp>

namespace {

template<class... Ts>
struct tuple;

template<class... Ts>
struct type_list;


template<int... Ns>
struct n_tuple;

template<int... Ns>
struct n_list;

struct convertible_from_int
{
    convertible_from_int(int);
};

struct not_convertible_from_int {};

struct explicit_from_int
{
    explicit explicit_from_int() = default;
    explicit explicit_from_int(int);
    explicit_from_int& operator=(int) { return *this; }
};

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

struct abstract_class
{
    virtual void f() = 0;
};

struct multi_arg_implicit
{
    multi_arg_implicit(int, double);
};

struct has_initializer_list_ctor
{
    has_initializer_list_ctor(std::initializer_list<int>);
};

struct member_ptr_test
{
    int member;
    void func();
};

} // anonymous


TEST_CASE("is_convertible_without_narrowing: same type identity")
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
}

TEST_CASE("is_convertible_without_narrowing: integer widening")
{
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
}

TEST_CASE("is_convertible_without_narrowing: integer narrowing")
{
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, signed char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, unsigned>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned int, unsigned short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned int, unsigned char>);
}

TEST_CASE("is_convertible_without_narrowing: signed/unsigned mismatch")
{
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
}

TEST_CASE("is_convertible_without_narrowing: floating-point widening")
{
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, long double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<double, long double>);
}

TEST_CASE("is_convertible_without_narrowing: floating-point narrowing")
{
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, double>);
}

TEST_CASE("is_convertible_without_narrowing: integer/floating-point cross")
{
    // Integer to floating-point: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, double>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long, float>);

    // Floating-point to integer: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<float, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<float, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, short>);
}

TEST_CASE("is_convertible_without_narrowing: bool")
{
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, bool>);

    // bool to integer: widening (bool is an integer type with rank < int)
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, unsigned>);

    // Integer/float to bool: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, bool>);
}

TEST_CASE("is_convertible_without_narrowing: char types")
{
    // char8_t, char16_t, char32_t are distinct types with specific widths
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char16_t, char8_t>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char32_t, char16_t>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<char32_t, char8_t>);

    // wchar_t cross: platform-dependent (wchar_t width varies)
    // On platforms where sizeof(wchar_t) == sizeof(int), these may be non-narrowing.

    // char to int: widening
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<signed char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned char, int>);

    // int to char: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, signed char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unsigned char>);
}

TEST_CASE("is_convertible_without_narrowing: enum types")
{
    // Scoped enums: not implicitly convertible to/from anything
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, scoped_enum>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum, scoped_enum_uint8>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum_uint8, scoped_enum>);

    // Unscoped enums: implicitly convertible to integer types
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unscoped_enum>);

    // Unscoped enum with fixed underlying type (short)
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum_short, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum_short, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unscoped_enum_short>);
}

TEST_CASE("is_convertible_without_narrowing: pointer types")
{
    // Same pointer type
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void*, void*>);

    // Derived-to-base pointer conversion
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived*, base*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<base*, derived*>);

    // T* to void*
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, void*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<base*, void*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void*, int*>);

    // Adding const via pointer
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int*, int const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int const*, int*>);

    // nullptr_t
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::nullptr_t, int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::nullptr_t, void const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, std::nullptr_t>);

    // pointer to bool: narrowing (all pointers/nullptr to bool is narrowing)
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<std::nullptr_t, bool>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, bool>);

    // Pointer to/from arithmetic: not convertible
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, int*>);

    // Pointer-to-member
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int member_ptr_test::*, int member_ptr_test::*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int member_ptr_test::*, int*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int member_ptr_test::*>);

    // Member function pointer
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void (member_ptr_test::*)(), int>);
}

TEST_CASE("is_convertible_without_narrowing: class types with implicit ctor")
{
    // Implicit converting constructor
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<convertible_from_int, int>);

    // Not convertible at all
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, not_convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<not_convertible_from_int, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, std::string>);
}

TEST_CASE("is_convertible_without_narrowing: class types with explicit ctor")
{
    // Explicit ctor: is_convertible is false, so trait is false
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, explicit_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_from_int, int>);
}

TEST_CASE("is_convertible_without_narrowing: class types with conversion operator")
{
    // Implicit conversion operator
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<implicit_conversion_op, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, implicit_conversion_op>);

    // Explicit conversion operator: is_convertible is false
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_conversion_op, int>);
}

TEST_CASE("is_convertible_without_narrowing: class types inheritance")
{
    // Derived to base (by value): implicitly convertible
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived, base>);
    // Base to derived: not implicitly convertible
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<base, derived>);

    // Derived& to base: implicitly convertible
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived&, base>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<const derived&, base>);
}

TEST_CASE("is_convertible_without_narrowing: aggregate types")
{
    // Aggregates with brace-init but no implicit conversion
    {
        struct S {
            union {
                int x;
                float y;
            } u;
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S {
            int x[1];
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S {
            struct {
                int x;
            } inner;
        };
        [[maybe_unused]] S s{42};
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
    }
    {
        struct S {
            int x;
            double y;
        };
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, S>);
        STATIC_CHECK(!iris::is_convertible_without_narrowing_v<S, int>);
    }
}

TEST_CASE("is_convertible_without_narrowing: void")
{
    // void to void: is_convertible_v<void, void> is true per the standard.
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void, void>);

    // void to/from anything else: not convertible
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, void>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, base>);
}

TEST_CASE("is_convertible_without_narrowing: cv-qualified types")
{
    // const arithmetic: same narrowing rules
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<const int, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, const int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<const int, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<const long long, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<volatile int, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<const volatile int, int>);
}

TEST_CASE("is_convertible_without_narrowing: reference types as From")
{
    // Lvalue reference
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long&, int>);

    // Rvalue reference
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&&, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int&&, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long&&, int>);
}

TEST_CASE("is_convertible_without_narrowing: function pointers")
{
    using fp = void(*)();
    using fp2 = int(*)(double);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<fp, fp>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, fp2>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, fp>);

    // Function pointer to bool: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, bool>);

    // Function pointer to void*: not allowe per standard, but MSVC accepts this conversion
    // STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, void*>);
}

TEST_CASE("is_convertible_without_narrowing: array and function types")
{
    // Array types as From decay to pointers
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int[3]>);

    // char[] to string
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char[5], std::string>);

    // Function type decays to function pointer
    using fn = void();
    using fnp = void(*)();
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<fn, fnp>);
}


TEST_CASE("specialization_of")
{
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<>, tuple>);
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<int>, tuple>);
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<int, double>, tuple>);

    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<>, type_list>);
    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<int>, type_list>);
    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<int, double>, type_list>);

    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<>, n_tuple>);
    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<0>, n_tuple>);
    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<0, 1>, n_tuple>);

    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<>, n_list>);
    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<0>, n_list>);
    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<0, 1>, n_list>);
}

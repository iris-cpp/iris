// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/type_traits.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <type_traits>
#include <concepts>
#include <utility>

#if __has_include(<stdfloat>)
# include <stdfloat>
#endif

#include <cstdint>

template<class... Ts>
struct tuple;

template<class... Ts>
struct type_list;

template<int... Ns>
struct n_tuple;

template<int... Ns>
struct n_list;

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

TEST_CASE("type traits")
{
    STATIC_CHECK(iris::signed_numeric_integral<std::int8_t>);
    STATIC_CHECK(iris::signed_numeric_integral<std::int16_t>);
    STATIC_CHECK(iris::signed_numeric_integral<std::int32_t>);
    STATIC_CHECK(iris::signed_numeric_integral<std::int64_t>);
    STATIC_CHECK(!iris::signed_numeric_integral<bool>);
    STATIC_CHECK(!iris::signed_numeric_integral<char>);
    STATIC_CHECK(!iris::signed_numeric_integral<wchar_t>);

    STATIC_CHECK(iris::unsigned_numeric_integral<std::uint8_t>);
    STATIC_CHECK(iris::unsigned_numeric_integral<std::uint16_t>);
    STATIC_CHECK(iris::unsigned_numeric_integral<std::uint32_t>);
    STATIC_CHECK(iris::unsigned_numeric_integral<std::uint64_t>);
    STATIC_CHECK(!iris::unsigned_numeric_integral<bool>);
    STATIC_CHECK(!iris::unsigned_numeric_integral<char>);
    STATIC_CHECK(!iris::unsigned_numeric_integral<wchar_t>);
}

// https://en.cppreference.com/cpp/types/floating-point

struct Float16 { explicit operator float() const noexcept { return 0.0f; } };
struct BFloat16 { explicit operator float() const noexcept { return 0.0f; } };

template<class T>
concept StandardFloatingPoint =
    std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, long double>;

// NOLINTBEGIN(bugprone-std-namespace-modification)
template<StandardFloatingPoint F> struct std::common_type<Float16, F> { using type = F; };
template<StandardFloatingPoint F> struct std::common_type<F, Float16> { using type = F; };
template<std::integral I> struct std::common_type<Float16, I> { using type = Float16; };
template<std::integral I> struct std::common_type<I, Float16> { using type = Float16; };

template<StandardFloatingPoint F> struct std::common_type<BFloat16, F> { using type = F; };
template<StandardFloatingPoint F> struct std::common_type<F, BFloat16> { using type = F; };
template<std::integral I> struct std::common_type<BFloat16, I> { using type = BFloat16; };
template<std::integral I> struct std::common_type<I, BFloat16> { using type = BFloat16; };
// NOLINTEND(bugprone-std-namespace-modification)

template<class... Ts>
concept has_common_type = requires { typename std::common_type_t<Ts...>; };

template<class... Ts>
concept has_dominant_type = requires { typename iris::dominant_type_t<Ts...>; };

template<class... Ts>
concept has_symmetric_common_type = requires { typename iris::symmetric_common_type_t<Ts...>; };

#if __STDCPP_FLOAT16_T__ && __STDCPP_BFLOAT16_T__
#define IRIS_TEST_NARROW_FP_PAIRS \
    (std::pair<Float16, BFloat16>), (std::pair<std::float16_t, std::bfloat16_t>)
#else
#define IRIS_TEST_NARROW_FP_PAIRS \
    (std::pair<Float16, BFloat16>)
#endif

TEST_CASE("dominant_type: basic", "[type_traits]")
{
    STATIC_CHECK(!has_dominant_type<>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<short, short>, short>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<short, int>, int>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<int, short>, int>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<int, unsigned>, unsigned>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<int, double>, double>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<short, char, int>, int>);
    STATIC_CHECK(!has_dominant_type<short, char>); // `std::common_type_t` is `int`
    STATIC_CHECK(!has_dominant_type<signed char, unsigned char>);
    STATIC_CHECK(!has_dominant_type<short, char, unsigned short>);
}

TEMPLATE_TEST_CASE("dominant_type", "[type_traits]", IRIS_TEST_NARROW_FP_PAIRS)
{
    using F16 = TestType::first_type;
    using BF16 = TestType::second_type;

    STATIC_CHECK(std::same_as<iris::dominant_type_t<short, F16>, F16>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, int>, F16>);
    STATIC_CHECK(!has_dominant_type<int, F16, BF16>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<int, F16, BF16, float>, float>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<float>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16>, F16>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<float, float>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, F16>, F16>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<float const>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<float const&, F16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16&&, float const&>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16 const&, F16>, F16>);

    STATIC_CHECK(!has_common_type<F16, BF16>);
    STATIC_CHECK(!has_dominant_type<F16, BF16>);
    STATIC_CHECK(!has_dominant_type<BF16, F16>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, float>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<float, F16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<BF16, float>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<float, BF16>, float>);

    STATIC_CHECK(std::same_as<std::common_type_t<float, F16, BF16>, float>);
    STATIC_CHECK(std::same_as<std::common_type_t<float, BF16, F16>, float>);
    STATIC_CHECK(std::same_as<std::common_type_t<F16, float, BF16>, float>);
    STATIC_CHECK(std::same_as<std::common_type_t<BF16, float, F16>, float>);
    STATIC_CHECK(!has_common_type<F16, BF16, float>);
    STATIC_CHECK(!has_common_type<BF16, F16, float>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<float, F16, BF16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<float, BF16, F16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, float, BF16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<BF16, float, F16>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, BF16, float>, float>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<BF16, F16, float>, float>);

    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, BF16, float, double>, double>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<double, F16, BF16, float>, double>);
    STATIC_CHECK(std::same_as<iris::dominant_type_t<F16, double, BF16, float>, double>);
    STATIC_CHECK(!has_common_type<F16, BF16, float, double>);

    STATIC_CHECK(!has_dominant_type<F16, BF16, F16>);
    STATIC_CHECK(!has_dominant_type<F16, F16, BF16, BF16>);
}

TEST_CASE("symmetric_common_type: basic", "[type_traits]")
{
    STATIC_CHECK(!has_symmetric_common_type<>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<float>, float>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<float const&>, float>);

    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<short, char, int>, int>);

    STATIC_CHECK(!has_dominant_type<short, char>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<short, char>, int>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<char, short>, int>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<signed char, unsigned char>, int>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<short, char, unsigned short>, int>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<unsigned short, char, short>, int>);
}

TEMPLATE_TEST_CASE("symmetric_common_type", "[type_traits]", IRIS_TEST_NARROW_FP_PAIRS)
{
    using F16 = TestType::first_type;
    using BF16 = TestType::second_type;

    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<F16, float>, float>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<float, F16, BF16>, float>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<F16, BF16, float>, float>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<BF16, F16, float>, float>);
    STATIC_CHECK(std::same_as<iris::symmetric_common_type_t<F16, BF16, float, double>, double>);

    STATIC_CHECK(!has_symmetric_common_type<F16, BF16>);
    STATIC_CHECK(!has_symmetric_common_type<BF16, F16>);
    STATIC_CHECK(!has_symmetric_common_type<F16, BF16, F16>);
}

#undef IRIS_TEST_NARROW_FP_PAIRS

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

TEST_CASE("is_convertible_without_narrowing: constexpr caveat")
{
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int const, float>);

    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2026/p0870r8.html#ch5.9
    // The paper wants true in this case.
    // TODO: GCC/Clang and MSVC(2026) disagree; unfixable
    //STATIC_CHECK(iris::is_convertible_without_narrowing_v<std::integral_constant<int, 42>, float>);
}

TEST_CASE("is_convertible_without_narrowing: class types with conversion")
{
    struct convertible_from_int
    {
        convertible_from_int(int);
    };

    struct not_convertible_from_int {};

    // Implicit converting constructor
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<convertible_from_int, int>);

    // Not convertible at all
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, not_convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<not_convertible_from_int, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, std::string>);

    // ------------------------------------------

    struct explicit_from_int
    {
        explicit explicit_from_int() = default;
        explicit explicit_from_int(int) {}
        explicit_from_int& operator=(int) { return *this; }
    };

    // Explicit ctor: is_convertible is false, so trait is false
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, explicit_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_from_int, int>);

    // ------------------------------------------

    struct convertible_to_double
    {
        operator double();
    };
    STATIC_CHECK(!iris::is_convertible_without_narrowing<double, float>::value);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_convertible_without_narrowing<convertible_to_double, float>::value);
}


TEST_CASE("is_convertible_without_narrowing: class types with conversion operator")
{
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<implicit_conversion_op, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, implicit_conversion_op>);

    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<explicit_conversion_op, int>);
}

TEST_CASE("is_convertible_without_narrowing: class types inheritance")
{
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived, base>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<base, derived>);

    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived&, base>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<derived const&, base>);
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
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void, void const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void const, void const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<void const, void>);

    // void to/from anything else: not convertible
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, void>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<void, base>);
}

TEST_CASE("is_convertible_without_narrowing: cv-qualified types")
{
    // const arithmetic: same narrowing rules
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, int const>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const, long long>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long const, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int volatile, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int const volatile, int>);
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

TEST_CASE("is_convertible_without_narrowing: reference types as To")
{
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

    // --------------------------------------------------

    using F = void();

    STATIC_CHECK(std::is_convertible_v<F&, F&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<F&, F&>);

    STATIC_CHECK(std::is_convertible_v<F, F&>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<F, F&>);
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

    // Function pointer to void*: not allowed as per standard, but MSVC accepts this conversion
    // STATIC_CHECK(!iris::is_convertible_without_narrowing_v<fp, void*>);
}

TEST_CASE("is_convertible_without_narrowing: array and function types")
{
    // Array types as From decay to pointers
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int*>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int[3], int const*>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int*, int[3]>);

    // Element types to array types
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, int[3]>);

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


struct Incomplete;

struct NonMovable
{
    NonMovable(NonMovable const&) = delete;
    NonMovable(NonMovable&&) = delete;
};

struct ThrowingMove
{
    ThrowingMove(ThrowingMove&&) noexcept(false);
};

struct ThrowingConv
{
    ThrowingConv(int);
};

struct NothrowConv
{
    NothrowConv(int) noexcept;
};

struct ExplicitConv
{
    explicit ExplicitConv(int);
};

struct ThrowingDtor
{
    ~ThrowingDtor() noexcept(false);
};

struct NoDtor
{
    ~NoDtor() = delete;
};

// Hard error when instantiated
template<class T>
struct Poison
{
    static_assert(sizeof(T) == 0, "Poison<T> must not be instantiated");
    T value;
};

struct WithMembers
{
    [[maybe_unused]] int data;
    [[maybe_unused]] int function() const;
};

struct NonConstCallable
{
    void operator()();
};

struct LvalueOnlyCallable
{
    void operator()() &;
};

struct RvalueOnlyCallable
{
    void operator()() &&;
};

struct GenericCallable
{
    template<class T>
    T operator()(T const&) const noexcept;
};

template<class From, class To>
concept copy_initializable_from_exact = requires {
    iris::copy_initialize<To>(iris::declval_exact<From>());
};

template<class... Ts>
concept has_directly_invoke_result = requires {
    typename iris::directly_invoke_result<Ts...>::type;
};

template<class T>
using declval_exact_t = decltype(iris::declval_exact<T>());

TEST_CASE("declval_exact", "[type_traits]")
{
    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<int&>()), int&>);
    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<int const&>()), int const&>);
    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<int&&>()), int&&>);
    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<void>()), void>);

    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<int>()), int>);
    STATIC_CHECK(std::same_as<decltype(iris::declval_exact<NonMovable>()), NonMovable>);
    STATIC_CHECK(std::same_as<declval_exact_t<Incomplete>, Incomplete>);
    STATIC_CHECK(std::same_as<decltype(std::declval<int>()), int&&>);

    STATIC_CHECK(noexcept(iris::declval_exact<ThrowingMove>()));
    STATIC_CHECK(noexcept(iris::copy_initialize<int>(0)));

    STATIC_CHECK(copy_initializable_from_exact<NonMovable, NonMovable>);
    STATIC_CHECK(!copy_initializable_from_exact<NonMovable&&, NonMovable>);
    STATIC_CHECK(!std::is_convertible_v<NonMovable, NonMovable>);

    STATIC_CHECK(copy_initializable_from_exact<int, ThrowingConv>);
    STATIC_CHECK(!copy_initializable_from_exact<int, ExplicitConv>);
    STATIC_CHECK(!copy_initializable_from_exact<int, void>);
    STATIC_CHECK(!copy_initializable_from_exact<void, int>);
    STATIC_CHECK(!copy_initializable_from_exact<int, int[3]>);

    STATIC_CHECK(noexcept(iris::copy_initialize<NothrowConv>(iris::declval_exact<int>())));
    STATIC_CHECK(!noexcept(iris::copy_initialize<ThrowingConv>(iris::declval_exact<int>())));
    STATIC_CHECK(noexcept(iris::copy_initialize<ThrowingMove>(iris::declval_exact<ThrowingMove>())));
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!noexcept(iris::copy_initialize<ThrowingMove>(iris::declval_exact<ThrowingMove&&>())));
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!noexcept(iris::copy_initialize<ThrowingDtor>(iris::declval_exact<ThrowingDtor>())));
}

TEST_CASE("invoke_convertible", "[type_traits]")
{
    STATIC_CHECK(iris::detail::invoke_convertible<int, void>);
    STATIC_CHECK(iris::detail::invoke_convertible<int, void const>);
    STATIC_CHECK(iris::detail::invoke_convertible<void, void>);
    STATIC_CHECK(!iris::detail::invoke_convertible<void, int>);

    STATIC_CHECK(iris::detail::invoke_convertible<Incomplete, void>);
    STATIC_CHECK(iris::detail::invoke_convertible<Poison<int>, void>);
    STATIC_CHECK(!iris::detail::invoke_convertible<Incomplete, int>);

    STATIC_CHECK(iris::detail::invoke_convertible<NonMovable, NonMovable>);
    STATIC_CHECK(!iris::detail::invoke_convertible<NonMovable&&, NonMovable>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::detail::invoke_convertible<NoDtor, NoDtor>);

    STATIC_CHECK(iris::detail::invoke_convertible<int, long>);
    STATIC_CHECK(iris::detail::invoke_convertible<int, ThrowingConv>);
    STATIC_CHECK(!iris::detail::invoke_convertible<int, ExplicitConv>);
    STATIC_CHECK(!iris::detail::invoke_convertible<int, int*>);
    STATIC_CHECK(iris::detail::invoke_convertible<void(&)(), void(*)()>);

    STATIC_CHECK(!iris::detail::invoke_convertible<int, int const&>);
    STATIC_CHECK(!iris::detail::invoke_convertible<int, int&&>);
    STATIC_CHECK(!iris::detail::invoke_convertible<int&, long const&>);
    STATIC_CHECK(iris::detail::invoke_convertible<int&, int const&>);
    STATIC_CHECK(iris::detail::invoke_convertible<int&&, int&&>);
    STATIC_CHECK(iris::detail::invoke_convertible<int&&, int const&>);
    STATIC_CHECK(!iris::detail::invoke_convertible<int const&, int&>);
}

TEST_CASE("directly_invocable", "[type_traits]")
{
    STATIC_CHECK(iris::directly_invocable<int(*)()>);
    STATIC_CHECK(iris::directly_invocable<int(&)()>);
    STATIC_CHECK(iris::directly_invocable<int()>);
    STATIC_CHECK(iris::directly_invocable<int(*)(int), int>);
    STATIC_CHECK(!iris::directly_invocable<int(*)(int)>);
    STATIC_CHECK(!iris::directly_invocable<int(*)(int), int, int>);
    STATIC_CHECK(!iris::directly_invocable<int(*)(int), void>);
    STATIC_CHECK(!iris::directly_invocable<int>);
    STATIC_CHECK(!iris::directly_invocable<void>);
    STATIC_CHECK(!iris::directly_invocable<int() const>);

    STATIC_CHECK(iris::directly_invocable<NonConstCallable&>);
    STATIC_CHECK(iris::directly_invocable<NonConstCallable>);
    STATIC_CHECK(!iris::directly_invocable<NonConstCallable const&>);
    STATIC_CHECK(iris::directly_invocable<LvalueOnlyCallable&>);
    STATIC_CHECK(!iris::directly_invocable<LvalueOnlyCallable>);
    STATIC_CHECK(!iris::directly_invocable<RvalueOnlyCallable&>);
    STATIC_CHECK(iris::directly_invocable<RvalueOnlyCallable>);
    STATIC_CHECK(iris::directly_invocable<RvalueOnlyCallable&&>);

    STATIC_CHECK(iris::directly_invocable<void(*)(int&), int&>);
    STATIC_CHECK(!iris::directly_invocable<void(*)(int&), int>);
    STATIC_CHECK(!iris::directly_invocable<void(*)(int&), int const&>);
    STATIC_CHECK(iris::directly_invocable<void(*)(int&&), int>);
    STATIC_CHECK(iris::directly_invocable<void(*)(int&&), int&&>);
    STATIC_CHECK(!iris::directly_invocable<void(*)(int&&), int&>);
    STATIC_CHECK(iris::directly_invocable<void(*)(int const&), int>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::directly_invocable<void(*)(NonMovable), NonMovable>);

    STATIC_CHECK(std::invocable<int WithMembers::*, WithMembers&>);
    STATIC_CHECK(!iris::directly_invocable<int WithMembers::*, WithMembers&>);
    STATIC_CHECK(std::invocable<int (WithMembers::*)() const, WithMembers const&>);
    STATIC_CHECK(!iris::directly_invocable<int (WithMembers::*)() const, WithMembers const&>);

    STATIC_CHECK(iris::directly_invocable<Incomplete(*)()>);
    STATIC_CHECK(iris::directly_invocable<NoDtor(*)()>);
    STATIC_CHECK(iris::directly_invocable<Poison<int>(*)()>);

    STATIC_CHECK(iris::is_directly_invocable_v<int(*)()>);
    STATIC_CHECK(!iris::is_directly_invocable_v<int>);
    STATIC_CHECK(iris::is_directly_invocable<GenericCallable const&, int>::value);

    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<int(*)()>, int>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<int&(*)()>, int&>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<int&&(*)()>, int&&>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<void(*)()>, void>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<GenericCallable const&, long>, long>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<Incomplete(*)()>, Incomplete>);
    STATIC_CHECK(std::same_as<iris::directly_invoke_result_t<Poison<int>(*)()>, Poison<int>>);

    // SFINAE-friendly
    STATIC_CHECK(has_directly_invoke_result<int(*)()>);
    STATIC_CHECK(!has_directly_invoke_result<int>);
    STATIC_CHECK(!has_directly_invoke_result<int(*)(), int>);
    STATIC_CHECK(!has_directly_invoke_result<int WithMembers::*, WithMembers&>);
}

TEST_CASE("directly_invocable_r", "[type_traits]")
{
    STATIC_CHECK(iris::directly_invocable_r<void, int(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<void const, NonMovable(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<void, void(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<int, void(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<void, int>);
    STATIC_CHECK(!iris::directly_invocable_r<void, int(*)(int)>);

    STATIC_CHECK(iris::directly_invocable_r<NonMovable, NonMovable(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<NonMovable, NonMovable&&(*)()>);

    STATIC_CHECK(iris::directly_invocable_r<long, int(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<ThrowingConv, int(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<ExplicitConv, int(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<int[3], int(*)()>);

    STATIC_CHECK(!iris::directly_invocable_r<int const&, int(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<int&&, int(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<long const&, int&(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<int const&, int&(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<int&&, int&&(*)()>);

    STATIC_CHECK(iris::directly_invocable_r<void, Incomplete(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<void, NoDtor(*)()>);
    STATIC_CHECK(iris::directly_invocable_r<void, Poison<int>(*)()>);
    STATIC_CHECK(!iris::directly_invocable_r<int, Incomplete(*)()>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::directly_invocable_r<NoDtor, NoDtor(*)()>);

    STATIC_CHECK(iris::is_directly_invocable_r_v<long, int(*)()>);
    STATIC_CHECK(!iris::is_directly_invocable_r_v<int*, int(*)()>);
    STATIC_CHECK(iris::is_directly_invocable_r<void, int(*)()>::value);
}

TEST_CASE("is_nothrow_directly_invocable", "[type_traits]")
{
    STATIC_CHECK(iris::is_nothrow_directly_invocable_v<int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_v<int(*)()>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_v<int>);
    STATIC_CHECK(iris::is_nothrow_directly_invocable_v<GenericCallable const&, int>);
    STATIC_CHECK(iris::is_nothrow_directly_invocable<void(*)(int) noexcept, int>::value);

    STATIC_CHECK(!iris::is_nothrow_directly_invocable_v<void(*)(ThrowingConv) noexcept, int>);
    STATIC_CHECK(iris::is_nothrow_directly_invocable_v<void(*)(NothrowConv) noexcept, int>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_v<ThrowingDtor(*)() noexcept>);

    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_v<NoDtor(*)() noexcept>);

    STATIC_CHECK(iris::is_nothrow_directly_invocable_r_v<void, int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<void, int(*)()>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<void, int>);

    STATIC_CHECK(iris::is_nothrow_directly_invocable_r_v<NonMovable, NonMovable(*)() noexcept>);
    STATIC_CHECK(iris::is_nothrow_directly_invocable_r_v<ThrowingMove, ThrowingMove(*)() noexcept>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<ThrowingMove, ThrowingMove&&(*)() noexcept>);

    STATIC_CHECK(iris::is_nothrow_directly_invocable_r_v<NothrowConv, int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<NothrowConv, int(*)()>);
    STATIC_CHECK(iris::is_directly_invocable_r_v<ThrowingConv, int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<ThrowingConv, int(*)() noexcept>);
    // ReSharper disable once CppStaticAssertFailure
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<ThrowingDtor, ThrowingDtor(*)() noexcept>);

    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<ExplicitConv, int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<int const&, int(*)() noexcept>);
    STATIC_CHECK(!iris::is_nothrow_directly_invocable_r_v<int, void(*)() noexcept>);

    STATIC_CHECK(iris::is_nothrow_directly_invocable_r<bool, GenericCallable const&, bool>::value);
    STATIC_CHECK(std::conjunction_v<
        iris::is_nothrow_directly_invocable_r<int, GenericCallable const&, int>,
        iris::is_nothrow_directly_invocable_r<long, GenericCallable const&, long>
    >);
}

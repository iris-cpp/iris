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

// "Broken" implementation: is_assignable_without_narrowing without the arithmetic guard.
// This demonstrates why the guard is needed.
template<class Dest, class Source>
struct broken_is_assignable_without_narrowing
    : std::bool_constant<
        std::is_assignable_v<Dest, Source> &&
        iris::is_convertible_without_narrowing_v<std::remove_cvref_t<Source>, std::remove_reference_t<Dest>>
    >
{};

enum class scoped_enum {};
enum unscoped_enum {};

} // anonymous


TEST_CASE("is_convertible_without_narrowing")
{
    // Same type: always OK
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<double, double>);

    // Widening integer conversions: OK
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, long long>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<short, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<char, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unsigned, unsigned long long>);

    // Narrowing integer conversions: rejected
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, short>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, char>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned long long, unsigned>);

    // Floating-point widening: OK
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<float, long double>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<double, long double>);

    // Floating-point narrowing: rejected
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long double, double>);

    // Integer to floating-point: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, float>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<long long, double>);

    // Floating-point to integer: narrowing
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<float, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<double, int>);

    // Signed/unsigned mismatch
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, unsigned>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<unsigned, int>);

    // Bool conversions
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<bool, int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, bool>);

    // User-defined implicit conversion: OK (no narrowing concern)
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<int, convertible_from_int>);

    // Not convertible at all
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, not_convertible_from_int>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, std::string>);

    // Enum conversions
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<int, scoped_enum>);
    STATIC_CHECK(!iris::is_convertible_without_narrowing_v<scoped_enum, int>);
    STATIC_CHECK(iris::is_convertible_without_narrowing_v<unscoped_enum, int>);

    // `To[]{from}` works, but not convertible
    {
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
    }

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

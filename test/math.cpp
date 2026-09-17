// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/math.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <bit>
#include <limits>

#include <cstdint>
#include <cmath>

namespace {

template<std::size_t N> struct uint_of_size;
template<> struct uint_of_size<2> { using type = std::uint16_t; };
template<> struct uint_of_size<4> { using type = std::uint32_t; };
template<> struct uint_of_size<8> { using type = std::uint64_t; };

#define IRIS_TEST_ISNAN_TYPES float, double, long double

} // anonymous

TEMPLATE_TEST_CASE("isnan vs std::isnan", "[math]", IRIS_TEST_ISNAN_TYPES)
{
    using T = TestType;
    using limits = std::numeric_limits<T>;

    STATIC_CHECK(!iris::isnan(T{}));
    STATIC_CHECK(!iris::isnan(T{1}));
    STATIC_CHECK(!iris::isnan(limits::infinity()));
    STATIC_CHECK(!iris::isnan(-limits::infinity()));
    STATIC_CHECK(!iris::isnan((limits::max)()));
    STATIC_CHECK(!iris::isnan(limits::denorm_min()));
    STATIC_CHECK(iris::isnan(limits::quiet_NaN()));
    STATIC_CHECK(iris::isnan(-limits::quiet_NaN()));
    STATIC_CHECK(iris::isnan(limits::signaling_NaN()));

    for (T const x : {
        T{}, -T{}, T{1}, T{-1},
        (limits::min)(), (limits::max)(), limits::lowest(), limits::epsilon(), limits::denorm_min(),
        limits::infinity(), -limits::infinity(),
        limits::quiet_NaN(), -limits::quiet_NaN(), limits::signaling_NaN(),
    }) {
        CHECK(iris::isnan(x) == std::isnan(x));
    }

    if constexpr (sizeof(T) <= 8) {
        using uint = uint_of_size<sizeof(T)>::type;
        constexpr int mantissa_bits = limits::digits - 1;
        constexpr uint mantissa_mask = (uint(1) << mantissa_bits) - 1;
        constexpr uint exponent_all_ones = ~mantissa_mask & (uint(-1) >> 1);
        constexpr uint sign = uint(1) << (sizeof(T) * 8 - 1);

        auto const check_bits = [](uint const bits) {
            T const x = std::bit_cast<T>(bits);
            CHECK(iris::isnan(x) == std::isnan(x));
        };

        // Infinity and NaNs with small payloads, both signs
        for (uint payload = 0; payload < 256; ++payload) {
            check_bits(exponent_all_ones | payload);
            check_bits(sign | exponent_all_ones | payload);
        }
        // Largest finite values, both signs
        for (uint payload = 0; payload < 256; ++payload) {
            check_bits((exponent_all_ones - (uint(1) << mantissa_bits)) | (mantissa_mask - payload));
            check_bits(sign | (exponent_all_ones - (uint(1) << mantissa_bits)) | (mantissa_mask - payload));
        }
        // NaN with only the top mantissa bit set (quiet NaN on most platforms) and only the lowest bit set
        check_bits(exponent_all_ones | (uint(1) << (mantissa_bits - 1)));
        check_bits(exponent_all_ones | uint(1));
    }
}

#undef IRIS_TEST_ISNAN_TYPES

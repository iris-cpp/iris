#ifndef IRIS_ZZ_MATH_HPP
#define IRIS_ZZ_MATH_HPP

// SPDX-License-Identifier: MIT

#include <iris/stdint.hpp>

#include <concepts>
#include <bit>
#include <limits>

#include <cstddef> // IWYU pragma: keep
#include <cmath> // IWYU pragma: keep

namespace iris {

namespace detail {

template<class T>
concept bit_inspectable_floating_point =
    std::floating_point<T> &&
    std::numeric_limits<T>::radix == 2 &&
    requires { typename unsigned_integer_of_size_t<sizeof(T)>; };

} // detail

template<std::floating_point T>
[[nodiscard]] constexpr bool isnan(T const x) noexcept
{
    if constexpr (detail::bit_inspectable_floating_point<T>) {
        using uint = unsigned_integer_of_size_t<sizeof(T)>;
        constexpr int mantissa_bits = std::numeric_limits<T>::digits - 1;
        constexpr int exponent_bits = int(sizeof(T) * 8) - 1 - mantissa_bits;
        constexpr uint mantissa_mask = (uint(1) << mantissa_bits) - 1;
        constexpr uint exponent_mask = ((uint(1) << exponent_bits) - 1) << mantissa_bits;
        uint const bits = std::bit_cast<uint>(x);
        return (bits & exponent_mask) == exponent_mask && (bits & mantissa_mask) != 0;
    } else {
        return x != x;
    }
}

template<std::integral T>
[[nodiscard]] constexpr bool isnan(T) noexcept
{
    return false;
}

} // iris

#endif

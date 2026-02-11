#ifndef IRIS_ENUM_BITOPS_HPP
#define IRIS_ENUM_BITOPS_HPP

// SPDX-License-Identifier: MIT

#include <concepts>
#include <type_traits>
#include <utility>

#ifndef NDEBUG
#include <bit>
#endif

#include <cassert>

namespace iris {

template<class T>
struct bitops_enabled : std::false_type {};

template<class T>
constexpr bool bitops_enabled_v = bitops_enabled<T>::value;

namespace detail {

template<class T>
concept bitops_enum_has_min_bit = std::same_as<std::remove_const_t<decltype(bitops_enabled<T>::min_bit)>, int>;

template<class T>
concept bitops_enum_has_max_bit = std::same_as<std::remove_const_t<decltype(bitops_enabled<T>::max_bit)>, int>;

} // detail

template<class T>
concept BitopsEnabledEnum = std::is_enum_v<T> && bitops_enabled_v<T>;

inline namespace bitops_operators {

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T operator~(T a) noexcept
{
    return static_cast<T>(~std::to_underlying(a));
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T operator|(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return static_cast<T>(std::to_underlying(a) | std::to_underlying(b));
}

template<BitopsEnabledEnum T>
constexpr T& operator|=(T& a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return a = a | b;
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T operator&(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return static_cast<T>(std::to_underlying(a) & std::to_underlying(b));
}

template<BitopsEnabledEnum T>
constexpr T& operator&=(T& a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return a = a & b;
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T operator^(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return static_cast<T>(std::to_underlying(a) ^ std::to_underlying(b));
}

template<BitopsEnabledEnum T>
constexpr T& operator^=(T& a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return a = a ^ b;
}

} // bitops_operators

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr bool contains(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return (std::to_underlying(a) & std::to_underlying(b)) == std::to_underlying(b);
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr bool contains_single_bit(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    assert(std::has_single_bit(std::to_underlying(b)));
    return std::to_underlying(a) & std::to_underlying(b);
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr bool contains_any_bit(T a, T b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
    return std::to_underlying(a) & std::to_underlying(b);
}

} // iris

#endif

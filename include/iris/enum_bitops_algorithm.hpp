#ifndef IRIS_ENUM_BITOPS_ALGORITHM_HPP
#define IRIS_ENUM_BITOPS_ALGORITHM_HPP

// SPDX-License-Identifier: MIT

#include <iris/enum_bitops.hpp>

#include <limits>
#include <ranges>
#include <utility>

namespace iris {

namespace detail {

struct each_bit_fn
{
    template<BitopsEnabledEnum T>
    [[nodiscard]] static constexpr auto operator()(T flags) noexcept
    {
        static_assert(std::is_unsigned_v<std::underlying_type_t<T>>);
        static_assert(detail::bitops_enum_has_max_bit<T>);
        static_assert(bitops_enabled<T>::max_bit < std::numeric_limits<std::underlying_type_t<T>>::digits);

        if constexpr (detail::bitops_enum_has_min_bit<T>) {
        static_assert(bitops_enabled<T>::min_bit <= bitops_enabled<T>::max_bit);
            return std::views::iota(bitops_enabled<T>::min_bit, bitops_enabled<T>::max_bit + 1)
                | std::views::filter([cat = std::to_underlying(flags)](int i) constexpr noexcept -> bool { return (cat >> i) & 1; })
                | std::views::transform([](int i) constexpr noexcept { return static_cast<T>(static_cast<std::underlying_type_t<T>>(1) << i); });
        } else {
            return std::views::iota(0, bitops_enabled<T>::max_bit + 1)
                | std::views::filter([cat = std::to_underlying(flags)](int i) constexpr noexcept -> bool { return (cat >> i) & 1; })
                | std::views::transform([](int i) constexpr noexcept { return static_cast<T>(static_cast<std::underlying_type_t<T>>(1) << i); });
        }
    }
};

} // detail

inline constexpr detail::each_bit_fn each_bit{};

} // iris

#endif

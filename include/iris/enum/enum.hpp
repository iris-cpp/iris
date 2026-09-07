#ifndef IRIS_ZZ_ENUM_ENUM_HPP
#define IRIS_ZZ_ENUM_ENUM_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <format>
#include <limits>
#include <ranges>
#include <utility>
#include <array>
#include <concepts>
#include <type_traits>
#include <numbers>

#ifndef NDEBUG
#include <bit>
#endif

#include <cassert>
#include <cstddef> // IWYU pragma: keep

namespace iris {

namespace detail {

struct enum_traits_not_defined {};

} // detail

template<class EnumT>
struct enum_traits : detail::enum_traits_not_defined
{};

template<class EnumT>
concept Enum = std::is_enum_v<EnumT> && requires {
    typename enum_traits<EnumT>;
    requires !std::derived_from<enum_traits<EnumT>, detail::enum_traits_not_defined>;
};

namespace detail {

template<class EnumT>
concept enum_has_finite_minmax_element = requires {
    requires std::same_as<std::remove_const_t<decltype(enum_traits<EnumT>::min_element)>, EnumT>;
    requires std::same_as<std::remove_const_t<decltype(enum_traits<EnumT>::max_element)>, EnumT>;
    requires std::to_underlying(enum_traits<EnumT>::min_element) <= std::to_underlying(enum_traits<EnumT>::max_element);
};

template<class EnumT>
concept enum_has_min_bit = requires {
    requires std::same_as<std::remove_const_t<decltype(enum_traits<EnumT>::min_bit)>, int>;
};
template<class EnumT>
concept enum_has_proper_min_bit = !requires { enum_traits<EnumT>::min_bit; } || enum_has_min_bit<EnumT>;

template<class EnumT>
constexpr int enum_min_bit_v = 0;

template<class EnumT>
    requires enum_has_min_bit<EnumT>
constexpr int enum_min_bit_v<EnumT> = enum_traits<EnumT>::min_bit;


template<class EnumT>
concept enum_has_max_bit = requires {
    requires std::same_as<std::remove_const_t<decltype(enum_traits<EnumT>::max_bit)>, int>;
};
template<class EnumT>
concept enum_has_proper_max_bit = !requires { enum_traits<EnumT>::max_bit; } || enum_has_max_bit<EnumT>;

template<class EnumT>
concept enum_has_finite_minmax_bit =
    std::unsigned_integral<std::underlying_type_t<EnumT>> &&
    enum_has_proper_min_bit<EnumT> &&
    enum_has_proper_max_bit<EnumT> &&
    requires {
        requires enum_min_bit_v<EnumT> <= enum_traits<EnumT>::max_bit;
    };

template<class EnumT>
struct enum_count_impl;

template<class EnumT>
    requires detail::enum_has_finite_minmax_bit<EnumT>
struct enum_count_impl<EnumT>
    : std::integral_constant<std::size_t, enum_traits<EnumT>::max_bit + 1 - detail::enum_min_bit_v<EnumT>>
{};

template<class EnumT>
    requires detail::enum_has_finite_minmax_element<EnumT> && (!detail::enum_has_finite_minmax_bit<EnumT>)
struct enum_count_impl<EnumT>
    : std::integral_constant<
        std::size_t,
        static_cast<std::size_t>(enum_traits<EnumT>::max_element) + 1uz -
        static_cast<std::size_t>(enum_traits<EnumT>::min_element)
    >
{};

} // detail


template<class EnumT>
concept FiniteEnum = Enum<EnumT> && (detail::enum_has_finite_minmax_bit<EnumT> || detail::enum_has_finite_minmax_element<EnumT>);

template<FiniteEnum EnumT>
constexpr std::size_t enum_count_v = detail::enum_count_impl<EnumT>::value;

template<class EnumT>
concept BitopsEnabledEnum =
    Enum<EnumT> &&
    std::unsigned_integral<std::underlying_type_t<EnumT>> &&
    detail::enum_has_proper_min_bit<EnumT> &&
    detail::enum_has_proper_max_bit<EnumT>;


template<Enum EnumT>
[[nodiscard]] consteval auto enum_values() noexcept
{
    static_assert(FiniteEnum<EnumT>);

    if constexpr (detail::enum_has_finite_minmax_bit<EnumT>) {
        return []<int... Offsets>(std::integer_sequence<int, Offsets...>) noexcept {
            return std::array<EnumT, enum_count_v<EnumT>>{
                static_cast<EnumT>(1u << (detail::enum_min_bit_v<EnumT> + Offsets))...
            };
        }(std::make_integer_sequence<int, static_cast<int>(enum_count_v<EnumT>)>{});

    } else {
        constexpr auto first_val = std::to_underlying(enum_traits<EnumT>::min_element);
        return []<class T, auto... Offsets>(std::integer_sequence<T, Offsets...>) noexcept {
            return std::array<EnumT, enum_count_v<EnumT>>{
                static_cast<EnumT>(first_val + Offsets)...
            };
        }(std::make_integer_sequence<std::underlying_type_t<EnumT>, enum_count_v<EnumT>>{});
    }
}


inline namespace enum_bitops_operators {

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr EnumT operator~(EnumT a) noexcept
{
    return static_cast<EnumT>(~std::to_underlying(a));
}

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr EnumT operator|(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return static_cast<EnumT>(std::to_underlying(a) | std::to_underlying(b));
}

template<BitopsEnabledEnum EnumT>
constexpr EnumT& operator|=(EnumT& a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return a = a | b;
}

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr EnumT operator&(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return static_cast<EnumT>(std::to_underlying(a) & std::to_underlying(b));
}

template<BitopsEnabledEnum EnumT>
constexpr EnumT& operator&=(EnumT& a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return a = a & b;
}

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr EnumT operator^(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return static_cast<EnumT>(std::to_underlying(a) ^ std::to_underlying(b));
}

template<BitopsEnabledEnum EnumT>
constexpr EnumT& operator^=(EnumT& a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return a = a ^ b;
}

} // enum_bitops_operators

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr bool contains(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return (std::to_underlying(a) & std::to_underlying(b)) == std::to_underlying(b);
}

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr bool contains_single_bit(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    assert(std::has_single_bit(std::to_underlying(b)));
    return std::to_underlying(a) & std::to_underlying(b);
}

template<BitopsEnabledEnum EnumT>
[[nodiscard]] constexpr bool contains_any_bit(EnumT a, EnumT b) noexcept
{
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumT>>);
    return std::to_underlying(a) & std::to_underlying(b);
}


namespace detail {

struct each_bit_fn
{
    template<FiniteEnum EnumT>
        requires
            BitopsEnabledEnum<EnumT> &&
            requires {
                requires enum_traits<EnumT>::max_bit < std::numeric_limits<std::underlying_type_t<EnumT>>::digits;
            }
    [[nodiscard]] static constexpr auto operator()(EnumT flags) noexcept
    {
        return std::views::iota(enum_min_bit_v<EnumT>, enum_traits<EnumT>::max_bit + 1)
            | std::views::filter([cat = std::to_underlying(flags)](int i) constexpr noexcept -> bool { return (cat >> i) & 1; })
            | std::views::transform([](int i) constexpr noexcept { return static_cast<EnumT>(static_cast<std::underlying_type_t<EnumT>>(1u) << i); });
    }
};

} // detail

inline constexpr detail::each_bit_fn each_bit{};

} // iris

template<iris::Enum EnumT, class CharT>
    requires requires(EnumT const& val) {
        { to_string(val) } -> std::formattable<CharT>;
    }
struct std::formatter<EnumT, CharT> : std::formatter<std::underlying_type_t<EnumT>, CharT>
{
    using base_formatter = std::formatter<std::underlying_type_t<EnumT>, CharT>;

    template<class Context>
    constexpr auto parse(Context& ctx)
    {
        if (ctx.begin() == ctx.end()) return ctx.end();
        has_format_spec_ = true;
        return base_formatter::parse(ctx);
    }

    template<class Context>
    auto format(EnumT const& val, Context& ctx) const
    {
        if (has_format_spec_) {
            return base_formatter::format(std::to_underlying(val), ctx);
        }
        return format_to(ctx.out(), "{}", to_string(val));
    }

private:
    bool has_format_spec_ = false;
};

#endif

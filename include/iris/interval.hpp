#ifndef IRIS_ZZ_INTERVAL_HPP
#define IRIS_ZZ_INTERVAL_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/format_traits.hpp>

#include <iterator>
#include <format>
#include <concepts>
#include <compare>
#include <type_traits>
#include <utility>

namespace iris {

template<std::signed_integral T>
struct interval
{
    using value_type = T;
    T lower, upper;

    constexpr interval() noexcept
        : lower{}
        , upper{}
    {}

    constexpr interval(T lower, T upper) noexcept
        : lower(lower)
        , upper(upper)
    {}

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        // Malformed interval is treated as "empty"
        return lower >= upper;
    }

    [[nodiscard]] constexpr value_type length() const noexcept
    {
        return static_cast<value_type>(upper - lower);
    }

    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool intersects(interval<U> other) const noexcept
    {
        return (lower < other.upper && other.lower < upper) && !empty() && !other.empty();
    }

    // !intersects
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool disjoint(interval<U> other) const noexcept
    {
        return (upper <= other.lower || other.upper <= lower) || empty() || other.empty();
    }

    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool touches(interval<U> other) const noexcept
    {
        return (upper == other.lower || other.upper == lower) && !empty() && !other.empty();
    }

    // intersects || touches
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool connected(interval<U> other) const noexcept
    {
        return (lower <= other.upper && other.lower <= upper) && !empty() && !other.empty();
    }

    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool covers(interval<U> other) const noexcept
    {
        return (lower <= other.lower && other.upper <= upper) || other.empty();
    }

    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool equals(interval<U> other) const noexcept
    {
        return (lower == other.lower && upper == other.upper) || (empty() && other.empty());
    }

    // -------------------------------------------

    [[nodiscard]] constexpr bool contains(value_type p) const noexcept
    {
        return lower <= p && p < upper;
    }

    // -------------------------------------------

    // Note: This does not reflect mathematical definition like `equals(...)`; this always checks exact data representation
    [[nodiscard]] constexpr bool operator==(interval const&) const noexcept = default;

    // Note: This does not reflect mathematical definition like `equals(...)`; this always checks exact data representation
    [[nodiscard]] constexpr std::strong_ordering operator<=>(interval const&) const noexcept = default;
};

template<std::size_t I, class T>
[[nodiscard]] constexpr T& get(interval<T>& iv) noexcept
{
    static_assert(I == 0 || I == 1);
    if constexpr (I == 0) { return iv.lower; } else { return iv.upper; }
}
template<std::size_t I, class T>
[[nodiscard]] constexpr T const& get(interval<T> const& iv) noexcept
{
    static_assert(I == 0 || I == 1);
    if constexpr (I == 0) { return iv.lower; } else { return iv.upper; }
}
template<std::size_t I, class T>
[[nodiscard]] constexpr T&& get(interval<T>&& iv) noexcept
{
    static_assert(I == 0 || I == 1);
    if constexpr (I == 0) { return std::move(iv).lower; } else { return std::move(iv).upper; }
}
template<std::size_t I, class T>
[[nodiscard]] constexpr T const&& get(interval<T> const&& iv) noexcept
{
    static_assert(I == 0 || I == 1);
    if constexpr (I == 0) { return std::move(iv).lower; } else { return std::move(iv).upper; }
}

} // iris

template<class T>
struct std::tuple_size<iris::interval<T>>
    : std::integral_constant<std::size_t, 2>
{};

template<std::size_t I, class T>
struct std::tuple_element<I, iris::interval<T>>
{
    using type = T;
};

template<class T, class CharT>
struct std::formatter<iris::interval<T>, CharT>
{
    [[nodiscard]] constexpr std::basic_format_parse_context<CharT>::const_iterator
    parse(std::basic_format_parse_context<CharT>& ctx)
    {
        auto const first = ctx.begin();
        if (first == ctx.end()) return first;
        if (*first == iris::format_traits<CharT>::brace_close) return first;

        // Bound the search to this replacement field
        auto const close_it = std::find(
            first, ctx.end(),
            iris::format_traits<CharT>::brace_close
        );
        if (close_it == ctx.end()) {
            throw std::format_error("unterminated format specifier");
        }

        auto const comma_it = std::find(
            first, close_it,
            iris::format_traits<CharT>::comma
        );
        if (comma_it == close_it) {
            throw std::format_error("expected ',' in format specifier");
        }

        {
            std::basic_format_parse_context<CharT> left_ctx{
                std::basic_string_view<CharT>{first, comma_it}
            };
            if (left_fmt_.parse(left_ctx) != left_ctx.end()) {
                throw std::format_error("trailing characters in lower format specifier");
            }
        }
        {
            std::basic_format_parse_context<CharT> right_ctx{
                std::basic_string_view<CharT>{std::next(comma_it), close_it}
            };
            if (right_fmt_.parse(right_ctx) != right_ctx.end()) {
                throw std::format_error("trailing characters in upper format specifier");
            }
        }
        return close_it;
    }

    template<class Ctx>
    Ctx::iterator format(iris::interval<T> const& iv, Ctx& ctx) const
    {
        ctx.advance_to(std::format_to(ctx.out(), "{}", iris::format_traits<CharT>::square_brace_open));
        left_fmt_.format(iv.lower, ctx);
        ctx.advance_to(std::format_to(ctx.out(), "{}", iris::format_traits<CharT>::comma));
        right_fmt_.format(iv.upper, ctx);
        return std::format_to(ctx.out(), "{}", iris::format_traits<CharT>::paren_close);
    }

private:
    std::formatter<T, CharT> left_fmt_, right_fmt_;
};

#endif

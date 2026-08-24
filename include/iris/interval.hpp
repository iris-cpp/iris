#ifndef IRIS_ZZ_INTERVAL_HPP
#define IRIS_ZZ_INTERVAL_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/format_traits.hpp>
#include <iris/string.hpp>

#include <algorithm>
#include <string_view>
#include <iterator>
#include <ranges>
#include <format>
#include <concepts>
#include <compare>
#include <type_traits>
#include <utility>
#include <stdexcept>

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

    // Returns `true` when `*this` is ∅ or malformed (flipped).
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return lower >= upper;
    }

    // Returns the length of the bounds.
    // Note: Always returns `0` for any ∅ positioned at any offset,
    //       including the ones with malformed bounds.
    [[nodiscard]] constexpr value_type length() const noexcept
    {
        return empty() ? value_type{0} : static_cast<value_type>(upper - lower);
    }

    // Returns `true` if bounds are not flipped.
    // Note: Always returns `true` if `*this` is ∅ or malformed.
    [[nodiscard]] constexpr bool is_proper() const noexcept
    {
        return lower <= upper;
    }

    // Returns `true` if `other` shares any point with `*this`.
    // Note 1: intersects(∅) always returns `false`.
    // Note 2: Adjacent-only contact is touches(); for intersects-or-touches use connected().
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool intersects(interval<U> const other) const noexcept
    {
        return (lower < other.upper && other.lower < upper) && !empty() && !other.empty();
    }

    // !intersects
    // Note: disjoint(∅) always returns `true`.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool disjoint(interval<U> const other) const noexcept
    {
        return (upper <= other.lower || other.upper <= lower) || empty() || other.empty();
    }

    // Closures meet but the sets share no point.
    // Note: touches(∅) always returns `false`.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool touches(interval<U> const other) const noexcept
    {
        return (upper == other.lower || other.upper == lower) && !empty() && !other.empty();
    }

    // intersects || touches
    // Note: connected(∅) always returns `false`.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool connected(interval<U> const other) const noexcept
    {
        return (lower <= other.upper && other.lower <= upper) && !empty() && !other.empty();
    }

    // Returns `true` if every point of `other` is a point of `*this`.
    // Note: covers(∅) always returns `true`.
    // See also: `encloses(other)`.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool covers(interval<U> const other) const noexcept
    {
        return (lower <= other.lower && other.upper <= upper) || other.empty();
    }

    // Returns `true` if `other`'s bounds lie within [lower, upper].
    // For nonempty `other`: identical to `covers(other)`.
    // For empty `other`: position-respecting (treats it like a 0-length "text caret".)
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool encloses(interval<U> const other) const noexcept
    {
        return lower <= other.lower && other.upper <= upper;
    }

    // Returns `true` if p ∈ [lower, upper).
    [[nodiscard]] constexpr bool contains(value_type p) const noexcept
    {
        return lower <= p && p < upper;
    }

    // Returns `is_proper() && interval{0, r.size()}.encloses(*this)`.
    template<std::ranges::sized_range R>
    [[nodiscard]] constexpr bool within(R const& r) const
        noexcept(noexcept(std::ranges::size(r)))
    {
        return is_proper() && 0 <= lower && static_cast<std::ranges::range_size_t<R>>(upper) <= std::ranges::size(r);
    }

    // Returns `is_proper() && interval{0, N - 1}.encloses(*this)`.
    template<CharLike CharT, std::size_t N>
    [[nodiscard]] constexpr bool within(CharT const (&)[N]) const noexcept
    {
        static_assert(N >= 1);
        return is_proper() && 0 <= lower && static_cast<std::size_t>(upper) <= N - 1;
    }

    // Returns `true` if both intervals have exactly same bounds.
    // Note: All empty intervals denote ∅ and are mutually equal regardless of
    //       bounds. Differs from `operator==`, which compares data representations.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr bool equals(interval<U> const other) const noexcept
    {
        return (lower == other.lower && upper == other.upper) || (empty() && other.empty());
    }

    // -------------------------------------------

    // A ∩ B. Result is canonical empty [0,0) when the sets share no point.
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr interval intersection(interval<U> const other) const noexcept
    {
        auto const lo = std::max<value_type>(lower, static_cast<value_type>(other.lower));
        auto const hi = std::min<value_type>(upper, static_cast<value_type>(other.upper));
        return lo < hi ? interval{lo, hi} : interval{};
    }
    template<std::signed_integral U = T>
    [[nodiscard]] constexpr interval operator&(interval<U> const other) const noexcept
    {
        return intersection(other);
    }

    // -------------------------------------------

    template<class R>
        requires (!std::ranges::borrowed_range<R>)
    constexpr void as_subview_of(R const&&) const = delete;

    template<std::ranges::forward_range R>
    [[nodiscard]] constexpr auto as_subview_of(R const& r) const
    {
        if (!is_proper() || lower < 0) {
            throw std::domain_error(std::format("interval [{},{}) cannot form a subview; requires 0 <= lower <= upper", lower, upper));
        }
        if constexpr (std::ranges::sized_range<R>) {
            auto const size = std::ranges::size(r);
            if (static_cast<std::ranges::range_size_t<R>>(upper) > size) {
                throw std::out_of_range(std::format("interval [{},{}) cannot form a subview; requires 0 <= lower <= upper <= {}", lower, upper, size));
            }
        }

        if constexpr (requires { r.subview(lower, length()); }) {
            return r.subview(lower, length());

        } else if constexpr (StringLike<R>) {
            using SV = std::basic_string_view<std::ranges::range_value_t<R>>;
            return SV{r}.substr(
                static_cast<std::size_t>(lower),
                static_cast<std::size_t>(length())
            );

        } else {
            auto const n = static_cast<std::ranges::range_difference_t<R>>(length());
            auto const first = std::ranges::next(std::ranges::begin(r), lower, std::ranges::end(r));
            return std::ranges::subrange(first, std::ranges::next(first, n, std::ranges::end(r)));
        }
    }

    template<CharLike CharT, std::size_t N>
    [[nodiscard]] constexpr auto as_subview_of(CharT const (&r)[N]) const
    {
        static_assert(N >= 1);
        return this->as_subview_of(std::basic_string_view{r, N - 1});
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

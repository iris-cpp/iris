#ifndef IRIS_ZZ_STRING_ALGO_HPP
#define IRIS_ZZ_STRING_ALGO_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/string.hpp> // IWYU pragma: keep
#include <iris/stdint.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <algorithm>
#include <type_traits>
#include <array>

#include <cassert>

namespace iris {

namespace detail {

template<class CharT>
struct string_algo_traits;

template<>
struct string_algo_traits<char>
{
    static constexpr char space = ' ';
    static constexpr std::string_view ordinary_spaces{"\u0020\t\r\n"};
    static constexpr std::string_view space_like_variant_chars{"\t\r\n"};
};

template<>
struct string_algo_traits<char32_t>
{
    static constexpr char32_t space = U' ';
    static constexpr std::u32string_view ordinary_spaces{U"\u0020\u3000\t\r\n"}; // includes Japanese space
    static constexpr std::u32string_view space_like_variant_chars{U"\u3000\t\r\n"};
};

} // detail

// TODO: make these CPO so that they can be passed to `std::views::transform`

template<class CharT, class TraitsT>
constexpr void trim_edges(
    std::basic_string<CharT, TraitsT>& input,
    std::type_identity_t<std::basic_string_view<CharT, TraitsT>> const spaces = detail::string_algo_traits<CharT>::ordinary_spaces
)
{
    auto const first = input.find_first_not_of(spaces);
    if (first == std::basic_string<CharT, TraitsT>::npos) {
        input.clear();
        return;
    }
    input.erase(0, first);

    auto const last = input.find_last_not_of(spaces);
    input.erase(last + 1);
}

template<int = 0>
[[nodiscard]] constexpr std::string trim_edges_copy(
    std::string_view input,
    std::string_view const spaces = detail::string_algo_traits<char>::ordinary_spaces
)
{
    std::string buf(input);
    iris::trim_edges(buf, spaces);
    return buf;
}

template<int = 0>
[[nodiscard]] constexpr std::u32string trim_edges_copy(
    std::u32string_view input,
    std::u32string_view const spaces = detail::string_algo_traits<char32_t>::ordinary_spaces
)
{
    std::u32string buf(input);
    iris::trim_edges(buf, spaces);
    return buf;
}


template<class CharT, class TraitsT>
constexpr void normalize_spaces(
    std::basic_string<CharT, TraitsT>& input,
    std::type_identity_t<std::basic_string_view<CharT, TraitsT>> const space_like_variant_chars = detail::string_algo_traits<CharT>::space_like_variant_chars,
    CharT const to_space = detail::string_algo_traits<CharT>::space
)
{
    std::ranges::replace_if(input, [&](CharT const ch) {
        return space_like_variant_chars.contains(ch);
    }, to_space);
}

template<int = 0>
[[nodiscard]] constexpr std::string normalize_spaces_copy(
    std::string_view input,
    std::string_view const space_like_variant_chars = detail::string_algo_traits<char>::space_like_variant_chars,
    char const to_space = detail::string_algo_traits<char>::space
)
{
    std::string buf(input);
    iris::normalize_spaces(buf, space_like_variant_chars, to_space);
    return buf;
}

template<int = 0>
[[nodiscard]] constexpr std::u32string normalize_spaces_copy(
    std::u32string_view input,
    std::u32string_view const space_like_variant_chars = detail::string_algo_traits<char32_t>::space_like_variant_chars,
    char32_t const to_space = detail::string_algo_traits<char32_t>::space
)
{
    std::u32string buf(input);
    iris::normalize_spaces(buf, space_like_variant_chars, to_space);
    return buf;
}


template<class CharT, class TraitsT>
constexpr void compact_spaces(
    std::basic_string<CharT, TraitsT>& input,
    std::type_identity_t<std::basic_string_view<CharT, TraitsT>> const spaces = detail::string_algo_traits<CharT>::ordinary_spaces,
    CharT const to_space = detail::string_algo_traits<CharT>::space
)
{
    iris::trim_edges(input, spaces);
    if (input.empty()) return;

    std::size_t w = 0;
    bool in_space = false;

    for (CharT const c : input) {
        if (spaces.contains(c)) {
            if (!in_space) {
                input[w++] = to_space;
            }
            in_space = true;

        } else {
            input[w++] = c;
            in_space = false;
        }
    }
    input.resize(w);
}

template<int = 0>
[[nodiscard]] constexpr std::string compact_spaces_copy(
    std::string_view input,
    std::string_view const spaces = detail::string_algo_traits<char>::ordinary_spaces,
    char const to_space = detail::string_algo_traits<char>::space
)
{
    std::string buf(input);
    iris::compact_spaces(buf, spaces, to_space);
    return buf;
}

template<int = 0>
[[nodiscard]] constexpr std::u32string compact_spaces_copy(
    std::u32string_view input,
    std::u32string_view const spaces = detail::string_algo_traits<char32_t>::ordinary_spaces,
    char32_t const to_space = detail::string_algo_traits<char32_t>::space
)
{
    std::u32string buf(input);
    iris::compact_spaces(buf, spaces, to_space);
    return buf;
}

// ---------------------------------------------

// Replaces all occurrences of each elem of `escape_targets` with `{leader, each elem of escape_targets}`.
// `leader` is automatically included in escape targets by default; you don't need to
// specify it in `escape_targets`.
template<class CharT, class TraitsT>
constexpr void escape(
    std::basic_string<CharT, TraitsT>& input,
    CharT const leader /* e.g. '\' */,
    std::type_identity_t<std::basic_string_view<CharT, TraitsT>> escape_targets /* e.g. "\"" */
)
{
    assert(!escape_targets.contains(leader) && "you don't need to include `leader` in `escape_targets`");

    auto const is_leader_needed = [&](CharT c) noexcept {
        return c == leader || escape_targets.find(c) != std::basic_string_view<CharT, TraitsT>::npos;
    };
    std::size_t const extra = std::ranges::count_if(input, is_leader_needed);
    if (extra == 0) return;

    input.resize(input.size() + extra);
    auto out = input.rbegin();
    for (auto in = input.rbegin() + extra; in != input.rend(); ++in) {
        *out++ = *in;
        if (is_leader_needed(*in)) {
            *out++ = leader;
        }
    }
}

template<int = 0>
[[nodiscard]] constexpr std::string escape_copy(
    std::string_view input,
    char const leader,
    std::string_view const escape_targets
)
{
    std::string buf(input);
    iris::escape(buf, leader, escape_targets);
    return buf;
}

template<int = 0>
[[nodiscard]] constexpr std::u32string escape_copy(
    std::u32string_view input,
    char32_t const leader,
    std::u32string_view const escape_targets
)
{
    std::u32string buf(input);
    iris::escape(buf, leader, escape_targets);
    return buf;
}

// Inverse of `escape(...)`.
template<class CharT, class TraitsT>
constexpr void unescape(
    std::basic_string<CharT, TraitsT>& input,
    CharT const leader /* e.g. '\' */
)
{
    std::size_t const first = input.find(leader);
    if (first == std::basic_string<CharT, TraitsT>::npos) return;

    auto out = input.begin() + first;
    auto const end = input.end();
    for (auto in = out; in != end; ++in, ++out) {
        if (*in == leader && std::next(in) != end) {
            ++in; // skip the leader, copy the escaped char below
        }
        *out = *in;
    }
    input.erase(out, end);
}

template<int = 0>
[[nodiscard]] constexpr std::string unescape_copy(
    std::string_view input,
    char const leader
)
{
    std::string buf(input);
    iris::unescape(buf, leader);
    return buf;
}

template<int = 0>
[[nodiscard]] constexpr std::u32string unescape_copy(
    std::u32string_view input,
    char32_t const leader
)
{
    std::u32string buf(input);
    iris::unescape(buf, leader);
    return buf;
}


namespace detail {

// Closed range [first, last] of character set
template<class CharT>
struct char_range
{
    using char_type = CharT;
    using value_type = make_unsigned_of_size_t<CharT>;

    CharT first, last;

    consteval char_range(CharT first, CharT last)
        : first(first)
        , last(last)
    {
        if (first > last) throw std::domain_error{"char_range must be proper"};
    }

    [[nodiscard]] constexpr std::size_t count() const noexcept
    {
        return static_cast<std::size_t>(
            static_cast<value_type>(last) - static_cast<value_type>(first)
        ) + 1uz;
    }
};

template<class FromT, class ToT = FromT>
struct char_substitute
{
    using char_type = FromT::char_type;
    using value_type = make_unsigned_of_size_t<char_type>;

    FromT from;
    ToT to;

    consteval char_substitute(FromT from, ToT to)
        : from(from)
        , to(to)
    {
        if (from.count() != to.count()) {
            throw std::out_of_range{"from.count() does not match to.count()"};
        }
    }

    [[nodiscard]] constexpr bool substitute(char_type& ch) const noexcept
    {
        if (from.first <= ch && ch <= from.last) {
            value_type const ofs = static_cast<value_type>(ch) - static_cast<value_type>(from.first);
            ch = static_cast<char_type>(static_cast<value_type>(to.first) + ofs);
            return true;
        }
        return false;
    }
};

template<class CharT>
inline constexpr auto jp_substitutes = std::to_array<char_substitute<char_range<CharT>>>({
    {{U'０', U'９'}, {U'0', U'9'}},
    {{U'Ａ', U'Ｚ'}, {U'A', U'Z'}},
    {{U'ａ', U'ｚ'}, {U'a', U'z'}},
});

} // detail

// Normalizes occurrences of "ordinary" inconsistent spelling with
// the canonicalized letter.
//
// This algorithm guarantees the character count never changes regardless
// of the transformation, in contrast to fully-featured Unicode normalization
// such as NFKC. (Note: NFKC not only changes the character count; it may
// occasionally render the resulting character to empty set, or even *flips*
// the character positioning, which makes it require a rather complex algo.)
//
// The above design choice enables this function to be used like an *easy*
// canonicalization, where the original and the transformed string can both
// be mapped to the identical `interval<int>`.
template<class CharT, class TraitsT>
constexpr void ordinary_normalize(std::basic_string<CharT, TraitsT>& input)
{
    static_assert(std::same_as<CharT, char32_t>, "sorry, not implemented");

    for (CharT& ch : input) {
        switch (ch) {
        case U'　': ch = U' '; continue;
        default: break;
        }

        for (auto const& subs : detail::jp_substitutes<CharT>) {
            if (subs.substitute(ch)) {
                break;
            }
        }
    }
}

//template<int = 0>
//[[nodiscard]] constexpr std::string ordinary_normalize_copy(std::string_view input)
//{
//    std::string buf(input);
//    iris::ordinary_normalize(buf);
//    return buf;
//}

template<int = 0>
[[nodiscard]] constexpr std::u32string ordinary_normalize_copy(std::u32string_view input)
{
    std::u32string buf(input);
    iris::ordinary_normalize(buf);
    return buf;
}

} // iris

#endif

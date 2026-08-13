#ifndef IRIS_ZZ_STRING_ALGO_HPP
#define IRIS_ZZ_STRING_ALGO_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/string.hpp>

#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>

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

template<class CharT, class Traits>
constexpr void trim_edges(
    std::basic_string<CharT, Traits>& input,
    std::basic_string_view<CharT, Traits> const spaces = detail::string_algo_traits<CharT>::ordinary_spaces
)
{
    auto const first = input.find_first_not_of(spaces);
    if (first == std::basic_string<CharT, Traits>::npos) {
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


template<class CharT, class Traits>
constexpr void normalize_spaces(
    std::basic_string<CharT, Traits>& input,
    std::basic_string_view<CharT, Traits> const space_like_variant_chars = detail::string_algo_traits<CharT>::space_like_variant_chars,
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


template<class CharT, class Traits>
constexpr void compact_spaces(
    std::basic_string<CharT, Traits>& input,
    std::basic_string_view<CharT, Traits> const spaces = detail::string_algo_traits<CharT>::ordinary_spaces,
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

} // iris

#endif

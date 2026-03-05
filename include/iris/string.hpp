#ifndef IRIS_ZZ_STRING_HPP
#define IRIS_ZZ_STRING_HPP

// SPDX-License-Identifier: MIT

#include <concepts>
#include <string>
#include <string_view>

namespace iris {

template<class T>
concept CharLike =
    std::same_as<T, char> ||
    std::same_as<T, char32_t> ||
    std::same_as<T, wchar_t> ||
    std::same_as<T, char8_t> ||
    std::same_as<T, char16_t>;

template<class T>
concept StringLike =
    std::convertible_to<T, std::basic_string_view<char>> ||
    std::convertible_to<T, std::basic_string_view<char32_t>> ||
    std::convertible_to<T, std::basic_string_view<wchar_t>> ||
    std::convertible_to<T, std::basic_string_view<char8_t>> ||
    std::convertible_to<T, std::basic_string_view<char16_t>>;

template<class T>
concept NotStringLike = !StringLike<T>;

} // iris

#endif

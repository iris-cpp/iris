#ifndef IRIS_ZZ_STRING_HPP
#define IRIS_ZZ_STRING_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <concepts>
#include <type_traits>
#include <utility>
#include <string> // IWYU pragma: export
#include <string_view> // IWYU pragma: export

namespace iris {

template<class T>
concept CharLike =
    std::same_as<std::remove_cv_t<T>, char> ||
    std::same_as<std::remove_cv_t<T>, char32_t> ||
    std::same_as<std::remove_cv_t<T>, wchar_t> ||
    std::same_as<std::remove_cv_t<T>, char8_t> ||
    std::same_as<std::remove_cv_t<T>, char16_t>;

template<class T>
concept StringLike =
    std::convertible_to<T, std::basic_string_view<char>> ||
    std::convertible_to<T, std::basic_string_view<char32_t>> ||
    std::convertible_to<T, std::basic_string_view<wchar_t>> ||
    std::convertible_to<T, std::basic_string_view<char8_t>> ||
    std::convertible_to<T, std::basic_string_view<char16_t>>;

template<class T>
concept NotStringLike = !StringLike<T>;


namespace detail {

template<class T>
struct char_type_for_impl
{
    using type = decltype(std::basic_string_view{std::declval<T>()})::value_type;
};

template<CharLike CharT>
struct char_type_for_impl<CharT>
{
    using type = std::remove_cv_t<CharT>;
};

} // detail

template<class T>
using char_type_for = detail::char_type_for_impl<T>::type;

} // iris

#endif

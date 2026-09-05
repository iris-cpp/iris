#ifndef IRIS_ZZ_ERROR_THROWF_FORMAT_HPP
#define IRIS_ZZ_ERROR_THROWF_FORMAT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/error/throwf.hpp> // IWYU pragma: export
#include <iris/string.hpp>

#include <exception> // IWYU pragma: export
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace iris {

namespace detail {

template<class T, class... Args>
concept constructible_from_string_like_types =
    std::is_constructible_v<T, Args..., std::string> ||
    std::is_constructible_v<T, Args..., std::string_view> ||
    std::is_constructible_v<T, Args..., const char*>;

} // detail

inline namespace error_functions {

template<class E, class... Args>
    requires detail::constructible_from_string_like_types<E>
IRIS_CONFIG_THROW_NORETURN void throwf(std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_CONFIG_THROW_IMPL(E{std::format(std::move(fmt), std::forward<Args>(args)...)});
}

template<class E, NotStringLike Arg0, class... Args>
    requires detail::constructible_from_string_like_types<E, Arg0>
IRIS_CONFIG_THROW_NORETURN void throwf(Arg0&& arg0, std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_CONFIG_THROW_IMPL(E{std::forward<Arg0>(arg0), std::format(std::move(fmt), std::forward<Args>(args)...)});
}

template<class E, NotStringLike Arg0, NotStringLike Arg1, class... Args>
    requires detail::constructible_from_string_like_types<E, Arg0, Arg1>
IRIS_CONFIG_THROW_NORETURN void throwf(Arg0&& arg0, Arg1&& arg1, std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_CONFIG_THROW_IMPL(
        E{
            std::forward<Arg0>(arg0), std::forward<Arg1>(arg1),
            std::format(std::move(fmt), std::forward<Args>(args)...)
        }
    );
}

} // error_functions

} // iris

#endif

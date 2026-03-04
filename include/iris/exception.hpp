#ifndef IRIS_ZZ_EXCEPTION_HPP
#define IRIS_ZZ_EXCEPTION_HPP

#include <iris/string.hpp>

#include <exception>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#ifndef IRIS_THROW_IMPL
# define IRIS_THROW_IMPL(...) throw __VA_ARGS__
#endif

#ifndef IRIS_THROW_NORETURN
# define IRIS_THROW_NORETURN [[noreturn]]
#endif

namespace iris {

namespace detail {

template<class T, class... Args>
concept constructible_from_string_like_types =
    std::is_constructible_v<T, Args..., std::string> ||
    std::is_constructible_v<T, Args..., std::string_view> ||
    std::is_constructible_v<T, Args..., const char*>;

} // detail

inline namespace error_functions {

template<class E>
IRIS_THROW_NORETURN void throwf()
{
    static_assert(std::is_base_of_v<std::exception, E>);
    static_assert(std::is_constructible_v<E>);
    IRIS_THROW_IMPL(E{});
}

template<class E, class Arg, class... Rest>
    requires std::is_constructible_v<E, Arg, Rest...>
IRIS_THROW_NORETURN void throwf(Arg&& arg, Rest&&... rest)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    static_assert(!std::is_base_of_v<std::exception, std::remove_cvref_t<Arg>>, "don't copy/move construct exception types directly");
    IRIS_THROW_IMPL(E{std::forward<Arg>(arg), std::forward<Rest>(rest)...});
}

template<class E, class... Args>
    requires detail::constructible_from_string_like_types<E>
IRIS_THROW_NORETURN void throwf(std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_THROW_IMPL(E{std::format(std::move(fmt), std::forward<Args>(args)...)});
}

template<class E, NotStringLike Arg0, class... Args>
    requires detail::constructible_from_string_like_types<E, Arg0>
IRIS_THROW_NORETURN void throwf(Arg0&& arg0, std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_THROW_IMPL(E{std::forward<Arg0>(arg0), std::format(std::move(fmt), std::forward<Args>(args)...)});
}

template<class E, NotStringLike Arg0, NotStringLike Arg1, class... Args>
    requires detail::constructible_from_string_like_types<E, Arg0, Arg1>
IRIS_THROW_NORETURN void throwf(Arg0&& arg0, Arg1&& arg1, std::format_string<Args...> fmt, Args&&... args)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    IRIS_THROW_IMPL(
        E{
            std::forward<Arg0>(arg0), std::forward<Arg1>(arg1),
            std::format(std::move(fmt), std::forward<Args>(args)...)
        }
    );
}

} // error_functions

} // iris

#endif

#ifndef IRIS_ZZ_ERROR_THROWF_HPP
#define IRIS_ZZ_ERROR_THROWF_HPP

#include <iris/config.hpp> // IWYU pragma: keep

#include <exception> // IWYU pragma: export
#include <stdexcept> // IWYU pragma: export
#include <type_traits>
#include <utility>

#ifndef IRIS_CONFIG_THROW_IMPL
# define IRIS_CONFIG_THROW_IMPL(...) throw __VA_ARGS__
#endif

#ifndef IRIS_CONFIG_THROW_NORETURN
# define IRIS_CONFIG_THROW_NORETURN [[noreturn]]
#endif

namespace iris {

inline namespace error_functions {

// This function can be used to strongly assume optimization in some performance-
// critical paths as some compilers fail to optimize the plain `throw` statement
// even though the statement itself should imply `[[noreturn]]`.
template<class E>
IRIS_CONFIG_THROW_NORETURN void throwf()
{
    static_assert(std::is_base_of_v<std::exception, E>);
    static_assert(std::is_constructible_v<E>);
    IRIS_CONFIG_THROW_IMPL(E{});
}

// This function can be used to strongly assume optimization in some performance-
// critical paths as some compilers fail to optimize the plain `throw` statement
// even though the statement itself should imply `[[noreturn]]`.
template<class E, class Arg, class... Rest>
    requires std::is_constructible_v<E, Arg, Rest...>
IRIS_CONFIG_THROW_NORETURN void throwf(Arg&& arg, Rest&&... rest)
{
    static_assert(std::is_base_of_v<std::exception, E>);
    static_assert(!std::is_base_of_v<std::exception, std::remove_cvref_t<Arg>>, "don't copy/move construct exception types directly");
    IRIS_CONFIG_THROW_IMPL(E{std::forward<Arg>(arg), std::forward<Rest>(rest)...});
}

} // error_functions

} // iris

#endif

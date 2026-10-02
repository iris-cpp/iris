#ifndef IRIS_ZZ_RVARIANT_RVARIANT_FWD_HPP
#define IRIS_ZZ_RVARIANT_RVARIANT_FWD_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_list.hpp>

#include <cstddef> // IWYU pragma: keep

namespace iris {

template<class... Ts>
class rvariant;

template<class T>
class recursive_wrapper;

template<class T, class Allocator>
class recursive_wrapper_alloca;

template<class T>
inline constexpr bool is_recursive_wrapper_v = false;

template<class T>
inline constexpr bool is_recursive_wrapper_v<recursive_wrapper<T>> = true;

template<class T, class Allocator>
inline constexpr bool is_recursive_wrapper_v<recursive_wrapper_alloca<T, Allocator>> = true;

template<std::size_t I, class VariantT>
inline constexpr bool is_wrapped_alternative_v = false;

template<std::size_t I, class... Ts>
inline constexpr bool is_wrapped_alternative_v<I, rvariant<Ts...>> = is_recursive_wrapper_v<pack_indexing_t<I, Ts...>>;


namespace detail {

struct valueless_t
{
    constexpr explicit valueless_t() = default;
};

inline constexpr valueless_t valueless{};

} // detail

} // iris

#endif

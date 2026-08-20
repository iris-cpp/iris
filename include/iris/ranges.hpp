#ifndef IRIS_ZZ_RANGES_HPP
#define IRIS_ZZ_RANGES_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <ranges> // IWYU pragma: export
#include <type_traits>
#include <utility>

namespace iris {

template<class T>
struct is_assoc_container : std::false_type
{};

template<class T>
struct is_assoc_container<T const> : is_assoc_container<T>
{};

template<class T>
struct is_assoc_container<T volatile> : is_assoc_container<T>
{};

template<class T>
struct is_assoc_container<T const volatile> : is_assoc_container<T>
{};

template<class T>
constexpr bool is_assoc_container_v = is_assoc_container<T>::value;

template<std::ranges::input_range R>
    requires
        requires {
            typename R::key_type;
            typename R::value_type;
            typename R::mapped_type;
            typename std::tuple_size<typename R::value_type>;
            std::tuple_size<typename R::value_type>::value;
            requires std::tuple_size<typename R::value_type>::value == 2;
        }
struct is_assoc_container<R> : std::true_type
{};

} // iris

#endif

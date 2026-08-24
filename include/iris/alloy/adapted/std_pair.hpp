#ifndef IRIS_ZZ_ALLOY_ADAPTED_STD_PAIR_HPP
#define IRIS_ZZ_ALLOY_ADAPTED_STD_PAIR_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/detail/std_get.hpp>

#include <utility> // IWYU pragma: export

#include <cstddef>

namespace iris::alloy {

template<class T>
struct tuple_size;

template<class T, class U>
struct tuple_size<std::pair<T, U>> : std::integral_constant<std::size_t, 2>
{};

template<class T, class U>
struct tuple_size<std::pair<T, U> const> : std::integral_constant<std::size_t, 2>
{};

template<class T, class U>
struct tuple_size<std::pair<T, U> volatile> : std::integral_constant<std::size_t, 2>
{};

template<class T, class U>
struct tuple_size<std::pair<T, U> const volatile> : std::integral_constant<std::size_t, 2>
{};

template<std::size_t I, class Tuple>
struct tuple_element;

template<std::size_t I, class T, class U>
struct tuple_element<I, std::pair<T, U>> : std::tuple_element<I, std::pair<T, U>>
{};

template<std::size_t I, class T, class U>
struct tuple_element<I, std::pair<T, U> const> : std::tuple_element<I, std::pair<T, U> const>
{};

template<std::size_t I, class T, class U>
struct tuple_element<I, std::pair<T, U> volatile> : std::tuple_element<I, std::pair<T, U> volatile>
{};

template<std::size_t I, class T, class U>
struct tuple_element<I, std::pair<T, U> const volatile> : std::tuple_element<I, std::pair<T, U> const volatile>
{};


template<class T>
struct adaptor;

template<class T, class U>
struct adaptor<std::pair<T, U>>
{
    using getters_list = detail::call_std_get<std::index_sequence<0, 1>>::type;
};

} // iris::alloy

#endif

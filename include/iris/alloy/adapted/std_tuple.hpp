#ifndef IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP
#define IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/detail/std_get.hpp>

#include <tuple> // IWYU pragma: export
#include <utility>

#include <cstddef>

namespace iris::alloy {

template<class T>
struct tuple_size;

template<class... Ts>
struct tuple_size<std::tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<class... Ts>
struct tuple_size<std::tuple<Ts...> const> : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<class... Ts>
struct tuple_size<std::tuple<Ts...> volatile> : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<class... Ts>
struct tuple_size<std::tuple<Ts...> const volatile> : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t I, class Tuple>
struct tuple_element;

template<std::size_t I, class... Ts>
struct tuple_element<I, std::tuple<Ts...>> : std::tuple_element<I, std::tuple<Ts...>>
{};

template<std::size_t I, class... Ts>
struct tuple_element<I, std::tuple<Ts...> const> : std::tuple_element<I, std::tuple<Ts...> const>
{};

template<std::size_t I, class... Ts>
struct tuple_element<I, std::tuple<Ts...> volatile> : std::tuple_element<I, std::tuple<Ts...> volatile>
{};

template<std::size_t I, class... Ts>
struct tuple_element<I, std::tuple<Ts...> const volatile> : std::tuple_element<I, std::tuple<Ts...> const volatile>
{};


template<class T>
struct adaptor;

template<class... Ts>
struct adaptor<std::tuple<Ts...>>
{
    using getters_list = detail::call_std_get<std::index_sequence_for<Ts...>>::type;
};

} // iris::alloy

#endif

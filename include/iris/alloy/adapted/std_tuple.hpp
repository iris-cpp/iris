#ifndef IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP
#define IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP

// SPDX-License-Identifier: MIT

#include <iris/alloy/detail/integer_seq_transform.hpp>

#include <tuple>
#include <utility>

#include <cstddef>

namespace iris::alloy {

template<class T>
struct tuple_size;

template<class... Ts>
struct tuple_size<std::tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
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

namespace detail {

template<std::size_t I>
struct call_std_get
{
    template<class Tuple>
    static constexpr decltype(auto) operator()(Tuple&& t)
    {
        return std::get<I>(static_cast<Tuple&&>(t));
    }
};

template<std::size_t I>
struct make_call_std_get
{
    static constexpr auto value = call_std_get<I>{};
};

} // detail

template<class... Ts>
struct adaptor<std::tuple<Ts...>>
{
    using getters_list = detail::integer_seq_transform<std::make_index_sequence<sizeof...(Ts)>, detail::make_call_std_get>::type;
};

} // iris::alloy

#endif

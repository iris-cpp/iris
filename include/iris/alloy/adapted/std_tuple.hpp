#ifndef IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP
#define IRIS_ZZ_ALLOY_ADAPTED_STD_TUPLE_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <iris/alloy/detail/integer_seq_transform.hpp>

#include <tuple>
#include <utility>

#include <cstddef>

namespace iris::alloy {

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
    using getters_list = detail::integer_seq_transform_t<std::make_index_sequence<sizeof...(Ts)>, detail::make_call_std_get>;
};

} // iris::alloy

#endif

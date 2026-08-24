#ifndef IRIS_ZZ_ALLOY_DETAIL_STD_GET_HPP
#define IRIS_ZZ_ALLOY_DETAIL_STD_GET_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_traits.hpp>

#include <utility>

#include <cstddef> // IWYU pragma: keep

namespace iris::alloy::detail {

template<std::size_t I>
struct call_std_get_impl
{
    template<class Tuple>
    [[nodiscard]] static constexpr decltype(auto) operator()(Tuple&& t) noexcept
    {
        return std::get<I>(static_cast<Tuple&&>(t));
    }
};

template<class Seq>
struct call_std_get;

template<std::size_t... Is>
struct call_std_get<std::index_sequence<Is...>>
{
    using type = constant_list<call_std_get_impl<Is>{}...>;
};

} // iris::alloy::detail

#endif

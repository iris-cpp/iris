#ifndef IRIS_ZZ_ALLOY_IO_HPP
#define IRIS_ZZ_ALLOY_IO_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <iris/io_fwd.hpp>

#include <iris/alloy/tuple.hpp>

#include <ostream>
#include <utility>

#include <cstddef>

namespace iris::alloy {

namespace detail {

template<class IndexSeq>
struct tuple_ostream_impl;

template<std::size_t... Is>
struct tuple_ostream_impl<std::index_sequence<Is...>>
{
    template<class... Ts>
    static constexpr std::ostream& apply(std::ostream& os, tuple<Ts...> const& t)
    {
        os << '(';
        [[maybe_unused]] bool first = true;
        ((std::exchange(first, false) ? os << alloy::get<Is>(t) : os << ", " << alloy::get<Is>(t)), ...);
        os << ')';
        return os;
    }
};

} // detail

template<class... Ts>
    requires std::conjunction_v<req::ADL_ostreamable<Ts>...>
std::ostream& operator<<(std::ostream& os, tuple<Ts...> const& t)
{
    return detail::tuple_ostream_impl<std::make_index_sequence<sizeof...(Ts)>>::apply(os, t);
}

} // iris::alloy

#endif

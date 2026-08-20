#ifndef IRIS_ZZ_ALLOY_ADAPTED_STD_PAIR_HPP
#define IRIS_ZZ_ALLOY_ADAPTED_STD_PAIR_HPP

// SPDX-License-Identifier: MIT

#include <utility>

namespace iris::alloy {

namespace detail {

template<auto... Vs>
struct non_type_list;

} // detail

template<class T>
struct adaptor;

template<class T, class U>
struct adaptor<std::pair<T, U>>
{
    using getters_list = detail::non_type_list<&std::pair<T, U>::first, &std::pair<T, U>::second>;
};

} // iris::alloy

#endif

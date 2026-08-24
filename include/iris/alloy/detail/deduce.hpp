#ifndef IRIS_ZZ_ALLOY_DETAIL_DEDUCE_HPP
#define IRIS_ZZ_ALLOY_DETAIL_DEDUCE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <type_traits>

namespace iris::alloy::detail {

template<class FromLValue, class FromXValue>
struct deduce
{
    static_assert(std::conjunction_v<
        std::is_reference<FromLValue>,
        std::is_reference<FromXValue>,
        std::is_same<std::remove_reference_t<FromLValue>, std::remove_reference_t<FromXValue>>
    >);
};

template<class T>
struct deduce<T&, T&>
{
    using type = T&;
};

template<class T>
struct deduce<T&, T&&>
{
    using type = T;
};

template<class T>
struct deduce<T&&, T&&>
{
    using type = T;
};

template<class FromLValue, class FromXValue>
using deduce_t = deduce<FromLValue, FromXValue>::type;

} // iris::alloy::detail

#endif

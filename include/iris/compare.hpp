#ifndef IRIS_COMPARE_HPP
#define IRIS_COMPARE_HPP

// SPDX-License-Identifier: MIT

#include <iris/requirements.hpp>

#include <compare>
#include <functional>
#include <type_traits>

namespace iris {

// Utilities defined in [library]
// https://eel.is/c++draft/library

constexpr auto synth_three_way = []<class T, class U>(T const& t, U const& u)
    requires requires {
        { t < u } -> req::boolean_testable;
        { u < t } -> req::boolean_testable;
    }
{
    if constexpr (std::three_way_comparable_with<T, U>) {
    //if constexpr (requires { t <=> u; }) {
        return t <=> u;
    } else {
        if (t < u) return std::weak_ordering::less;
        if (u < t) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;
    }
};

template<class T, class U = T>
using synth_three_way_result_t = decltype(synth_three_way(std::declval<T&>(), std::declval<U&>()));


namespace cmp {

template<class Compare, class T>
struct relop_bool_expr : std::false_type {};

template<class Compare, class T>
constexpr bool relop_bool_expr_v = relop_bool_expr<Compare, T>::value;

// NOTE: boolean_testable is WRONG here; it leads to false positive

template<class T>
    requires requires(T const& t) { { t == t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::equal_to<>, T> : std::true_type {};

template<class T>
    requires requires(T const& t) { { t != t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::not_equal_to<>, T> : std::true_type {};

template<class T>
    requires requires(T const& t) { { t < t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::less<>, T> : std::true_type {};

template<class T>
    requires requires(T const& t) { { t > t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::greater<>, T> : std::true_type {};

template<class T>
    requires requires(T const& t) { { t <= t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::less_equal<>, T> : std::true_type {};

template<class T>
    requires requires(T const& t) { { t >= t } -> std::convertible_to<bool>; }
struct relop_bool_expr<std::greater_equal<>, T> : std::true_type {};

}  // cmp

}  // iris

#endif

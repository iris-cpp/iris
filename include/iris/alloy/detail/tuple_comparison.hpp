#ifndef IRIS_ZZ_ALLOY_DETAIL_TUPLE_COMPARISON_HPP
#define IRIS_ZZ_ALLOY_DETAIL_TUPLE_COMPARISON_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <iris/requirements.hpp>

#include <concepts>
#include <type_traits>

namespace iris::alloy {

template<class... Ts>
class tuple;

namespace detail {

namespace equality_operator_poison_barrier {

bool operator==(auto, auto) = delete;  // poison-pill

template<class T, class U>
concept has_equality_operator = requires(T&& x, U&& y) {
    { static_cast<T&&>(x) == static_cast<U&&>(y) } -> req::boolean_testable;
};

template<class T, class U>
struct is_nothrow_equality_comparable : std::bool_constant<has_equality_operator<T, U> && noexcept(std::declval<T>() == std::declval<U>())> {};

}  // equality_operator_poison_barrier

using equality_operator_poison_barrier::has_equality_operator;
using equality_operator_poison_barrier::is_nothrow_equality_comparable;

template<class TTuple, class UTuple>
struct do_tuple_all_elements_have_equality_operator {};

template<class... Ts, class... Us>
struct do_tuple_all_elements_have_equality_operator<tuple<Ts...>, tuple<Us...>>
    : std::bool_constant<(has_equality_operator<Ts const&, Us const&> && ...)> {};

template<class TTuple, class UTuple>
inline constexpr bool do_tuple_all_elements_have_equality_operator_v = do_tuple_all_elements_have_equality_operator<TTuple, UTuple>::value;

template<class TTuple, class UTuple>
concept tuple_all_elements_have_equality_operator = do_tuple_all_elements_have_equality_operator_v<TTuple, UTuple>;

template<class TTuple, class UTuple>
struct are_tuple_all_elements_nothrow_equality_comparable {};

template<class... Ts, class... Us>
struct are_tuple_all_elements_nothrow_equality_comparable<tuple<Ts...>, tuple<Us...>>
    : std::conjunction<is_nothrow_equality_comparable<Ts const&, Us const&>...> {};

template<class TTuple, class UTuple>
inline constexpr bool are_tuple_all_elements_nothrow_equality_comparable_v = are_tuple_all_elements_nothrow_equality_comparable<TTuple, UTuple>::value;

} // detail

} // iris::alloy

#endif

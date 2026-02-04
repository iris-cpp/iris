#ifndef IRIS_TYPE_TRAITS_HPP
#define IRIS_TYPE_TRAITS_HPP

#include <iris/config.hpp>

#include <type_traits>

namespace iris {

template<class T, template<class...> class TT>
struct is_ttp_specialization_of : std::false_type {};

template<template<class...> class TT, class... Ts>
struct is_ttp_specialization_of<TT<Ts...>, TT> : std::true_type {};

template<class T, template<class...> class TT>
inline constexpr bool is_ttp_specialization_of_v = is_ttp_specialization_of<T, TT>::value;


template<class T, template<auto...> class TT>
struct is_nttp_specialization_of : std::false_type {};

template<template<auto...> class TT, auto... Ts>
struct is_nttp_specialization_of<TT<Ts...>, TT> : std::true_type {};

template<class T, template<auto...> class TT>
inline constexpr bool is_nttp_specialization_of_v = is_nttp_specialization_of<T, TT>::value;

} // iris

#endif

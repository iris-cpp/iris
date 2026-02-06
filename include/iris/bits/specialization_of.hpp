#ifndef IRIS_BITS_SPECIALIZATION_OF
#define IRIS_BITS_SPECIALIZATION_OF

#include <type_traits>

namespace iris {

// https://eel.is/c++draft/temp#param-2

template<class T, template<class...> class TT>
struct is_ttp_specialization_of : std::false_type {};

template<template<class...> class TT, class... Ts>
struct is_ttp_specialization_of<TT<Ts...>, TT> : std::true_type {};

template<class T, template<class...> class TT>
inline constexpr bool is_ttp_specialization_of_v = is_ttp_specialization_of<T, TT>::value;


template<class T, template<auto...> class TT>
struct is_ctp_specialization_of : std::false_type {};

template<template<auto...> class TT, auto... Cs>
struct is_ctp_specialization_of<TT<Cs...>, TT> : std::true_type {};

template<class T, template<auto...> class TT>
inline constexpr bool is_ctp_specialization_of_v = is_ctp_specialization_of<T, TT>::value;

}  // iris

#endif

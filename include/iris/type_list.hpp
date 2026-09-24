#ifndef IRIS_ZZ_TYPE_LIST_HPP
#define IRIS_ZZ_TYPE_LIST_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <type_traits>
#include <utility>

#include <cstddef> // IWYU pragma: keep

namespace iris {

template<class... Ts>
struct type_list
{
    static constexpr std::size_t size = sizeof...(Ts);
};

template<auto... Vals>
struct constant_list
{
    static constexpr std::size_t size = sizeof...(Vals);
};

// ----------------------------------------------------------

template<class... Lists>
struct concat_type_list;

// `concat_type_list_t` is intentionally not defined; typical use case
// of concatenating type lists involves deep recursive instantiation of
// parent templates, which sometimes makes IDE's completion choke on
// single additional type alias of `_t`. Just use `typename ...::type`.

template<>
struct concat_type_list<>
{
    using type = type_list<>;
};

template<class... Ts>
struct concat_type_list<type_list<Ts...>>
{
    using type = type_list<Ts...>;
};

template<class... Ts, class... Us>
struct concat_type_list<type_list<Ts...>, type_list<Us...>>
{
    using type = type_list<Ts..., Us...>;
};

template<class... Ts, class... Us, class... Rest>
struct concat_type_list<type_list<Ts...>, type_list<Us...>, Rest...>
    : concat_type_list<type_list<Ts..., Us...>, Rest...>
{};

// ----------------------------------------------------------

namespace detail {

template<class... Ts>
struct identity_bases : std::type_identity<Ts>...
{
    template<template<class...> class TT>
    using rebind = TT<Ts...>;
};

template<class T, class BasesT>
concept identity_base_of = requires(BasesT* bases) {
    static_cast<std::type_identity<T>*>(bases);
};

template<class BasesT, class... Ts>
struct unique_type_list_impl;

template<class BasesT>
struct unique_type_list_impl<BasesT>
{
    using type = BasesT;
};

template<class BasesT, class T, class... Rest>
    requires identity_base_of<T, BasesT>
struct unique_type_list_impl<BasesT, T, Rest...>
    : unique_type_list_impl<BasesT, Rest...>
{};

template<class... AcceptedTs, class T, class... Rest>
    requires (!identity_base_of<T, identity_bases<AcceptedTs...>>)
struct unique_type_list_impl<identity_bases<AcceptedTs...>, T, Rest...>
    : unique_type_list_impl<identity_bases<AcceptedTs..., T>, Rest...>
{};

} // detail

template<class List = void>
struct unique_type_list;

template<>
struct unique_type_list<void>
{
    using type = type_list<>;
};

template<class... Ts>
struct unique_type_list<type_list<Ts...>>
{
    using type = detail::unique_type_list_impl<
        detail::identity_bases<>, Ts...
    >::type::template rebind<type_list>;
};

// ----------------------------------------------------------

namespace detail {

template<class Voids>
struct do_pack_indexing;

template<std::size_t... Voids>
struct do_pack_indexing<std::index_sequence<Voids...>>
{
    template<class T>
    static std::type_identity<T> select(
        // ReSharper disable once CppCStyleCast
        decltype((void*)Voids)...,
        std::type_identity<T>*,
        ...
    );
};

template<class Voids>
struct do_cpack_indexing;

template<std::size_t... Voids>
struct do_cpack_indexing<std::index_sequence<Voids...>>
{
    template<class T, T N>
    static std::integral_constant<T, N> select(
        // ReSharper disable once CppCStyleCast
        decltype((void*)Voids)...,
        std::integral_constant<T, N>*,
        ...
    );
};

} // detail


template<std::size_t I, class T>
struct at_c;

template<std::size_t I, class T>
using at_c_t = at_c<I, T>::type;

// Has native pack indexing?
// Note: GCC 15 emits "sorry, unimplemented: mangling type pack index"
#if __cpp_pack_indexing >= 202311L && \
    !(defined(__GNUC__) && !defined(__clang__) && __GNUC__ <= 15) && \
    !defined(__clang__)

# define IRIS_PACK_INDEXING(I, Ts_ellipsis) Ts_ellipsis[I]

template<std::size_t I, class... Ts> struct pack_indexing { using type = Ts...[I]; };
template<std::size_t I, class... Ts> using pack_indexing_t = Ts...[I];
template<std::size_t I, auto... Ns> struct cpack_indexing { static constexpr auto value = Ns...[I]; };
template<std::size_t I, auto... Ns> constexpr auto cpack_indexing_v = Ns...[I];

template<std::size_t I, template<class...> class TT, class... Ts>
struct at_c<I, TT<Ts...>>
{
    using type = Ts...[I];
};

// ----------------------------------------------------------
#else // no native pack indexing
template<std::size_t I, class... Ts>
struct pack_indexing
{
    static_assert(I < sizeof...(Ts));
    using type = decltype(detail::do_pack_indexing<std::make_index_sequence<I>>::select(
        static_cast<std::type_identity<Ts>*>(nullptr)...
    ))::type;
};

template<std::size_t I, class... Ts>
using pack_indexing_t = pack_indexing<I, Ts...>::type;

# define IRIS_PACK_INDEXING(I, Ts_ellipsis) ::iris::pack_indexing_t<I, Ts_ellipsis>


template<std::size_t I, auto... Ns>
struct cpack_indexing
{
    static_assert(I < sizeof...(Ns));
    static constexpr auto value = decltype(detail::do_cpack_indexing<std::make_index_sequence<I>>::select(
        static_cast<std::integral_constant<decltype(Ns), Ns>*>(nullptr)...
    ))::value;
};

template<std::size_t I, auto... Ns>
constexpr auto cpack_indexing_v = cpack_indexing<I, Ns...>::value;


template<std::size_t I, template<class...> class TT, class... Ts>
struct at_c<I, TT<Ts...>>
{
    static_assert(I < sizeof...(Ts));
    using type = decltype(detail::do_pack_indexing<std::make_index_sequence<I>>::select(
        static_cast<std::type_identity<Ts>*>(nullptr)...
    ))::type;
};
#endif

// ----------------------------------------------------------

inline constexpr std::size_t find_npos = static_cast<std::size_t>(-1);

namespace detail {

template<std::size_t I, class T, class... Ts>
struct find_index_impl
    : std::integral_constant<std::size_t, find_npos>
{};

template<std::size_t I, class T, class U, class... Us>
struct find_index_impl<I, T, U, Us...>
    : std::conditional_t<
        std::is_same_v<T, U>,
        std::integral_constant<std::size_t, I>,
        find_index_impl<I + 1, T, Us...>
    >
{};

} // detail

template<class T, class List>
struct find_index;

template<class T, template<class...> class TT, class... Ts>
struct find_index<T, TT<Ts...>> : detail::find_index_impl<0, T, Ts...> {};

template<class T, class List>
inline constexpr std::size_t find_index_v = find_index<T, List>::value;


template<class T, class... Ts>
struct is_in : std::disjunction<std::is_same<T, Ts>...> {};

template<class T, class... Ts>
inline constexpr bool is_in_v = is_in<T, Ts...>::value;

// ----------------------------------------------------------

namespace detail {

template<bool Found, class T, class... Us>
struct exactly_once_impl : std::bool_constant<Found> {};

template<class T, class U, class... Us>
struct exactly_once_impl<false, T, U, Us...>
    : exactly_once_impl<std::is_same_v<T, U>, T, Us...> {};

template<class T, class U, class... Us>
struct exactly_once_impl<true, T, U, Us...>
    : std::conditional_t<std::is_same_v<T, U>, std::false_type, exactly_once_impl<true, T, Us...>> {};

} // detail

template<class T, class List>
struct exactly_once;

template<class T, template<class...> class TT, class... Ts>
struct exactly_once<T, TT<Ts...>> : detail::exactly_once_impl<false, T, Ts...>
{
    static_assert(sizeof...(Ts) > 0);
};

template<class T, class List>
inline constexpr bool exactly_once_v = exactly_once<T, List>::value;

} // iris

#endif

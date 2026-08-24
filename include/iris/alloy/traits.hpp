#ifndef IRIS_ZZ_ALLOY_COMMON_DEF_HPP
#define IRIS_ZZ_ALLOY_COMMON_DEF_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/detail/deduce.hpp>

#include <iris/type_traits.hpp>

#include <functional>
#include <type_traits>

#include <cstddef>

namespace iris::alloy {

template<class T>
struct adaptor;

struct value_initialize_t {};

inline constexpr value_initialize_t value_initialize{};

template<class... Ts>
class tuple;

namespace detail {

template<class T>
concept PureAdapted = requires { typename adaptor<T>::getters_list; };

template<class T>
concept PureTupleLike = is_ttp_specialization_of_v<T, tuple> || PureAdapted<T>;

} // detail

template<class T>
struct is_tuple_like : std::bool_constant<detail::PureTupleLike<T>> {};

template<class T>
inline constexpr bool is_tuple_like_v = is_tuple_like<T>::value;

template<class T>
concept Adapted = detail::PureAdapted<std::remove_cvref_t<T>>;

template<class T>
concept TupleLike = detail::PureTupleLike<std::remove_cvref_t<T>>;

template<class T>
struct tuple_size {};

template<class T>
struct tuple_size<T const> : tuple_size<T> {};

template<class T>
struct tuple_size<T volatile> : tuple_size<T> {};

template<class T>
struct tuple_size<T const volatile> : tuple_size<T> {};

template<class... Ts>
struct tuple_size<tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<Adapted T>
struct tuple_size<T> : std::integral_constant<std::size_t, adaptor<std::remove_cvref_t<T>>::getters_list::size> {};

template<class T>
inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

template<std::size_t I, class Tuple>
struct tuple_element {};

template<std::size_t I, class... Ts>
struct tuple_element<I, tuple<Ts...>>
{
    using type = IRIS_CORE_PACK_INDEXING(I, Ts...);
};

template<std::size_t I, class... Ts>
struct tuple_element<I, tuple<Ts...> const>
{
    using type = tuple_element<I, tuple<Ts...>>::type const;
};

template<std::size_t I, class... Ts>
struct tuple_element<I, tuple<Ts...> volatile>
{
    using type = tuple_element<I, tuple<Ts...>>::type volatile;
};

template<std::size_t I, class... Ts>
struct tuple_element<I, tuple<Ts...> const volatile>
{
    using type = tuple_element<I, tuple<Ts...>>::type const volatile;
};


template<std::size_t I, class Tuple>
using tuple_element_t = tuple_element<I, Tuple>::type;

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t) noexcept;

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>> const& get(tuple<Ts...> const& t) noexcept;

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t) noexcept;

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>> const&& get(tuple<Ts...> const&& t) noexcept;

namespace detail {

template<Adapted T, std::size_t I, auto... Getters>
struct adaptor_getter_t_impl;

template<Adapted T, auto Getter, auto... Getters>
struct adaptor_getter_t_impl<T, 0, Getter, Getters...>
{
    static constexpr auto getter = Getter;
    using getter_type = decltype(Getter);
    using type = decltype(std::invoke(Getter, std::declval<T>()));
};

template<Adapted T, std::size_t I, auto Getter, auto... Getters>
struct adaptor_getter_t_impl<T, I, Getter, Getters...>
    : adaptor_getter_t_impl<T, I - 1, Getters...>
{};

template<Adapted T, std::size_t I, class GettersListT>
struct adaptor_getter_t_dispatch;

template<Adapted T, std::size_t I, auto... Getters>
struct adaptor_getter_t_dispatch<T, I, constant_list<Getters...>>
{
    using type = adaptor_getter_t_impl<T, I, Getters...>;
};

template<std::size_t I, Adapted T>
using adaptor_getter_t = adaptor_getter_t_dispatch<T, I, typename adaptor<std::remove_cvref_t<T>>::getters_list>::type;

} // detail

template<std::size_t I, Adapted T>
[[nodiscard]] constexpr detail::adaptor_getter_t<I, T>::type
get(T&& x)
    noexcept(std::is_nothrow_invocable_v<typename detail::adaptor_getter_t<I, T>::getter_type, T>)
{
    return std::invoke(detail::adaptor_getter_t<I, T>::getter, std::forward<T>(x));
}

namespace detail {

template<std::size_t I, class T>
using tuple_get_t = decltype(alloy::get<I>(std::declval<T>()));

template<std::size_t I, class T>
struct is_nothrow_gettable : std::bool_constant<noexcept(alloy::get<I>(std::declval<T>()))> {};

template<std::size_t I, class T>
inline constexpr bool is_nothrow_gettable_v = is_nothrow_gettable<I, T>::value;

} // detail

template<std::size_t I, Adapted T>
struct tuple_element<I, T>
{
    // Since we only have access through getters, we don't know exact types of user-defined tuple-like types' elements.
    // Threrefore, we deduce the types from what we get from getters.
    using type = detail::deduce_t<detail::tuple_get_t<I, std::remove_cvref_t<T>&>&&, detail::tuple_get_t<I, std::remove_cvref_t<T>&&>&&>;
};

namespace detail {

template<class T, class IndexSeq>
struct is_view_impl {};

template<class T, std::size_t... Is>
struct is_view_impl<T, std::index_sequence<Is...>> : std::conjunction<std::is_lvalue_reference<tuple_element_t<Is, T>>...> {};

template<class T>
struct is_view : std::false_type {};

template<class T>
    requires is_tuple_like_v<T>
struct is_view<T> : is_view_impl<T, std::make_index_sequence<tuple_size_v<T>>> {};

} // detail

template<class T>
struct is_tuple_like_view : std::conjunction<is_tuple_like<T>, detail::is_view<T>> {};

template<class T>
concept TupleLikeView = TupleLike<T> && detail::is_view<std::remove_cvref_t<T>>::value;

template<class T>
inline constexpr bool is_tuple_like_view_v = is_tuple_like_view<T>::value;

namespace detail {

template<class TTuple, class UTuple, template<class> class TQual, template<class> class UQual, class IndexSeq>
struct basic_common_reference_impl;

template<class TTuple, class UTuple, template<class> class TQual, template<class> class UQual, std::size_t... Is>
struct basic_common_reference_impl<TTuple, UTuple, TQual, UQual, std::index_sequence<Is...>>
{
    using type = tuple<std::common_reference_t<TQual<tuple_element_t<Is, TTuple>>, UQual<tuple_element_t<Is, UTuple>>>...>;
};

} // detail

} // iris::alloy

// Note: We can't directly specify the concept `TupleLike` in the
// declaration of the template parameter because it would invoke
// the `TupleLike` check for virtually ANY types whenever the
// *primary* template of `std::basic_common_reference` is instantiated.
//
// Doing so would produce some hard errors on completely irrelevant
// context; for example, calling `std::map<T, U>{}.rbegin()` would
// inevitably *check* `TupleLike` for `std::pair<T const, U>` thus
// leads to instantiation of `alloy::get`. Then if the instantiation
// yields hard error for some reason, the error is propagated to the
// caller in SFINAE-unfriendly context.
template<class TTuple, class UTuple, template<class> class TQual, template<class> class UQual>
    requires
        (
            iris::is_ttp_specialization_of_v<TTuple, iris::alloy::tuple> ||
            iris::is_ttp_specialization_of_v<UTuple, iris::alloy::tuple>
        ) &&
        iris::alloy::TupleLike<TTuple> && iris::alloy::TupleLike<UTuple> &&
        (iris::alloy::tuple_size_v<TTuple> == iris::alloy::tuple_size_v<UTuple>)
struct std::basic_common_reference<TTuple, UTuple, TQual, UQual>
    : iris::alloy::detail::basic_common_reference_impl<
        TTuple, UTuple, TQual, UQual,
        std::make_index_sequence<iris::alloy::tuple_size_v<TTuple>>
    >
{};

#endif

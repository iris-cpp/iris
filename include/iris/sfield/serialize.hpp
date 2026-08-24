#ifndef IRIS_ZZ_SFIELD_SERIALIZE_HPP
#define IRIS_ZZ_SFIELD_SERIALIZE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/traits.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/adapted/std_pair.hpp>

#include <iris/string.hpp>
#include <iris/ranges.hpp>

#include <iris/bits/specialization_of.hpp>

#include <concepts>
#include <type_traits>
#include <optional>

namespace iris::sfield {

namespace detail {

template<class ClassT>
struct adapted_class;

template<class T>
struct is_adapted_class : std::false_type
{};

template<class T>
    requires requires {
        typename adapted_class<T>;
        adapted_class<T>::fields;
    }
struct is_adapted_class<T> : std::true_type
{};


template<class T>
struct is_serializable_primitive : std::false_type
{};

template<class T>
struct is_serializable : is_serializable_primitive<T>
{};

template<class T>
struct is_serializable_primitive<T const> : is_serializable_primitive<T>
{};

template<class T>
    requires
        std::is_arithmetic_v<T> ||
        std::is_enum_v<T> ||
        StringLike<T>
struct is_serializable_primitive<T> : std::true_type
{};

template<class T>
    requires
        is_ttp_specialization_of_v<T, std::optional>
struct is_serializable_primitive<T> : is_serializable<typename T::value_type>
{};

// ---------------------------------------------------

template<std::ranges::forward_range R>
    requires (!StringLike<R>)
struct is_serializable<R> : is_serializable<std::ranges::range_value_t<R>>
{};

} // detail

// T is a type that is directly representable with JSON's native type
template<class T>
concept serializable_primitive = detail::is_serializable_primitive<T>::value;


template<class T>
concept serializable_array =
    std::ranges::forward_range<T> &&
    !is_assoc_container_v<T> &&
    !StringLike<T> &&
    detail::is_serializable<std::ranges::range_value_t<T>>::value;

template<class T>
concept serializable_map =
    std::ranges::forward_range<T> &&
    is_assoc_container_v<T> &&
    detail::is_serializable<std::ranges::range_value_t<T>>::value;


namespace detail {

template<class Tup, class Seq = void>
struct serializable_tuple_impl : std::false_type
{};

template<class Tup>
struct serializable_tuple_impl<Tup, void>
    : serializable_tuple_impl<Tup, std::make_index_sequence<alloy::tuple_size_v<Tup>>>
{};

template<class Tup, std::size_t... Is>
struct serializable_tuple_impl<Tup, std::index_sequence<Is...>>
    : std::conjunction<
        is_serializable<typename alloy::tuple_element<Is, Tup>::type>...
    >
{};

template<alloy::TupleLike Tup>
struct is_serializable<Tup> : serializable_tuple_impl<Tup, std::make_index_sequence<alloy::tuple_size<Tup>::value>>
{};

} // detail

template<class T>
concept serializable_tuple =
    alloy::TupleLike<T> &&
    detail::serializable_tuple_impl<T>::value &&
    !detail::is_adapted_class<T>::value;

template<class T>
concept serializable_class =
    detail::is_adapted_class<T>::value;

template<class T>
concept serializable =
    serializable_primitive<T> ||
    serializable_array<T> ||
    serializable_tuple<T> ||
    serializable_class<T>;

} // iris::sfield

#endif

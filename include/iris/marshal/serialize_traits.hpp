#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_TRAITS_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_TRAITS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/traits.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/adapted/std_pair.hpp>

#include <iris/string.hpp>
#include <iris/ranges.hpp>

#include <iris/bits/specialization_of.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <optional>

#include <cstddef>

namespace iris::marshal {

// Customization point
template<class T>
struct adapted_optional_traits;

template<class T>
    requires is_ttp_specialization_of_v<T, std::optional>
struct adapted_optional_traits<T>
{
    using value_type = T::value_type;
};

template<class T>
concept adapted_optional = requires(T const& opt) {
    typename adapted_optional_traits<std::remove_cvref_t<T>>::value_type;
    static_cast<bool>(opt);
    { *opt } -> std::convertible_to<typename adapted_optional_traits<std::remove_cvref_t<T>>::value_type const&>;
};

// Customization point
template<class ClassT>
struct adapted_class_traits;

template<class T>
concept adapted_class = requires {
    adapted_class_traits<std::remove_cvref_t<T>>::fields;
};

namespace detail {

template<class T>
[[nodiscard]] consteval bool is_serializable_impl();

template<class T>
struct is_serializable_scalar : std::false_type
{};

template<class T>
    requires
        std::is_arithmetic_v<T> ||
        std::is_enum_v<T> ||
        StringLike<T>
struct is_serializable_scalar<T> : std::true_type
{};

// ---------------------------------------------------

template<class Tup, std::size_t I>
[[nodiscard]] consteval bool is_tuple_elements_serializable_impl();

template<class T>
consteval bool is_serializable_impl()
{
    using V = std::remove_cvref_t<T>;

    if constexpr (adapted_class<V>) {
        return true;

    } else if constexpr (adapted_optional<V>) {
        return is_serializable_impl<typename adapted_optional_traits<V>::value_type>();

    } else if constexpr (is_serializable_scalar<V>::value) {
        return true;

    } else if constexpr (ranges::key_value_range<V>) {
        return is_serializable_impl<ranges::range_key_t<V>>() &&
               is_serializable_impl<ranges::range_mapped_t<V>>();

    } else if constexpr (std::ranges::input_range<V>) {
        return is_serializable_impl<std::ranges::range_value_t<V>>();

    } else if constexpr (alloy::TupleLike<V>) {
        return is_tuple_elements_serializable_impl<V>();

    } else {
        return false;
    }
}

template<class Tup, std::size_t I = 0>
consteval bool is_tuple_elements_serializable_impl()
{
    if constexpr (I == alloy::tuple_size_v<Tup>) {
        return true;

    } else if constexpr (!is_serializable_impl<typename alloy::tuple_element<I, Tup>::type>()) {
        return false;

    } else {
        return is_tuple_elements_serializable_impl<Tup, I + 1>();
    }
}

} // detail


template<class T>
concept serializable_class =
    adapted_class<T>;

template<class T>
concept serializable_optional =
    !serializable_class<T> &&
    adapted_optional<T> &&
    detail::is_serializable_impl<typename adapted_optional_traits<std::remove_cvref_t<T>>::value_type>();

template<class T>
concept serializable_scalar =
    !serializable_class<T> &&
    !adapted_optional<T> &&
    detail::is_serializable_scalar<std::remove_cvref_t<T>>::value;

template<class T>
concept serializable_map =
    !serializable_class<T> &&
    !adapted_optional<T> &&
    !serializable_scalar<T> &&
    ranges::key_value_range<T> &&
    detail::is_serializable_impl<ranges::range_key_t<T>>() &&
    detail::is_serializable_impl<ranges::range_mapped_t<T>>();

template<class T>
concept serializable_array =
    !serializable_class<T> &&
    !adapted_optional<T> &&
    !serializable_scalar<T> &&
    !ranges::key_value_range<T> &&
    std::ranges::input_range<T> &&
    detail::is_serializable_impl<std::ranges::range_value_t<T>>();

template<class T>
concept serializable_tuple =
    !serializable_class<T> &&
    !adapted_optional<T> &&
    !serializable_scalar<T> &&
    !std::ranges::range<T> &&
    alloy::TupleLike<T> &&
    detail::is_tuple_elements_serializable_impl<std::remove_cvref_t<T>>();

template<class T>
concept serializable =
    serializable_class<T> ||
    serializable_optional<T> ||
    serializable_scalar<T> ||
    serializable_map<T> ||
    serializable_array<T> ||
    serializable_tuple<T>;

} // iris::marshal

#endif

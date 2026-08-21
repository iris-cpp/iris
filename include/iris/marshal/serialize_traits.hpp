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

struct generic_format
{
    template<class T>
    static constexpr bool key = true;
};

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

template<class T, class Format>
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

template<class Tup, class Format, std::size_t I = 0>
[[nodiscard]] consteval bool is_tuple_elements_serializable_impl();

template<class T, class Format>
consteval bool is_serializable_impl()
{
    using V = std::remove_cvref_t<T>;

    if constexpr (adapted_class<V>) {
        return true;

    } else if constexpr (adapted_optional<V>) {
        return is_serializable_impl<typename adapted_optional_traits<V>::value_type, Format>();

    } else if constexpr (is_serializable_scalar<V>::value) {
        return true;

    } else if constexpr (ranges::key_value_range<V>) {
        return Format::template key<ranges::range_key_t<V>> &&
               is_serializable_impl<ranges::range_key_t<V>, Format>() &&
               is_serializable_impl<ranges::range_mapped_t<V>, Format>();

    } else if constexpr (std::ranges::input_range<V>) {
        return is_serializable_impl<std::ranges::range_value_t<V>, Format>();

    } else if constexpr (alloy::TupleLike<V>) {
        return is_tuple_elements_serializable_impl<V, Format>();

    } else {
        return false;
    }
}

template<class Tup, class Format, std::size_t I>
consteval bool is_tuple_elements_serializable_impl()
{
    if constexpr (I == alloy::tuple_size_v<Tup>) {
        return true;

    } else if constexpr (!is_serializable_impl<typename alloy::tuple_element<I, Tup>::type, Format>()) {
        return false;

    } else {
        return is_tuple_elements_serializable_impl<Tup, Format, I + 1>();
    }
}

} // detail


template<class T, class Format = generic_format>
concept serializable_class =
    adapted_class<T>;

template<class T, class Format = generic_format>
concept serializable_optional =
    !serializable_class<T, Format> &&
    adapted_optional<T> &&
    detail::is_serializable_impl<typename adapted_optional_traits<std::remove_cvref_t<T>>::value_type, Format>();

template<class T, class Format = generic_format>
concept serializable_scalar =
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    detail::is_serializable_scalar<std::remove_cvref_t<T>>::value;

template<class T, class Format = generic_format>
concept serializable_map =
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    ranges::key_value_range<T> &&
    Format::template key<ranges::range_key_t<T>> &&
    detail::is_serializable_impl<ranges::range_key_t<T>, Format>() &&
    detail::is_serializable_impl<ranges::range_mapped_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable_array =
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    !ranges::key_value_range<T> &&
    std::ranges::input_range<T> &&
    detail::is_serializable_impl<std::ranges::range_value_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable_tuple =
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    !std::ranges::range<T> &&
    alloy::TupleLike<T> &&
    detail::is_tuple_elements_serializable_impl<std::remove_cvref_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable =
    serializable_class<T, Format> ||
    serializable_optional<T, Format> ||
    serializable_scalar<T, Format> ||
    serializable_map<T, Format> ||
    serializable_array<T, Format> ||
    serializable_tuple<T, Format>;

} // iris::marshal

#endif

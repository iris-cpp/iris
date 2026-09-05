#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_TRAITS_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_TRAITS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/traits.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/adapted/std_pair.hpp>

#include <iris/string.hpp>
#include <iris/ranges.hpp>
#include <iris/container_traits.hpp>

#include <iris/bits/specialization_of.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <optional>

#include <cstddef>

namespace iris::marshal {

struct generic_format
{
    // Checks whether the *exact* type `K` is eligible for the key of this format's "map" type
    template<class K>
    static constexpr bool map_key = true;

    // Checks whether `K` can be converted to the key of this format's "map" type
    template<class K>
    static constexpr bool loadable_key = true;

    template<class T>
    static constexpr bool loadable_scalar =
        std::is_arithmetic_v<T> ||
        std::is_enum_v<T> ||
        StringLike<T>;
};

namespace detail {

template<class Format>
concept marshal_format = requires {
    { Format::template map_key<int> } -> std::convertible_to<bool>;
    { Format::template loadable_key<int> } -> std::convertible_to<bool>;
    { Format::template loadable_scalar<int> } -> std::convertible_to<bool>;
};

} // detail

// Customization point: serialize/deserialize `T` by converting it to/from `native_type`.
template<class T, detail::marshal_format Format>
struct adapted_proxy_traits;

template<class T, detail::marshal_format Format>
using adapted_proxy_native_type_t = adapted_proxy_traits<std::remove_cvref_t<T>, Format>::native_type;

template<class T, class Format>
concept adapted_proxy =
    detail::marshal_format<Format> &&
    requires(std::remove_cvref_t<T> const& v) {
        typename adapted_proxy_native_type_t<T, Format>;
        { adapted_proxy_traits<std::remove_cvref_t<T>, Format>::to_native_type(v) }
            -> std::convertible_to<adapted_proxy_native_type_t<T, Format>>;
    };

namespace detail {

template<class T, class Format>
concept proxy_writable =
    detail::marshal_format<Format> &&
    adapted_proxy<T, Format> &&
    requires(adapted_proxy_native_type_t<T, Format> p) {
        { adapted_proxy_traits<std::remove_cvref_t<T>, Format>::from_native_type(std::move(p)) }
            -> std::convertible_to<std::remove_cvref_t<T>>;
    } &&
    std::is_assignable_v<std::remove_cvref_t<T>&, std::remove_cvref_t<T>>;

} // detail

// --------------------------------------------------------------

// Customization point
template<class ClassT>
struct adapted_class_traits;

template<class T>
concept adapted_class = requires {
    adapted_class_traits<std::remove_cvref_t<T>>::fields;
};

// Customization point
template<class T>
struct adapted_optional_traits;

template<class T>
using adapted_optional_value_t = adapted_optional_traits<std::remove_cvref_t<T>>::value_type;

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
    { *opt } -> std::convertible_to<adapted_optional_value_t<T> const&>;
};

namespace detail {

template<class T>
concept optional_writable =
    adapted_optional<T> &&
    std::default_initializable<std::remove_cvref_t<T>> &&
    std::is_assignable_v<std::remove_cvref_t<T>&, std::remove_cvref_t<T>> &&
    std::is_assignable_v<std::remove_cvref_t<T>&, adapted_optional_value_t<T>>;


template<class T, detail::marshal_format Format>
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

template<class Tup, detail::marshal_format Format, std::size_t I = 0>
[[nodiscard]] consteval bool is_tuple_elements_serializable_impl();

template<class T, detail::marshal_format Format>
consteval bool is_serializable_impl()
{
    using V = std::remove_cvref_t<T>;

    static_assert(
        !(adapted_class<V> && adapted_proxy<V, Format>),
        "a type cannot be adapted both as a class and as a proxy"
    );

    if constexpr (adapted_proxy<V, Format>) {
        return is_serializable_impl<adapted_proxy_native_type_t<V, Format>, Format>();

    } else if constexpr (adapted_class<V>) {
        return true;

    } else if constexpr (adapted_optional<V>) {
        return is_serializable_impl<adapted_optional_value_t<V>, Format>();

    } else if constexpr (is_serializable_scalar<V>::value) {
        return true;

    } else if constexpr (ranges::mapping_range<V>) {
        return
            Format::template map_key<ranges::range_key_t<V>> &&
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

template<class Tup, detail::marshal_format Format, std::size_t I>
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
concept serializable_proxy =
    detail::marshal_format<Format> &&
    adapted_proxy<T, Format> &&
    detail::is_serializable_impl<adapted_proxy_native_type_t<T, Format>, Format>();

template<class T, class Format = generic_format>
concept serializable_class =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    adapted_class<T>;

template<class T, class Format = generic_format>
concept serializable_optional =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    !serializable_class<T, Format> &&
    adapted_optional<T> &&
    detail::is_serializable_impl<adapted_optional_value_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable_scalar =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    detail::is_serializable_scalar<std::remove_cvref_t<T>>::value;

template<class T, class Format = generic_format>
concept serializable_map =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    ranges::mapping_range<T> &&
    Format::template map_key<ranges::range_key_t<T>> &&
    detail::is_serializable_impl<ranges::range_key_t<T>, Format>() &&
    detail::is_serializable_impl<ranges::range_mapped_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable_array =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    !ranges::mapping_range<T> &&
    std::ranges::input_range<T> &&
    detail::is_serializable_impl<std::ranges::range_value_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable_tuple =
    detail::marshal_format<Format> &&
    !serializable_proxy<T, Format> &&
    !serializable_class<T, Format> &&
    !adapted_optional<T> &&
    !serializable_scalar<T, Format> &&
    !std::ranges::range<T> &&
    alloy::TupleLike<T> &&
    detail::is_tuple_elements_serializable_impl<std::remove_cvref_t<T>, Format>();

template<class T, class Format = generic_format>
concept serializable =
    detail::marshal_format<Format> &&
    (
        serializable_proxy<T, Format> ||
        serializable_class<T, Format> ||
        serializable_optional<T, Format> ||
        serializable_scalar<T, Format> ||
        serializable_map<T, Format> ||
        serializable_array<T, Format> ||
        serializable_tuple<T, Format>
    );

// ---------------------------------------------------

namespace detail {

template<class T, marshal_format Format>
[[nodiscard]] consteval bool is_deserializable_impl();

template<class T, class Format>
concept loadable =
    marshal_format<Format> &&
    std::default_initializable<std::remove_cvref_t<T>> &&
    std::movable<std::remove_cvref_t<T>> &&
    is_deserializable_impl<std::remove_cvref_t<T>, Format>();

template<class T, marshal_format Format>
consteval bool is_deserializable_impl()
{
    using V = std::remove_cvref_t<T>;

    if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
        return false;

    } else if constexpr (!is_serializable_impl<V, Format>()) {
        return false;

    } else if constexpr (adapted_proxy<V, Format>) {
        return loadable<adapted_proxy_native_type_t<V, Format>, Format> && proxy_writable<V, Format>;

    } else if constexpr (adapted_class<V>) {
        return true;

    } else if constexpr (adapted_optional<V>) {
        return loadable<adapted_optional_value_t<V>, Format> && optional_writable<V>;

    } else if constexpr (is_serializable_scalar<V>::value) {
        return Format::template loadable_scalar<V>;

    } else if constexpr (ranges::mapping_range<V>) {
        return
            container::mapping_container<V> &&
            Format::template loadable_key<ranges::range_key_t<V>> &&
            loadable<ranges::range_key_t<V>, Format> &&
            loadable<ranges::range_mapped_t<V>, Format>;

    } else if constexpr (std::ranges::input_range<V>) {
        if constexpr (container::growable_array<V>) {
            return loadable<std::ranges::range_value_t<V>, Format>;

        } else if constexpr (container::fixed_array<V>) {
            return is_deserializable_impl<std::ranges::range_value_t<V>, Format>();

        } else {
            return false;
        }

    } else if constexpr (alloy::TupleLike<V>) {
        return is_tuple_elements_serializable_impl<V, Format>();   // non-const get<I> + recurse

    } else {
        return false;
    }
}

} // detail

template<class T, class Format = generic_format>
concept deserializable_proxy =
    serializable_proxy<T, Format> &&
    detail::is_deserializable_impl<T, Format>();

template<class T, class Format = generic_format>
concept deserializable_class =
    serializable_class<T, Format> &&
    detail::is_deserializable_impl<T, Format>();

template<class T, class Format = generic_format>
concept deserializable_optional =
    serializable_optional<T, Format> &&
    detail::is_deserializable_impl<T, Format>();

template<class T, class Format = generic_format>
concept deserializable_scalar =
    serializable_scalar<T, Format> &&
    detail::is_deserializable_impl<T, Format>();

template<class T, class Format = generic_format>
concept deserializable_map =
    serializable_map<T, Format> &&
    detail::loadable<T, Format>;

template<class T, class Format = generic_format>
concept deserializable_array =
    serializable_array<T, Format> &&
    detail::loadable<T, Format>;

template<class T, class Format = generic_format>
concept deserializable_tuple =
    serializable_tuple<T, Format> &&
    detail::loadable<T, Format>;

template<class T, class Format = generic_format>
concept deserializable =
    detail::marshal_format<Format> &&
    (
        deserializable_proxy<T, Format> ||
        deserializable_class<T, Format> ||
        deserializable_optional<T, Format> ||
        deserializable_scalar<T, Format> ||
        deserializable_map<T, Format> ||
        deserializable_array<T, Format> ||
        deserializable_tuple<T, Format>
    );

} // iris::marshal

#endif

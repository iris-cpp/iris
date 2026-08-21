#ifndef IRIS_ZZ_RANGES_HPP
#define IRIS_ZZ_RANGES_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/utility.hpp>

#include <ranges> // IWYU pragma: export
#include <concepts>
#include <type_traits>
#include <utility>

namespace iris::ranges {

namespace detail {

template<std::ranges::range R>
using kv_element_t = std::remove_cvref_t<std::ranges::range_reference_t<R>>;

} // detail

template<class R>
concept key_value_range =
    std::ranges::input_range<R> &&
    requires {
        typename detail::kv_element_t<R>;
        requires std::tuple_size<detail::kv_element_t<R>>::value == 2;
        typename std::tuple_element_t<0, detail::kv_element_t<R>>;
        typename std::tuple_element_t<1, detail::kv_element_t<R>>;
    } &&
    gettable<0, std::ranges::range_reference_t<R>> &&
    gettable<1, std::ranges::range_reference_t<R>>;

template<key_value_range R>
using range_key_t    = std::remove_cvref_t<std::tuple_element_t<0, detail::kv_element_t<R>>>;

template<key_value_range R>
using range_mapped_t = std::remove_cvref_t<std::tuple_element_t<1, detail::kv_element_t<R>>>;



template<class C>
concept key_value_container =
    key_value_range<C> &&
    std::default_initializable<std::remove_cvref_t<C>> &&
    requires {
        typename std::remove_cvref_t<C>::key_type;
        typename std::remove_cvref_t<C>::mapped_type;
    } &&
    std::same_as<range_key_t<C>,    typename std::remove_cvref_t<C>::key_type> &&
    std::same_as<range_mapped_t<C>, typename std::remove_cvref_t<C>::mapped_type> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type m
    ) {
        c.emplace(std::move(k), std::move(m));
    };

template<class C>
concept unique_key_value_container =
    key_value_container<C> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type m
    ) {
        { c.try_emplace(std::move(k), std::move(m)).second } -> std::convertible_to<bool>;
    };

} // iris::ranges

#endif

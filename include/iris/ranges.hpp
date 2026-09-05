#ifndef IRIS_ZZ_RANGES_HPP
#define IRIS_ZZ_RANGES_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/iterator.hpp> // IWYU pragma: keep
#include <iris/utility.hpp>

#include <iterator> // IWYU pragma: keep
#include <ranges> // IWYU pragma: export
#include <concepts>
#include <type_traits>
#include <utility>

namespace iris::ranges {

namespace detail {

template<std::ranges::range R>
using kv_element_t = std::remove_cvref_t<std::ranges::range_reference_t<R>>;

} // detail

// `R` is a range with value type of 2-element tuple.
// This accepts also array of pair. If stricter check is needed, use `mapping_range<R>`.
// Note: To convert `key_value_range` to `mapping_range`, use `iris::ranges::as_map`.
template<class R>
concept key_value_range =
    std::ranges::input_range<R> &&
    requires {
        typename detail::kv_element_t<R>;
        requires std::tuple_size<detail::kv_element_t<R>>::value == 2;
        typename std::tuple_element<0, detail::kv_element_t<R>>::type;
        typename std::tuple_element<1, detail::kv_element_t<R>>::type;
    } &&
    gettable<0, std::ranges::range_reference_t<R>> &&
    gettable<1, std::ranges::range_reference_t<R>>;

template<key_value_range R>
using range_key_t    = std::remove_cvref_t<std::tuple_element_t<0, detail::kv_element_t<R>>>;

template<key_value_range R>
using range_mapped_t = std::remove_cvref_t<std::tuple_element_t<1, detail::kv_element_t<R>>>;


// `R` is a range with value type of 2-element tuple AND it has certain
// strictly map-specific traits such as `::key_type` and `::mapped_type`.
// To include more loose type like an array of pair, use `key_value_range<R>`.
template<class R>
concept mapping_range =
    key_value_range<R> &&
    requires {
        typename std::remove_cvref_t<R>::key_type;
        typename std::remove_cvref_t<R>::mapped_type;
    } &&
    std::same_as<range_key_t<R>,    typename std::remove_cvref_t<R>::key_type> &&
    std::same_as<range_mapped_t<R>, typename std::remove_cvref_t<R>::mapped_type>;


// Thin view that adds the map-specific trait to the underlying range.
// Can be used for making `key_value_range` model `mapping_range`.
template<std::ranges::input_range R>
    requires ranges::key_value_range<R>
struct as_map_view : std::ranges::view_interface<as_map_view<R>>
{
    R base_ = R{};

public:
    using key_type    = range_key_t<R>;
    using mapped_type = range_mapped_t<R>;

    as_map_view() requires std::default_initializable<R> = default;
    constexpr explicit as_map_view(R base) : base_(std::move(base)) {}

    [[nodiscard]] constexpr R base() const& requires std::copy_constructible<R> { return base_; }
    [[nodiscard]] constexpr R base() && { return std::move(base_); }

    [[nodiscard]] constexpr auto begin()       { return std::ranges::begin(base_); }
    [[nodiscard]] constexpr auto end()         { return std::ranges::end(base_); }
    [[nodiscard]] constexpr auto begin() const requires std::ranges::range<R const> { return std::ranges::begin(base_); }
    [[nodiscard]] constexpr auto end()   const requires std::ranges::range<R const> { return std::ranges::end(base_); }

    [[nodiscard]] constexpr auto size()       requires std::ranges::sized_range<R>       { return std::ranges::size(base_); }
    [[nodiscard]] constexpr auto size() const requires std::ranges::sized_range<R const> { return std::ranges::size(base_); }
};

template<class R>
as_map_view(R&&) -> as_map_view<std::views::all_t<R>>;


namespace detail {

struct as_map_fn : std::ranges::range_adaptor_closure<as_map_fn>
{
    template<std::ranges::viewable_range R>
        requires ranges::key_value_range<R>
    [[nodiscard]] static constexpr auto operator()(R&& r)
    {
        return as_map_view{std::views::all(std::forward<R>(r))};
    }
};

} // detail

// Can be used for making `key_value_range` model `mapping_range`.
[[maybe_unused]] inline constexpr detail::as_map_fn as_map{};

} // iris::ranges

template<class V>
inline constexpr bool std::ranges::enable_borrowed_range<iris::ranges::as_map_view<V>>
    = std::ranges::enable_borrowed_range<V>;


namespace iris::ranges::dummy {

template<class K, class V>
struct key_value_range
{
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;
    ~key_value_range() = delete;
};

template<class K, class V>
struct mapping_range
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;
    ~mapping_range() = delete;
};

} // iris::ranges::dummy

#endif

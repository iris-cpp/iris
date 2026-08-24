#ifndef IRIS_ZZ_ITERATOR_HPP
#define IRIS_ZZ_ITERATOR_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iterator>
#include <compare>

namespace iris {

template<std::input_or_output_iterator It>
struct iterator_tags_base;

template<std::input_or_output_iterator It>
    requires requires {
        typename std::iterator_traits<It>::iterator_category;
        typename std::iterator_traits<It>::iterator_concept;
    }
struct iterator_tags_base<It>
{
    using iterator_base_type = It;
    using iterator_category = std::iterator_traits<It>::iterator_category;
    using iterator_concept = std::iterator_traits<It>::iterator_concept;

    [[nodiscard]] constexpr bool operator==(iterator_tags_base const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(iterator_tags_base const&) const noexcept = default;
};

template<std::input_or_output_iterator It>
    requires
        requires { typename std::iterator_traits<It>::iterator_category; } &&
        (!requires { typename std::iterator_traits<It>::iterator_concept; })
struct iterator_tags_base<It>
{
    using iterator_base_type = It;
    using iterator_category = std::iterator_traits<It>::iterator_category;

    [[nodiscard]] constexpr bool operator==(iterator_tags_base const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(iterator_tags_base const&) const noexcept = default;
};

template<std::input_or_output_iterator It>
    requires
        (!requires { typename std::iterator_traits<It>::iterator_category; }) &&
        requires { typename std::iterator_traits<It>::iterator_concept; }
struct iterator_tags_base<It>
{
    using iterator_base_type = It;
    using iterator_concept = std::iterator_traits<It>::iterator_concept;

    [[nodiscard]] constexpr bool operator==(iterator_tags_base const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(iterator_tags_base const&) const noexcept = default;
};

// ----------------------------------------------

template<std::input_or_output_iterator It>
struct iterator_base : iterator_tags_base<It>
{
    using difference_type = std::iterator_traits<It>::difference_type;

    [[nodiscard]] constexpr bool operator==(iterator_base const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(iterator_base const&) const noexcept = default;
};

} // iris

#endif

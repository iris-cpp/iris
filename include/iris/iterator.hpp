#ifndef IRIS_ZZ_ITERATOR_HPP
#define IRIS_ZZ_ITERATOR_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iterator>
#include <type_traits>
#include <concepts>

namespace iris {

namespace detail {

template<std::input_iterator It>
std::input_iterator_tag iter_concept_of();

template<std::forward_iterator It>
std::forward_iterator_tag iter_concept_of();

template<std::bidirectional_iterator It>
std::bidirectional_iterator_tag iter_concept_of();

template<std::random_access_iterator It>
std::random_access_iterator_tag iter_concept_of();

template<std::contiguous_iterator It>
std::contiguous_iterator_tag iter_concept_of();

} // detail

template<std::input_iterator It>
using iter_concept_t = decltype(detail::iter_concept_of<It>());

template<class It>
using iter_cat_t = std::iterator_traits<It>::iterator_category;

template<class It>
concept has_iter_cat = requires { typename iter_cat_t<It>; };

// min(Tag, Limit) on the standard tag hierarchy
template<class Tag, class Limit>
    requires std::derived_from<Tag, Limit> || std::derived_from<Limit, Tag>
using clamp_iter_tag_t = std::conditional_t<std::derived_from<Tag, Limit>, Limit, Tag>;

} // iris

#endif

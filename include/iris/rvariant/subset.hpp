#ifndef IRIS_RVARIANT_SUBSET_HPP
#define IRIS_RVARIANT_SUBSET_HPP

// SPDX-License-Identifier: MIT

#include <iris/rvariant/detail/rvariant_fwd.hpp>
#include <iris/rvariant/variant_helper.hpp>
#include <iris/type_traits.hpp>

#include <type_traits>

namespace iris {

namespace detail {

template<class From, class To>
struct subset_reindex_impl;

template<class... Ts, class... Us>
struct subset_reindex_impl<type_list<Ts...>, type_list<Us...>>
{
    static constexpr std::size_t table[]{
        find_index_v<Ts, type_list<Us...>>...
    };
};

template<class From, class To>
[[nodiscard]] consteval std::size_t subset_reindex(std::size_t index) noexcept
{
    return subset_reindex_impl<typename From::unwrapped_types, typename To::unwrapped_types>::table[index];
}

} // detail


namespace rvariant_set {

template<class W, class V>
struct is_subset_of : std::false_type
{
    static_assert(is_ttp_specialization_of_v<W, rvariant>);
    static_assert(is_ttp_specialization_of_v<V, rvariant>);
};

template<class... Us, class... Ts>
struct is_subset_of<rvariant<Us...>, rvariant<Ts...>>
    : std::conjunction<
        std::disjunction<
            is_in<Us, Ts...>,
            is_in<Us, unwrap_recursive_type<Ts>...>
        >...
    >
{};

// subset_of<int, int> => true
// subset_of<int, recursive_wrapper<int>> => true
// subset_of<recursive_wrapper<int>, recursive_wrapper<int>> => true
// subset_of<recursive_wrapper<int>, int> => false

template<class W, class V>
inline constexpr bool is_subset_of_v = is_subset_of<W, V>::value;

template<class W, class V>
concept subset_of = is_subset_of_v<W, V>;

template<class W, class V>
concept equivalent_to = subset_of<W, V> && subset_of<V, W>;

} // rvariant_set

} // iris

#endif

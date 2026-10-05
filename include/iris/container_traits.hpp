#ifndef IRIS_ZZ_CONTAINER_TRAITS_HPP
#define IRIS_ZZ_CONTAINER_TRAITS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/iterator.hpp> // IWYU pragma: keep
#include <iris/ranges.hpp> // IWYU pragma: keep

#include <algorithm>
#include <memory>
#include <iterator>
#include <ranges>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cassert>

namespace iris::container {

namespace detail {

template<class T>
concept allocator_aware_impl = requires(T const& c) {
    typename T::allocator_type;
    { c.get_allocator() } -> std::same_as<typename T::allocator_type>;
};

} // detail

template<class T>
concept allocator_aware = detail::allocator_aware_impl<std::remove_cvref_t<T>>;

template<class T>
concept has_stateless_allocator = std::allocator_traits<typename std::remove_cvref_t<T>::allocator_type>::is_always_equal::value;

template<class ContainerT>
concept keeps_allocator_on_move_assignment =
    !allocator_aware<ContainerT> ||
    has_stateless_allocator<ContainerT> ||
    !std::allocator_traits<typename std::remove_cvref_t<ContainerT>::allocator_type>::propagate_on_container_move_assignment::value;

template<class A, class B>
concept has_same_allocator_type =
    allocator_aware<A> && allocator_aware<B> &&
    std::same_as<typename std::remove_cvref_t<A>::allocator_type, typename std::remove_cvref_t<B>::allocator_type>;

template<class C>
concept mapping_container =
    ranges::mapping_range<C> &&
    std::default_initializable<std::remove_cvref_t<C>> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type v
    ) {
        c.emplace(std::move(k), std::move(v));
    };

template<class C>
concept unique_mapping_container =
    mapping_container<C> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type v
    ) {
        { c.try_emplace(std::move(k), std::move(v)).second } -> std::convertible_to<bool>;
        { c.insert_or_assign(std::move(k), std::move(v)).second } -> std::convertible_to<bool>;
    };


namespace detail {

template<class T>
struct element_impl
{
    using type = T;
};

template<class K, class V>
struct element_impl<std::pair<K const, V>>
{
    using type = std::pair<K, V>;
};

} // detail

// The type of an element made before it is added to `ContainerT`: the value type, whose key is not const
template<class ContainerT>
using element_t = detail::element_impl<std::ranges::range_value_t<ContainerT>>::type;


namespace detail {

template<class ContainerT>
concept has_front = requires(ContainerT& cont) {
    { cont.front() } -> std::same_as<std::ranges::range_reference_t<ContainerT>>;
};

template<class ContainerT>
concept has_back = requires(ContainerT& cont) {
    { cont.back() } -> std::same_as<std::ranges::range_reference_t<ContainerT>>;
};

struct front_fn
{
    template<std::ranges::borrowed_range R>
        requires has_front<R>
    [[nodiscard]] static constexpr decltype(auto) operator()(R&& r)
        noexcept(noexcept(r.front()))
    {
        return r.front();
    }

    template<std::ranges::borrowed_range R>
        requires (!has_front<R>) && std::ranges::input_range<R>
    [[nodiscard]] static constexpr decltype(auto) operator()(R&& r)
        noexcept(noexcept(*std::ranges::begin(r)))
    {
        return *std::ranges::begin(r);
    }
};

struct back_fn
{
    template<std::ranges::borrowed_range R>
        requires has_back<R>
    [[nodiscard]] static constexpr decltype(auto) operator()(R&& r)
        noexcept(noexcept(r.back()))
    {
        return r.back();
    }

    template<std::ranges::borrowed_range R>
        requires
            (!has_back<R>) &&
            std::ranges::bidirectional_range<R> &&
            std::ranges::common_range<R>
    [[nodiscard]] static constexpr decltype(auto) operator()(R&& r)
        noexcept(noexcept(*std::ranges::prev(std::ranges::end(r))))
    {
        return *std::ranges::prev(std::ranges::end(r));
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::front_fn front{};
[[maybe_unused]] inline constexpr detail::back_fn back{};

template<class R> concept front_accessible = requires(R& r) { front(r); };
template<class R> concept back_accessible = requires(R& r) { back(r); };


// ------------------------------------------------------------

namespace detail {

template<class ContainerT, class... Args>
concept emplaceable = std::constructible_from<std::ranges::range_value_t<ContainerT>, Args...>;

template<class ContainerT, class... Args>
concept position_emplaceable =
    emplaceable<ContainerT, Args...> &&
    !requires { typename std::remove_cvref_t<ContainerT>::key_type; };

template<class ContainerT, class... Args>
concept has_emplace_front = requires(ContainerT& cont) {
    requires emplaceable<ContainerT, Args...>;
    cont.emplace_front(std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_push_front = requires(ContainerT& cont) {
    requires sizeof...(Args) == 1;
    cont.push_front(std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_begin_emplace = requires(ContainerT& cont) {
    requires std::ranges::range<ContainerT>;
    requires position_emplaceable<ContainerT, Args...>;
    cont.emplace(std::ranges::begin(cont), std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_begin_insert = requires(ContainerT& cont) {
    requires sizeof...(Args) == 1;
    requires std::ranges::range<ContainerT>;
    cont.insert(std::ranges::begin(cont), std::declval<Args>()...);
};

} // detail

template<class ContainerT, class... Args>
concept front_pushable =
    detail::has_emplace_front<ContainerT, Args...> ||
    detail::has_push_front<ContainerT, Args...>;

template<class ContainerT>
concept default_prependable =
    std::ranges::range<ContainerT> &&
    std::default_initializable<std::ranges::range_value_t<ContainerT>> &&
    (
        detail::has_emplace_front<ContainerT> || detail::has_begin_emplace<ContainerT> ||
        (
            std::move_constructible<std::ranges::range_value_t<ContainerT>> &&
            (
                detail::has_push_front<ContainerT, std::ranges::range_value_t<ContainerT>> ||
                detail::has_begin_insert<ContainerT, std::ranges::range_value_t<ContainerT>>
            )
        )
    );

template<class ContainerT, class... Args>
concept prependable =
    front_pushable<ContainerT, Args...> ||
    detail::has_begin_emplace<ContainerT, Args...> ||
    detail::has_begin_insert<ContainerT, Args...> ||
    requires {
        requires sizeof...(Args) == 0;
        requires default_prependable<ContainerT>;
    };

namespace detail {

template<class ContainerT, class... Args>
concept front_emplace_returns =
    has_emplace_front<ContainerT, Args...> &&
    !std::is_void_v<decltype(std::declval<ContainerT&>().emplace_front(std::declval<Args>()...))>;

template<bool NeedReturn, class ContainerT>
concept default_front_emplace_front_accessible =
    !NeedReturn ||
    front_emplace_returns<ContainerT> ||
    front_accessible<ContainerT> ||
    (
        !has_emplace_front<ContainerT> &&
        (
            has_begin_emplace<ContainerT> ||
            (
                !has_push_front<ContainerT, std::ranges::range_value_t<ContainerT>> &&
                has_begin_insert<ContainerT, std::ranges::range_value_t<ContainerT>>
            )
        )
    );

template<bool NeedReturn, class ContainerT, class... Args>
concept front_emplace_front_accessible =
    !NeedReturn ||
    front_emplace_returns<ContainerT, Args...> ||
    front_accessible<ContainerT> ||
    (
        !has_emplace_front<ContainerT, Args...> &&
        !has_push_front<ContainerT, Args...> &&
        (has_begin_emplace<ContainerT, Args...> || has_begin_insert<ContainerT, Args...>)
    );

template<bool NeedReturn>
struct prepend_fn
{
    template<class ContainerT>
        requires default_prependable<ContainerT> && default_front_emplace_front_accessible<NeedReturn, ContainerT>
    static constexpr decltype(auto)
    operator()(ContainerT& cont)
    {
        if constexpr (has_emplace_front<ContainerT>) {
            if constexpr (NeedReturn) {
                if constexpr (std::is_void_v<decltype(cont.emplace_front())>) {
                    cont.emplace_front();
                    return front(cont);
                } else {
                    return cont.emplace_front();
                }
            } else {
                (void)cont.emplace_front();
            }
        } else {
            using ValueT = std::ranges::range_value_t<ContainerT>;
            if constexpr (has_begin_emplace<ContainerT>) { // prefer most ergonomic insertion for immovable types
                if constexpr (NeedReturn) {
                    return *cont.emplace(std::ranges::begin(cont));
                } else {
                    (void)cont.emplace(std::ranges::begin(cont));
                }
            } else if constexpr (has_push_front<ContainerT, ValueT>) {
                cont.push_front(ValueT{});
                if constexpr (NeedReturn) {
                    return front(cont);
                }
            } else if constexpr (has_begin_insert<ContainerT, ValueT>) {
                if constexpr (NeedReturn) {
                    return *cont.insert(std::ranges::begin(cont), ValueT{});
                } else {
                    (void)cont.insert(std::ranges::begin(cont), ValueT{});
                }
            } else {
                static_assert(false);
            }
        }
    }

    template<class ContainerT, class FirstT, class... Rest>
        requires prependable<ContainerT, FirstT, Rest...> && front_emplace_front_accessible<NeedReturn, ContainerT, FirstT, Rest...>
    static constexpr decltype(auto)
    operator()(ContainerT& cont, FirstT&& first, Rest&&... rest)
    {
        if constexpr (has_emplace_front<ContainerT, FirstT, Rest...>) {
            if constexpr (NeedReturn) {
                if constexpr (std::is_void_v<decltype(cont.emplace_front(std::forward<FirstT>(first), std::forward<Rest>(rest)...))>) {
                    cont.emplace_front(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                    return front(cont);
                } else {
                    return cont.emplace_front(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                }
            } else {
                (void)cont.emplace_front(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
            }
        } else {
            if constexpr (has_push_front<ContainerT, FirstT, Rest...>) {
                static_assert(sizeof...(Rest) == 0);
                cont.push_front(std::forward<FirstT>(first));
                if constexpr (NeedReturn) {
                    return front(cont);
                }
            } else if constexpr (has_begin_emplace<ContainerT, FirstT, Rest...>) {
                if constexpr (NeedReturn) {
                    return *cont.emplace(std::ranges::begin(cont), std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                } else {
                    (void)cont.emplace(std::ranges::begin(cont), std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                }
            } else if constexpr (has_begin_insert<ContainerT, FirstT, Rest...>) {
                static_assert(sizeof...(Rest) == 0);
                if constexpr (NeedReturn) {
                    return *cont.insert(std::ranges::begin(cont), std::forward<FirstT>(first));
                } else {
                    (void)cont.insert(std::ranges::begin(cont), std::forward<FirstT>(first));
                }
            } else {
                static_assert(false);
            }
        }
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::prepend_fn<true> prepend_return{};
[[maybe_unused]] inline constexpr detail::prepend_fn<false> prepend{};


// ------------------------------------------------------------

namespace detail {

template<class ContainerT, class... Args>
concept has_emplace_back = requires(ContainerT& cont) {
    requires emplaceable<ContainerT, Args...>;
    cont.emplace_back(std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_push_back = requires(ContainerT& cont) {
    requires sizeof...(Args) == 1;
    cont.push_back(std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_end_emplace = requires(ContainerT& cont) {
    requires std::ranges::range<ContainerT>;
    requires position_emplaceable<ContainerT, Args...>;
    cont.emplace(std::ranges::end(cont), std::declval<Args>()...);
};

template<class ContainerT, class... Args>
concept has_end_insert = requires(ContainerT& cont) {
    requires sizeof...(Args) == 1;
    requires std::ranges::range<ContainerT>;
    cont.insert(std::ranges::end(cont), std::declval<Args>()...);
};

} // detail

template<class ContainerT, class... Args>
concept back_pushable =
    detail::has_emplace_back<ContainerT, Args...> ||
    detail::has_push_back<ContainerT, Args...>;

template<class ContainerT>
concept default_appendable =
    std::ranges::range<ContainerT> &&
    std::default_initializable<std::ranges::range_value_t<ContainerT>> &&
    (
        detail::has_emplace_back<ContainerT> || detail::has_end_emplace<ContainerT> ||
        (
            std::move_constructible<std::ranges::range_value_t<ContainerT>> &&
            (
                detail::has_push_back<ContainerT, std::ranges::range_value_t<ContainerT>> ||
                detail::has_end_insert<ContainerT, std::ranges::range_value_t<ContainerT>>
            )
        )
    );

template<class ContainerT, class... Args>
concept appendable =
    back_pushable<ContainerT, Args...> ||
    detail::has_end_emplace<ContainerT, Args...> ||
    detail::has_end_insert<ContainerT, Args...> ||
    requires {
        requires sizeof...(Args) == 0;
        requires default_appendable<ContainerT>;
    };

namespace detail {

template<class ContainerT, class... Args>
concept back_emplace_returns =
    has_emplace_back<ContainerT, Args...> &&
    !std::is_void_v<decltype(std::declval<ContainerT&>().emplace_back(std::declval<Args>()...))>;

template<bool NeedReturn, class ContainerT>
concept default_back_emplace_back_accessible =
    !NeedReturn ||
    back_emplace_returns<ContainerT> ||
    back_accessible<ContainerT> ||
    (
        !has_emplace_back<ContainerT> &&
        (
            has_end_emplace<ContainerT> ||
            (
                !has_push_back<ContainerT, std::ranges::range_value_t<ContainerT>> &&
                has_end_insert<ContainerT, std::ranges::range_value_t<ContainerT>>
            )
        )
    );

template<bool NeedReturn, class ContainerT, class... Args>
concept back_emplace_back_accessible =
    !NeedReturn ||
    back_emplace_returns<ContainerT, Args...> ||
    back_accessible<ContainerT> ||
    (
        !has_emplace_back<ContainerT, Args...> &&
        !has_push_back<ContainerT, Args...> &&
        (has_end_emplace<ContainerT, Args...> || has_end_insert<ContainerT, Args...>)
    );

template<bool NeedReturn>
struct append_fn
{
    template<class ContainerT>
        requires default_appendable<ContainerT> && default_back_emplace_back_accessible<NeedReturn, ContainerT>
    static constexpr decltype(auto)
    operator()(ContainerT& cont)
    {
        if constexpr (has_emplace_back<ContainerT>) {
            if constexpr (NeedReturn) {
                if constexpr (std::is_void_v<decltype(cont.emplace_back())>) {
                    cont.emplace_back();
                    return back(cont);
                } else {
                    return cont.emplace_back();
                }
            } else {
                (void)cont.emplace_back();
            }
        } else {
            using ValueT = std::ranges::range_value_t<ContainerT>;
            if constexpr (has_end_emplace<ContainerT>) { // prefer most ergonomic insertion for immovable types
                if constexpr (NeedReturn) {
                    return *cont.emplace(std::ranges::end(cont));
                } else {
                    (void)cont.emplace(std::ranges::end(cont));
                }
            } else if constexpr (has_push_back<ContainerT, ValueT>) {
                cont.push_back(ValueT{});
                if constexpr (NeedReturn) {
                    return back(cont);
                }
            } else if constexpr (has_end_insert<ContainerT, ValueT>) {
                if constexpr (NeedReturn) {
                    return *cont.insert(std::ranges::end(cont), ValueT{});
                } else {
                    (void)cont.insert(std::ranges::end(cont), ValueT{});
                }
            } else {
                static_assert(false);
            }
        }
    }

    template<class ContainerT, class FirstT, class... Rest>
        requires appendable<ContainerT, FirstT, Rest...> && back_emplace_back_accessible<NeedReturn, ContainerT, FirstT, Rest...>
    static constexpr decltype(auto)
    operator()(ContainerT& cont, FirstT&& first, Rest&&... rest)
    {
        if constexpr (has_emplace_back<ContainerT, FirstT, Rest...>) {
            if constexpr (NeedReturn) {
                if constexpr (std::is_void_v<decltype(cont.emplace_back(std::forward<FirstT>(first), std::forward<Rest>(rest)...))>) {
                    cont.emplace_back(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                    return back(cont);
                } else {
                    return cont.emplace_back(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                }
            } else {
                (void)cont.emplace_back(std::forward<FirstT>(first), std::forward<Rest>(rest)...);
            }
        } else {
            if constexpr (has_push_back<ContainerT, FirstT, Rest...>) {
                static_assert(sizeof...(Rest) == 0);
                cont.push_back(std::forward<FirstT>(first));
                if constexpr (NeedReturn) {
                    return back(cont);
                }
            } else if constexpr (has_end_emplace<ContainerT, FirstT, Rest...>) {
                if constexpr (NeedReturn) {
                    return *cont.emplace(std::ranges::end(cont), std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                } else {
                    (void)cont.emplace(std::ranges::end(cont), std::forward<FirstT>(first), std::forward<Rest>(rest)...);
                }
            } else if constexpr (has_end_insert<ContainerT, FirstT, Rest...>) {
                static_assert(sizeof...(Rest) == 0);
                if constexpr (NeedReturn) {
                    return *cont.insert(std::ranges::end(cont), std::forward<FirstT>(first));
                } else {
                    (void)cont.insert(std::ranges::end(cont), std::forward<FirstT>(first));
                }
            } else {
                static_assert(false);
            }
        }
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::append_fn<true> append_return{};
[[maybe_unused]] inline constexpr detail::append_fn<false> append{};

// ------------------------------------------------------------

namespace detail {

template<class ContainerT>
concept begin_erasable =
    std::ranges::range<ContainerT> &&
    requires(ContainerT& cont) {
        cont.erase(std::ranges::begin(cont));
    };

template<class ContainerT>
concept end_erasable =
    std::ranges::bidirectional_range<ContainerT> &&
    std::ranges::common_range<ContainerT> &&
    requires(ContainerT& cont) {
        cont.erase(std::ranges::prev(std::ranges::end(cont)));
    };

template<class ContainerT>
concept front_poppable = requires(ContainerT& cont) {
    cont.pop_front();
};

template<class ContainerT>
concept back_poppable = requires(ContainerT& cont) {
    cont.pop_back();
};

struct erase_front_fn
{
    template<class ContainerT>
        requires front_poppable<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.pop_front()))
    {
        cont.pop_front();
    }

    template<class ContainerT>
        requires (!front_poppable<ContainerT>) && begin_erasable<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.erase(std::ranges::begin(cont))))
    {
        cont.erase(std::ranges::begin(cont));
    }
};

struct erase_back_fn
{
    template<class ContainerT>
        requires back_poppable<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.pop_back()))
    {
        cont.pop_back();
    }

    template<class ContainerT>
        requires (!back_poppable<ContainerT>) && end_erasable<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.erase(std::ranges::prev(std::ranges::end(cont)))))
    {
        cont.erase(std::ranges::prev(std::ranges::end(cont)));
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::erase_front_fn erase_front{};
[[maybe_unused]] inline constexpr detail::erase_back_fn erase_back{};

template<class ContainerT> concept front_erasable = requires(ContainerT& cont) { erase_front(cont); };
template<class ContainerT> concept back_erasable = requires(ContainerT& cont) { erase_back(cont); };

// ------------------------------------------------------------

template<class ContainerT>
concept growable_array =
    std::ranges::range<ContainerT> &&
    (
        (
            std::default_initializable<std::ranges::range_value_t<ContainerT>> &&
            appendable<ContainerT>
        ) ||
        (
            std::move_constructible<std::ranges::range_value_t<ContainerT>> &&
            appendable<ContainerT, std::ranges::range_value_t<ContainerT>>
        )
    );

template<class ContainerT>
concept fixed_array =
    !growable_array<ContainerT> &&
    std::ranges::output_range<ContainerT, std::ranges::range_value_t<ContainerT>> &&
    std::ranges::sized_range<ContainerT>;

// ------------------------------------------------------------

template<class ContainerT, class SizeT>
concept has_reserve = requires(std::remove_cvref_t<ContainerT>& cont, SizeT const size) {
    cont.reserve(size);
};

template<class ContainerT>
concept has_shrink_to_fit = requires(std::remove_cvref_t<ContainerT>& cont) {
    cont.shrink_to_fit();
};


template<class It, class ElemT>
concept compatible_iterator =
    // The standard only requires "not participating unless `It` qualifies as an input
    // iterator" and leaves the extent unspecified; implementations check as little as
    // `iterator_traits<It>::iterator_category`. The iterators that satisfy Cpp17InputIterator
    // but not `std::input_iterator` are legacy defects (e.g. `difference_type = void`),
    // which we do not support, so `std::input_iterator` is used here.
    std::input_iterator<It> &&
    // This is NOT `std::convertible_to`; see: https://stackoverflow.com/questions/79940611/stdfrom-range-does-not-work-with-explicit-conversions?noredirect=1
    std::constructible_from<ElemT, std::iter_reference_t<It>>;

// Note: `compatible_range` does NOT subsume `compatible_iterator` by design.
// Historically in the C++ standard, `container(std::from_range_t, R)` and `container(it, se)`
// meant two distinct semantics. For details, see the comment on `compatible_iterator`.
//
// Also note that in the standard, the element requirement of `container(it, se)` is a
// precondition (Cpp17EmplaceConstructible; violation is a hard error inside the constructor),
// whereas `compatible_iterator` makes it a constraint. This keeps
// `std::constructible_from<Container, It, Se>` honest, which generic code such as
// `std::ranges::to` relies on when choosing a construction strategy.
template<class R, class ElemT>
concept compatible_range =
    std::ranges::input_range<R> &&
    std::convertible_to<std::ranges::range_reference_t<R>, ElemT>;

// ------------------------------------------------------------

namespace detail {

template<class ContainerT, class R>
concept has_append_range = requires(ContainerT& cont, R&& r) {
    cont.append_range(std::forward<R>(r));
};

template<class ContainerT, class R>
concept has_end_insert_range =
    std::ranges::range<ContainerT> &&
    requires(ContainerT& cont, R&& r) {
        cont.insert_range(std::ranges::end(cont), std::forward<R>(r));
    };

template<class ContainerT, class R>
concept has_insert_range = requires(ContainerT& cont, R&& r) {
    cont.insert_range(std::forward<R>(r));
};

template<class ContainerT, class R>
concept has_end_insert_iterators =
    std::ranges::range<ContainerT> &&
    std::ranges::common_range<R> &&
    requires(ContainerT& cont, R& r) {
        cont.insert(std::ranges::end(cont), std::ranges::begin(r), std::ranges::end(r));
    };

template<class ContainerT, class R>
concept has_insert_iterators =
    std::ranges::common_range<R> &&
    requires(ContainerT& cont, R& r) {
        cont.insert(std::ranges::begin(r), std::ranges::end(r));
    };

// The elements of `r` at the end of `cont`, or into the associative `cont`
struct append_range_fn
{
    template<class ContainerT, class R>
        requires
            compatible_range<R, std::ranges::range_value_t<ContainerT>> &&
            (
                has_append_range<ContainerT, R> ||
                has_end_insert_range<ContainerT, R> ||
                has_insert_range<ContainerT, R> ||
                has_end_insert_iterators<ContainerT, R> ||
                has_insert_iterators<ContainerT, R>
            )
    static constexpr void operator()(ContainerT& cont, R&& r)
    {
        if constexpr (has_append_range<ContainerT, R>) {
            cont.append_range(std::forward<R>(r));

        } else if constexpr (has_end_insert_range<ContainerT, R>) {
            cont.insert_range(std::ranges::end(cont), std::forward<R>(r));

        } else if constexpr (has_insert_range<ContainerT, R>) {
            cont.insert_range(std::forward<R>(r));

        } else if constexpr (has_end_insert_iterators<ContainerT, R>) {
            cont.insert(std::ranges::end(cont), std::ranges::begin(r), std::ranges::end(r));

        } else {
            cont.insert(std::ranges::begin(r), std::ranges::end(r));
        }
    }
};

struct clear_fn
{
    template<class ContainerT>
        requires requires(ContainerT& cont) { cont.clear(); }
    static constexpr void operator()(ContainerT& cont) noexcept(noexcept(cont.clear()))
    {
        cont.clear();
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::append_range_fn append_range{};
[[maybe_unused]] inline constexpr detail::clear_fn clear{};

// ------------------------------------------------------------

namespace detail {

template<class DstT, class SrcT>
concept element_wise_transferable_from =
    requires(DstT& dst, SrcT src) {
        append_range(dst, std::forward<SrcT>(src) | std::views::as_rvalue);
    };

template<class DstT, class SrcT>
concept has_splice = requires(DstT& dst, SrcT src) {
    dst.splice(std::ranges::end(dst), std::forward<SrcT>(src));
};

template<class DstT, class SrcT>
concept has_merge = requires(DstT& dst, SrcT src) {
    typename std::remove_cvref_t<DstT>::key_type; // `std::list::merge` merges sorted lists
    dst.merge(std::forward<SrcT>(src));
};

template<class DstT, class SrcT>
concept node_family_transferable_from =
    has_splice<DstT, SrcT> ||
    has_merge<DstT, SrcT>;

template<class SrcT>
using extracted_t = decltype(std::declval<std::remove_reference_t<SrcT>&&>().extract());

template<class Extracted>
[[nodiscard]] constexpr auto moved_elements(Extracted& extracted) noexcept
{
    if constexpr (requires { extracted.keys; extracted.values; }) {
        // `std::views::zip` would be simpler, but Clang 22 rejects inserting two kinds of `zip_view` into libc++'s
        // flat containers in one translation unit (an access check on a constraint of `__product_iterator_traits`).
        using key_type = std::ranges::range_value_t<decltype(extracted.keys)>;
        using mapped_type = std::ranges::range_value_t<decltype(extracted.values)>;
        return std::views::zip_transform(
            [](auto&& key, auto&& value) { return std::pair<key_type, mapped_type>(std::move(key), std::move(value)); },
            extracted.keys, extracted.values
        );
    } else {
        return extracted | std::views::as_rvalue;
    }
}

template<class DstT, class SrcT>
concept flat_transferable_from =
    (
        std::same_as<extracted_t<SrcT>, typename std::remove_cvref_t<SrcT>::containers> ||
        std::same_as<extracted_t<SrcT>, typename std::remove_cvref_t<SrcT>::container_type>
    ) &&
    requires(DstT& dst, extracted_t<SrcT>& extracted) {
        append_range(dst, detail::moved_elements(extracted));
    };

template<class DstT, class SrcT>
concept flat_sorted_transferable_from =
    flat_transferable_from<DstT, SrcT> &&
    std::same_as<std::remove_cvref_t<DstT>, std::remove_cvref_t<SrcT>> &&
    std::is_empty_v<typename std::remove_cvref_t<DstT>::key_compare>;

template<class ContainerT>
concept has_unique_keys = requires(ContainerT& cont, std::ranges::range_value_t<ContainerT>&& value) {
    cont.insert(std::move(value)).second;
};

template<class DstT, class SrcT>
concept node_transferable_from =
    has_same_allocator_type<DstT, SrcT> &&
    node_family_transferable_from<DstT, SrcT> &&
    (
        has_stateless_allocator<DstT> ||
        // When we merge containers with stateful allocator, it is
        // UB unless `dst.get_allocator() == src.get_allocator()`.
        // Which means we must ensure they *always* have well-defined
        // element-wise operation available for fallback.
        element_wise_transferable_from<DstT, SrcT>
    );

struct transfer_from_fn
{
    template<class DstT, class SrcT>
        requires
            (!node_transferable_from<DstT, SrcT>) &&
            (flat_transferable_from<DstT, SrcT> || element_wise_transferable_from<DstT, SrcT>)
    static constexpr void operator()(DstT& dst, SrcT&& src)
    {
        assert(
            static_cast<void const*>(std::addressof(src)) != static_cast<void const*>(std::addressof(dst)) &&
            "self-transfer cannot be defined as a generic operation, even though some "
            "containers define it"
        );

        if constexpr (flat_sorted_transferable_from<DstT, SrcT>) {
            auto extracted = std::move(src).extract();
            auto const comp = dst.key_comp();
            auto const is_equivalent = [&comp](auto const& a, auto const& b) { return !comp(a, b) && !comp(b, a); };

            if constexpr (requires { extracted.keys; extracted.values; }) {
                if constexpr (
                    keeps_allocator_on_move_assignment<decltype(extracted.keys)> &&
                    keeps_allocator_on_move_assignment<decltype(extracted.values)>
                ) {
                    if (std::ranges::empty(dst)) {
                        dst.replace(std::move(extracted.keys), std::move(extracted.values));
                        return;
                    }
                }
                auto merged = std::move(dst).extract();
                auto const middle = std::ranges::ssize(merged.keys);
                append_range(merged.keys, extracted.keys | std::views::as_rvalue);
                append_range(merged.values, extracted.values | std::views::as_rvalue);

                auto zipped = std::views::zip(merged.keys, merged.values);
                auto const key = [](auto const& element) noexcept -> auto const& { return std::get<0>(element); };
                std::ranges::inplace_merge(zipped, std::ranges::begin(zipped) + middle, comp, key);
                if constexpr (has_unique_keys<std::remove_cvref_t<DstT>>) {
                    auto const unique_end = std::ranges::begin(std::ranges::unique(zipped, is_equivalent, key));
                    auto const size = unique_end - std::ranges::begin(zipped);
                    merged.keys.erase(std::ranges::begin(merged.keys) + size, std::ranges::end(merged.keys));
                    merged.values.erase(std::ranges::begin(merged.values) + size, std::ranges::end(merged.values));
                }
                dst.replace(std::move(merged.keys), std::move(merged.values));

            } else {
                if constexpr (keeps_allocator_on_move_assignment<decltype(extracted)>) {
                    if (std::ranges::empty(dst)) {
                        dst.replace(std::move(extracted));
                        return;
                    }
                }
                auto merged = std::move(dst).extract();
                auto const middle = std::ranges::ssize(merged);
                append_range(merged, extracted | std::views::as_rvalue);

                std::ranges::inplace_merge(merged, std::ranges::begin(merged) + middle, comp);
                if constexpr (has_unique_keys<std::remove_cvref_t<DstT>>) {
                    auto const removed = std::ranges::unique(merged, is_equivalent);
                    merged.erase(std::ranges::begin(removed), std::ranges::end(removed));
                }
                dst.replace(std::move(merged));
            }

        } else if constexpr (flat_transferable_from<DstT, SrcT>) {
            auto extracted = std::move(src).extract();
            append_range(dst, detail::moved_elements(extracted));

        } else {
            append_range(dst, std::forward<SrcT>(src) | std::views::as_rvalue);
        }
    }

    template<class DstT, class SrcT>
        requires node_transferable_from<DstT, SrcT>
    static constexpr void operator()(DstT& dst, SrcT&& src)
    {
        assert(
            static_cast<void const*>(std::addressof(src)) != static_cast<void const*>(std::addressof(dst)) &&
            "self-transfer cannot be defined as a generic operation, even though some "
            "containers define it"
        );

        if constexpr (!has_stateless_allocator<DstT>) {
            if (dst.get_allocator() != src.get_allocator()) {
                append_range(dst, std::forward<SrcT>(src) | std::views::as_rvalue);
                return;
            }
        }

        if constexpr (has_splice<DstT, SrcT>) {
            dst.splice(std::ranges::end(dst), std::forward<SrcT>(src));

        } else if constexpr (has_merge<DstT, SrcT>) {
            dst.merge(std::forward<SrcT>(src));

        } else {
            static_assert(false);
        }
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::transfer_from_fn transfer_from{};


} // iris::container

namespace iris::container::dummy {

template<class K, class V>
struct mapping_container
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;

    mapping_container() = default;
    mapping_container(mapping_container const&) = delete;
    mapping_container(mapping_container&&) = delete;
    mapping_container& operator=(mapping_container const&) = delete;
    mapping_container& operator=(mapping_container&&) = delete;

    void emplace(K&&, V&&);
};

template<class K, class V>
struct unique_mapping_container
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;

    unique_mapping_container() = default;
    unique_mapping_container(unique_mapping_container const&) = delete;
    unique_mapping_container(unique_mapping_container&&) = delete;
    unique_mapping_container& operator=(unique_mapping_container const&) = delete;
    unique_mapping_container& operator=(unique_mapping_container&&) = delete;

    void emplace(K&&, V&&);
    std::pair<std::pair<K, V> const*, bool> try_emplace(K&&, V&&);
    std::pair<std::pair<K, V> const*, bool> insert_or_assign(K&&, V&&);
};

template<class T>
struct growable_array
{
    T const* begin() const;
    T const* end() const;
    void emplace_back(T&&);
    void emplace_back() requires std::default_initializable<T>;
};

template<class T>
using fixed_array = T[1];

} // iris::container::dummy

#endif

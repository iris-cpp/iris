#ifndef IRIS_ZZ_ALLOY_TUPLE_HPP
#define IRIS_ZZ_ALLOY_TUPLE_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef IRIS_USE_PREPROCESSED
#define IRIS_USE_PREPROCESSED 1
#endif

#if IRIS_USE_PREPROCESSED
#include <iris/alloy/detail/preprocessed/tuple_impl.hpp>
#else
#include <iris/alloy/detail/tuple_impl.hpp>
#endif

#include <iris/alloy/detail/integer_seq_transform.hpp>
#include <iris/alloy/detail/tuple_comparison.hpp>

#include <type_traits>
#include <utility>

#include <cstddef>

namespace iris::alloy {

namespace detail {

template<class... Ts>
struct type_list;

template<class IndexSeq, class UTuple, class... Ts>
struct tuple_traits_impl;

template<std::size_t... Is, class UTuple, class... Ts>
struct tuple_traits_impl<std::index_sequence<Is...>, UTuple, Ts...>
{
    static constexpr bool all_convertible = std::conjunction_v<std::is_convertible<tuple_get_t<Is, UTuple>, Ts>...>;
    static constexpr bool all_constructible = std::conjunction_v<std::is_constructible<Ts, tuple_get_t<Is, UTuple>>...>;
    static constexpr bool all_nothrow_constructible = std::conjunction_v<std::is_nothrow_constructible<Ts, tuple_get_t<Is, UTuple>>...>;
    static constexpr bool all_assignable = std::conjunction_v<std::is_assignable<Ts&, tuple_get_t<Is, UTuple>>...>;
    static constexpr bool all_nothrow_assignable = std::conjunction_v<std::is_nothrow_assignable<Ts&, tuple_get_t<Is, UTuple>>...>;
    static constexpr bool all_nothrow_gettable = std::conjunction_v<is_nothrow_gettable<Is, UTuple>...>;
#if __cpp_lib_reference_from_temporary >= 202202L
    static constexpr bool any_reference_constructs_from_temporary = std::disjunction_v<std::reference_constructs_from_temporary<Ts, tuple_get_t<Is, UTuple>>...>;
#endif
};

template<class UTuple, class... Ts>
struct tuple_traits : tuple_traits_impl<std::index_sequence_for<Ts...>, UTuple, Ts...> {};

template<class UTuple, class... Ts>
struct tuple_one_element_is_constructible_from_tuple
    : std::bool_constant<(sizeof...(Ts) == 1) &&
                         (std::is_convertible_v<UTuple, IRIS_CORE_PACK_INDEXING(0, Ts...)> || std::is_constructible_v<IRIS_CORE_PACK_INDEXING(0, Ts...), UTuple>)>
{};

template<class UTuple, class... Ts>
inline constexpr bool tuple_one_element_is_constructible_from_tuple_v = tuple_one_element_is_constructible_from_tuple<UTuple, Ts...>::value;

} // detail

template<class... Ts>
class tuple : public detail::tuple_impl<Ts...>
{
private:
    static_assert(!std::disjunction_v<std::is_rvalue_reference<Ts>...>, "alloy::tuple must not be instantiated with rvalue reference type");
    using base_type = detail::tuple_impl<Ts...>;

    template<class... Us>
    static constexpr bool disambiguating_constraint = []() {
        if constexpr (sizeof...(Ts) == 1) {
            return !std::is_same_v<std::remove_cvref_t<IRIS_CORE_PACK_INDEXING(0, Us...)>, tuple>;
        } else {
            return true;
        }
    }();

    struct construct_t {};

    static constexpr construct_t construct{};

    template<std::size_t... Is, class UTuple>
    constexpr explicit tuple(construct_t, std::index_sequence<Is...>, UTuple&& other)
        noexcept(detail::tuple_traits<UTuple, Ts...>::all_nothrow_gettable && detail::tuple_traits<UTuple, Ts...>::all_nothrow_constructible)
        : base_type(alloy::get<Is>(static_cast<UTuple>(other))...)
    {}

public:
    tuple() = default;

    tuple(tuple const&) = default;

    tuple(tuple&&)
        requires std::conjunction_v<std::is_move_constructible<Ts>...>
    = default;

    constexpr explicit tuple(value_initialize_t vi)
        noexcept(std::conjunction_v<std::is_nothrow_default_constructible<Ts>...>)
        : base_type(vi) {}

    constexpr explicit(!std::conjunction_v<std::is_convertible<Ts const&, Ts>...>) tuple(Ts const&... ts)
        noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
        requires requires {
            requires (sizeof...(Ts) > 0);
            requires std::conjunction_v<std::is_copy_constructible<Ts>...>;
        }
        : base_type(ts...)
    {}

    template<class... Us>
        requires requires {
            requires (sizeof...(Ts) == sizeof...(Us));
            requires disambiguating_constraint<Us...>;
            requires std::conjunction_v<std::is_constructible<Ts, Us>...>;
        }
#if __cpp_lib_reference_from_temporary >= 202202L
    && (!(std::reference_constructs_from_temporary_v<Ts, Us&&> || ...))
#endif
    constexpr explicit(!std::conjunction_v<std::is_convertible<Us, Ts>...>) tuple(Us&&... us)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<Ts, Us>...>)
        : base_type(static_cast<Us&&>(us)...)
    {}

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...>&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...>&, Ts...>);
        }
#if __cpp_lib_reference_from_temporary >= 202202L
        && (!detail::tuple_traits<tuple<Us...>&, Ts...>::any_reference_constructs_from_temporary)
#endif
    constexpr explicit(!detail::tuple_traits<tuple<Us...>&, Ts...>::all_convertible) tuple(tuple<Us...>& other)
        noexcept(detail::tuple_traits<tuple<Us...>&, Ts...>::all_nothrow_constructible)
        : base_type(static_cast<detail::tuple_impl<Us...>&>(other))
    {}

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...> const&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...> const&, Ts...>);
        }
#if __cpp_lib_reference_from_temporary >= 202202L
        && (!detail::tuple_traits<tuple<Us...> const&, Ts...>::any_reference_constructs_from_temporary)
#endif
    constexpr explicit(!detail::tuple_traits<tuple<Us...> const&, Ts...>::all_convertible) tuple(tuple<Us...> const& other)
        noexcept(detail::tuple_traits<tuple<Us...> const&, Ts...>::all_nothrow_constructible)
        : base_type(static_cast<detail::tuple_impl<Us...> const&>(other))
    {}

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...>&&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...>&&, Ts...>);
        }
#if __cpp_lib_reference_from_temporary >= 202202L
        && (!detail::tuple_traits<tuple<Us...>&&, Ts...>::any_reference_constructs_from_temporary)
#endif
    constexpr explicit(!detail::tuple_traits<tuple<Us...>&&, Ts...>::all_convertible) tuple(tuple<Us...>&& other)
        noexcept(detail::tuple_traits<tuple<Us...>&&, Ts...>::all_nothrow_constructible)
        : base_type(static_cast<detail::tuple_impl<Us...>&&>(other))
    {}

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...> const&&, Ts...>);
        }
#if __cpp_lib_reference_from_temporary >= 202202L
        && (!detail::tuple_traits<tuple<Us...> const&&, Ts...>::any_reference_constructs_from_temporary)
#endif
    constexpr explicit(!detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_convertible) tuple(tuple<Us...> const&& other)
        noexcept(detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_nothrow_constructible)
        : base_type(static_cast<detail::tuple_impl<Us...> const&&>(other))
    {}

    template<TupleLike UTuple>
        requires requires {
            requires !std::is_same_v<std::remove_cvref_t<UTuple>, tuple>;
            requires sizeof...(Ts) == tuple_size_v<std::remove_cvref_t<UTuple>>;
            requires detail::tuple_traits<UTuple, Ts...>::all_constructible;
            requires !detail::tuple_one_element_is_constructible_from_tuple_v<UTuple, Ts...>;
        }
#if __cpp_lib_reference_from_temporary >= 202202L
        && (!detail::tuple_traits<UTuple, Ts...>::any_reference_constructs_from_temporary)
#endif
    constexpr explicit(!detail::tuple_traits<UTuple, Ts...>::all_convertible) tuple(UTuple&& other)
        noexcept(detail::tuple_traits<UTuple, Ts...>::all_nothrow_gettable && detail::tuple_traits<UTuple, Ts...>::all_nothrow_constructible)
        : tuple(construct, std::make_index_sequence<tuple_size_v<std::remove_cvref_t<UTuple>>>{}, static_cast<UTuple>(other))
    {}

#if __cpp_lib_reference_from_temporary >= 202202L
    template<class... Us>
        requires requires {
            requires (sizeof...(Ts) == sizeof...(Us));
            requires disambiguating_constraint<Us...>;
            requires std::conjunction_v<std::is_constructible<Ts, Us>...>;
        }
    && (std::reference_constructs_from_temporary_v<Ts, Us&&> || ...)
    constexpr explicit(!std::conjunction_v<std::is_convertible<Us, Ts>...>) tuple(Us&&... us)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<Ts, Us>...>)
    = delete;

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...>&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...>&, Ts...>);
        }
        && detail::tuple_traits<tuple<Us...>&, Ts...>::any_reference_constructs_from_temporary
    constexpr explicit(!detail::tuple_traits<tuple<Us...>&, Ts...>::all_convertible) tuple(tuple<Us...>& other)
        noexcept(detail::tuple_traits<tuple<Us...>&, Ts...>::all_nothrow_constructible)
    = delete;

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...> const&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...> const&, Ts...>);
        }
        && detail::tuple_traits<tuple<Us...> const&, Ts...>::any_reference_constructs_from_temporary
    constexpr explicit(!detail::tuple_traits<tuple<Us...> const&, Ts...>::all_convertible) tuple(tuple<Us...> const& other)
        noexcept(detail::tuple_traits<tuple<Us...> const&, Ts...>::all_nothrow_constructible)
    = delete;

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...>&&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...>&&, Ts...>);
        }
        && detail::tuple_traits<tuple<Us...>&&, Ts...>::any_reference_constructs_from_temporary
    constexpr explicit(!detail::tuple_traits<tuple<Us...>&&, Ts...>::all_convertible) tuple(tuple<Us...>&& other)
        noexcept(detail::tuple_traits<tuple<Us...>&&, Ts...>::all_nothrow_constructible)
    = delete;

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires std::negation_v<std::conjunction<std::is_same<Ts, Us>...>>;
            requires detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_constructible;
            requires (!detail::tuple_one_element_is_constructible_from_tuple_v<tuple<Us...> const&&, Ts...>);
        }
        && detail::tuple_traits<tuple<Us...> const&&, Ts...>::any_reference_constructs_from_temporary
    constexpr explicit(!detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_convertible) tuple(tuple<Us...> const&& other)
        noexcept(detail::tuple_traits<tuple<Us...> const&&, Ts...>::all_nothrow_constructible)
    = delete;

    template<TupleLike UTuple>
        requires requires {
            requires !std::is_same_v<std::remove_cvref_t<UTuple>, tuple>;
            requires sizeof...(Ts) == tuple_size_v<std::remove_cvref_t<UTuple>>;
            requires detail::tuple_traits<UTuple, Ts...>::all_constructible;
            requires !detail::tuple_one_element_is_constructible_from_tuple_v<UTuple, Ts...>;
        }
        && detail::tuple_traits<UTuple, Ts...>::any_reference_constructs_from_temporary
    constexpr explicit(!detail::tuple_traits<UTuple, Ts...>::all_convertible) tuple(UTuple&& other)
        noexcept(detail::tuple_traits<UTuple, Ts...>::all_nothrow_gettable && detail::tuple_traits<UTuple, Ts...>::all_nothrow_constructible)
    = delete;
#endif

    constexpr tuple& operator=(tuple const& other)
        noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<Ts>...>)
    {
        base_type::operator=(other);
        return *this;

    }
    constexpr tuple& operator=(tuple&& other)
        noexcept(std::conjunction_v<std::is_nothrow_move_assignable<Ts>...>)
        requires std::conjunction_v<std::is_move_assignable<Ts>...>
    {
        base_type::operator=(static_cast<tuple&&>(other));
        return *this;
    }

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires detail::tuple_traits<tuple<Us...> const&, Ts...>::all_assignable;
        }
    constexpr tuple& operator=(tuple<Us...> const& other)
        noexcept(detail::tuple_traits<tuple<Us...> const&, Ts...>::all_nothrow_assignable)
    {
        base_type::operator=(static_cast<detail::tuple_impl<Us...> const&>(other));
        return *this;
    }

    template<class... Us>
        requires requires {
            requires sizeof...(Ts) == sizeof...(Us);
            requires detail::tuple_traits<tuple<Us...>&&, Ts...>::all_assignable;
        }
    constexpr tuple& operator=(tuple<Us...>&& other)
        noexcept(detail::tuple_traits<tuple<Us...>&&, Ts...>::all_nothrow_assignable)
    {
        base_type::operator=(static_cast<detail::tuple_impl<Us...> &&>(other));
        return *this;
    }

    template<TupleLike UTuple>
        requires requires {
            requires !std::is_same_v<std::remove_cvref_t<UTuple>, tuple>;
            requires sizeof...(Ts) == tuple_size_v<std::remove_cvref_t<UTuple>>;
            requires detail::tuple_traits<UTuple, Ts...>::all_assignable;
        }
    constexpr tuple& operator=(UTuple&& other)
        noexcept(detail::tuple_traits<UTuple, Ts...>::all_nothrow_assignable)
    {
        base_type::operator=(static_cast<UTuple>(other));
        return *this;
    }

    constexpr void swap(tuple& other) noexcept(std::conjunction_v<std::is_nothrow_swappable<Ts>...>)
    {
        base_type::swap(other);
    }

    template<std::size_t I>
    [[nodiscard]] constexpr tuple_element_t<I, tuple>& get() & noexcept
    {
        static_assert(I < sizeof...(Ts));
        return base_type::template get<I>();
    }

    template<std::size_t I>
    [[nodiscard]] constexpr tuple_element_t<I, tuple> const& get() const& noexcept
    {
        static_assert(I < sizeof...(Ts));
        return base_type::template get<I>();
    }

    template<std::size_t I>
    [[nodiscard]] constexpr tuple_element_t<I, tuple>&& get() && noexcept
    {
        static_assert(I < sizeof...(Ts));
        return std::move(*this).base_type::template get<I>();
    }

    template<std::size_t I>
    [[nodiscard]] constexpr tuple_element_t<I, tuple> const&& get() const&& noexcept
    {
        static_assert(I < sizeof...(Ts));
        return std::move(*this).base_type::template get<I>();
    }
};

template<class... Ts>
tuple(Ts...) -> tuple<Ts...>;

template<class... Ts>
    requires std::conjunction_v<std::is_nothrow_swappable<Ts>...>
constexpr void swap(tuple<Ts...>& a, tuple<Ts...>& b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

template<class... Ts, class... Us>
    requires detail::tuple_all_elements_have_equality_operator<tuple<Ts...>, tuple<Us...>>
constexpr bool operator==(tuple<Ts...> const& a, tuple<Us...> const& b)
    noexcept(detail::are_tuple_all_elements_nothrow_equality_comparable_v<tuple<Ts...>, tuple<Us...>>)
{
    return a.equal_to(b);
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t) noexcept
{
    static_assert(I < sizeof...(Ts));
    return t.template get<I>();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>> const& get(tuple<Ts...> const& t) noexcept
{
    static_assert(I < sizeof...(Ts));
    return t.template get<I>();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t) noexcept
{
    static_assert(I < sizeof...(Ts));
    return static_cast<tuple<Ts...>&&>(t).template get<I>();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr tuple_element_t<I, tuple<Ts...>> const&& get(tuple<Ts...> const&& t) noexcept
{
    static_assert(I < sizeof...(Ts));
    return static_cast<tuple<Ts...> const&&>(t).template get<I>();
}

} // iris::alloy

#endif

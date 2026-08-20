#ifndef IRIS_ZZ_ALLOY_DETAIL_TUPLE_IMPL_HPP
#define IRIS_ZZ_ALLOY_DETAIL_TUPLE_IMPL_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef IRIS_ALLOY_GENERATE_PREPROCESSED

#include <iris/config.hpp>

#include <iris/alloy/detail/tuple_comparison.hpp>

#include <iris/alloy/traits.hpp>

#include <iris/type_traits.hpp>

#include <type_traits>

#include <cstddef>

#endif

#include <iris/pp/cat.hpp>
#include <iris/pp/repeat.hpp>
#include <iris/pp/comma.hpp>
#include <iris/pp/if.hpp>

#define IRIS_ALLOY_EXPR_IF(cond, expr) IRIS_PP_IF(cond, expr, )

namespace iris::alloy {

template<class... Ts>
class tuple;

template<class... Ts, class... Us>
    requires detail::tuple_all_elements_have_equality_operator<tuple<Ts...>, tuple<Us...>>
constexpr bool operator==(tuple<Ts...> const&, tuple<Us...> const&)
    noexcept(detail::are_tuple_all_elements_nothrow_equality_comparable_v<tuple<Ts...>, tuple<Us...>>);

namespace detail {

template<class... Ts>
class tuple_impl;

template<>
class tuple_impl<>
{
    template<class... Us, class... Vs>
        requires tuple_all_elements_have_equality_operator<tuple<Us...>, tuple<Vs...>>
    friend constexpr bool alloy::operator==(tuple<Us...> const& a, tuple<Vs...> const& b)
        noexcept(detail::are_tuple_all_elements_nothrow_equality_comparable_v<tuple<Us...>, tuple<Vs...>>);

private:
    constexpr bool equal_to(tuple_impl const&) const noexcept { return true; }

public:
    tuple_impl() = default;

    tuple_impl(tuple_impl const&) = default;

    tuple_impl(tuple_impl&&) = default;

    constexpr tuple_impl(value_initialize_t) noexcept {}
};

#define IRIS_ALLOY_TUPLE_LIMIT 32

#define IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1 T
#define IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2 U
#define IRIS_ALLOY_DETAIL_FUNCTION_PARAM_1 t
#define IRIS_ALLOY_DETAIL_FUNCTION_PARAM_2 u
#define IRIS_ALLOY_DETAIL_MEMBER_PREFIX _

#define IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS(n, name) IRIS_PP_COMMA_IF(n) class IRIS_PP_CAT(name, n)
#define IRIS_ALLOY_DETAIL_ARGS(n, name) IRIS_PP_COMMA_IF(n) IRIS_PP_CAT(name, n)

#define IRIS_ALLOY_DETAIL_MEM_DEFS(n, data) \
    IRIS_NO_UNIQUE_ADDRESS IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n) IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n);

#define IRIS_ALLOY_DETAIL_FWD_PARAMS(n, data) \
    IRIS_PP_COMMA_IF(n) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n) && IRIS_PP_CAT(IRIS_ALLOY_DETAIL_FUNCTION_PARAM_2, n)

#define IRIS_ALLOY_DETAIL_FWD_INITS(n, data) \
    IRIS_PP_COMMA_IF(n) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, \
                 n)(static_cast<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n)>(IRIS_PP_CAT(IRIS_ALLOY_DETAIL_FUNCTION_PARAM_2, n)))

#define IRIS_ALLOY_DETAIL_INITS(n, other) \
    IRIS_PP_COMMA_IF(n) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n)(other.IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n))

#define IRIS_ALLOY_DETAIL_NOTHROW_DEFAULT_CONSTRUCTIBLE(n, data) \
    IRIS_PP_COMMA_IF(n) std::is_nothrow_default_constructible<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n)>

#define IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE(n, suffix) \
    IRIS_PP_COMMA_IF(n) \
    std::is_nothrow_constructible<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n), \
                                  IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n) suffix>

#define IRIS_ALLOY_DETAIL_NOTHROW_COPY_ASSIGNABLE(n, data) \
    IRIS_PP_COMMA_IF(n) std::is_nothrow_copy_assignable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n)>

#define IRIS_ALLOY_DETAIL_NOTHROW_MOVE_ASSIGNABLE(n, data) \
    IRIS_PP_COMMA_IF(n) std::is_nothrow_move_assignable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n)>

#define IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE(n, suffix) \
    IRIS_PP_COMMA_IF(n) \
    std::is_nothrow_assignable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n) &, \
                               IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n) suffix>

#define IRIS_ALLOY_DETAIL_VALUE_INITS(n, data) \
    IRIS_PP_COMMA_IF(n) IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n) {}

#define IRIS_ALLOY_DETAIL_ASSIGN(n, other) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n) = other.IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n);

#define IRIS_ALLOY_DETAIL_ASSIGN_ASSIGN(n, data) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n) = \
        static_cast<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n)>(IRIS_PP_CAT(IRIS_ALLOY_DETAIL_FUNCTION_PARAM_2, n));

#define IRIS_ALLOY_DETAIL_ASSIGN_GET(n, other) \
    IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n) = alloy::get<n>(static_cast<decltype(other)>(other));

#define IRIS_ALLOY_DETAIL_SWAP(n, other) \
    swap(IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n), other.IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n));

#define IRIS_ALLOY_DETAIL_SWAPPABLE(n, data) IRIS_PP_COMMA_IF(n) std::is_swappable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n)>

#define IRIS_ALLOY_DETAIL_NOTHROW_SWAPPABLE(n, data) \
    IRIS_PP_COMMA_IF(n) std::is_nothrow_swappable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n)>

#define IRIS_ALLOY_DETAIL_LVALUE_GET(n, data) \
    IRIS_ALLOY_EXPR_IF(n, else) if constexpr (I == n) return IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n);

#define IRIS_ALLOY_DETAIL_FORWARDING_GET(n, const_) \
    IRIS_ALLOY_EXPR_IF(n, else) \
    if constexpr (I == n) return static_cast<decltype(IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n)) const_&&>( \
        IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n));

#define IRIS_ALLOY_DETAIL_EQUAL_TO(n, other) \
    IRIS_ALLOY_EXPR_IF(n, &&) IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n) == other.IRIS_PP_CAT(IRIS_ALLOY_DETAIL_MEMBER_PREFIX, n)

#define IRIS_ALLOY_DETAIL_NOTHROW_EQUALITY_COMPARABLE(n, data) \
    IRIS_PP_COMMA_IF(n) \
    is_nothrow_equality_comparable<IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1, n), IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, n)>

#define IRIS_ALLOY_DETAIL_TUPLE_IMPL_DEF(n, data) \
    template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)> \
    class tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)> \
    { \
        template<class... IRIS_PP_CAT(IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2, s)> \
        friend class tuple_impl; \
\
        template<class... Us, class... Vs> \
            requires tuple_all_elements_have_equality_operator<tuple<Us...>, tuple<Vs...>> \
        friend constexpr bool alloy::operator==(tuple<Us...> const&, tuple<Vs...> const&) \
            noexcept(detail::are_tuple_all_elements_nothrow_equality_comparable_v<tuple<Us...>, tuple<Vs...>>); \
\
    private: \
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr bool equal_to(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const& other) const \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_EQUALITY_COMPARABLE, )>) \
        { \
            return IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_EQUAL_TO, other); \
        } \
\
    public: \
        IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_MEM_DEFS, ) \
\
        explicit tuple_impl() = default; \
\
        explicit tuple_impl(tuple_impl const&) = default; \
\
        explicit tuple_impl(tuple_impl&&) = default; \
\
        constexpr explicit tuple_impl(value_initialize_t) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_DEFAULT_CONSTRUCTIBLE, )>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_VALUE_INITS, ) \
        { \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr explicit tuple_impl(IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_FWD_PARAMS, )) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, )>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_FWD_INITS, ) \
        { \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr explicit tuple_impl(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)>& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, &)>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_INITS, other) \
        { \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr explicit tuple_impl(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, const&)>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_INITS, other) \
        { \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr explicit tuple_impl(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)>&& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, &&)>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_INITS, static_cast<decltype(other)>(other)) \
        { \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr explicit tuple_impl( \
            tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const&& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, const&&)>) \
            : IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_INITS, static_cast<decltype(other)>(other)) \
        { \
        } \
\
        constexpr tuple_impl& operator=(tuple_impl const& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_COPY_ASSIGNABLE, )>) \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ASSIGN, other) \
            return *this; \
        } \
\
        constexpr tuple_impl& operator=(tuple_impl&& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_MOVE_ASSIGNABLE, )>) \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ASSIGN, static_cast<decltype(other)>(other)) \
            return *this; \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr tuple_impl& \
        operator=(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE, const&)>) \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ASSIGN, other) \
            return *this; \
        } \
\
        template<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> \
        constexpr tuple_impl& operator=(tuple_impl<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)>&& other) \
            noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE, &&)>) \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ASSIGN, static_cast<decltype(other)>(other)) \
            return *this; \
        } \
\
        template<class UTuple> \
        constexpr tuple_impl& operator=(UTuple&& other) \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ASSIGN_GET, other) \
            return *this; \
        } \
\
        constexpr void swap(tuple_impl& other) noexcept(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_NOTHROW_SWAPPABLE, )>) \
        { \
            static_assert(std::conjunction_v<IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_SWAPPABLE, )>); \
            using std::swap; \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_SWAP, other) \
        } \
\
        template<std::size_t I> \
        constexpr pack_indexing_t<I, IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)>& get() & noexcept \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_LVALUE_GET, ) \
        } \
\
        template<std::size_t I> \
        constexpr pack_indexing_t<I, IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)> const& \
        get() const& noexcept \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_LVALUE_GET, ) \
        } \
\
        template<std::size_t I> \
        constexpr pack_indexing_t<I, IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)>&& get() && noexcept \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_FORWARDING_GET, ) \
        } \
\
        template<std::size_t I> \
        constexpr pack_indexing_t<I, IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1)> const&& \
        get() const&& noexcept \
        { \
            IRIS_PP_REPEAT(n, IRIS_ALLOY_DETAIL_FORWARDING_GET, const) \
        } \
    };

IRIS_PP_REPEAT_FROM_TO(1, IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TUPLE_IMPL_DEF, )

template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1), class... Ts>
class tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1), Ts...>
{
    template<class... Us>
    friend class tuple_impl;

    template<class... Us, class... Vs>
        requires tuple_all_elements_have_equality_operator<tuple<Us...>, tuple<Vs...>>
    friend constexpr bool alloy::operator==(tuple<Us...> const& a, tuple<Vs...> const& b)
        noexcept(detail::are_tuple_all_elements_nothrow_equality_comparable_v<tuple<Us...>, tuple<Vs...>>);

private:
    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr void assign(IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_FWD_PARAMS, ), Us&&... us)
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ASSIGN_ASSIGN, )
        rest.assign(static_cast<Us>(us)...);
    }

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)>
    constexpr bool equal_to(tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS,
                                                       IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const& other) const
         noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_EQUALITY_COMPARABLE, )>) \
    {
        return IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_EQUAL_TO, other) && rest == other.rest;
    }

public:
    IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_MEM_DEFS, )
    IRIS_NO_UNIQUE_ADDRESS tuple_impl<Ts...> rest;

    explicit tuple_impl() = default;

    explicit tuple_impl(tuple_impl const&) = default;

    explicit tuple_impl(tuple_impl&&) = default;

    constexpr explicit tuple_impl(value_initialize_t vi)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_DEFAULT_CONSTRUCTIBLE, ),
                                    std::is_nothrow_default_constructible<Ts>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_VALUE_INITS, ), rest(vi)
    {}

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
        requires (sizeof...(Ts) == sizeof...(Us))
    constexpr explicit tuple_impl(IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_FWD_PARAMS, ), Us&&... us)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, ),
                                    std::is_nothrow_constructible<Ts, Us>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_FWD_INITS, ), rest(static_cast<Us&&>(us)...)
    {}

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr explicit tuple_impl(
        tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2), Us...>& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, &),
                                    std::is_nothrow_constructible<Ts, Us&>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_INITS, other), rest(other.rest)
    {}

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr explicit tuple_impl(
        tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2), Us...> const&
            other) noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, const&),
                                               std::is_nothrow_constructible<Ts, Us const&>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_INITS, other), rest(other.rest)
    {}

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr explicit tuple_impl(
        tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2), Us...>&& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, &&),
                                    std::is_nothrow_constructible<Ts, Us&&>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_INITS, static_cast<decltype(other)>(other)),
          rest(static_cast<decltype(other)>(other).rest)
    {}

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr explicit tuple_impl(
        tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2), Us...> const&&
            other) noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE, const&&),
                                               std::is_nothrow_constructible<Ts, Us const&&>...>)
        : IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_INITS, static_cast<decltype(other)>(other)),
          rest(static_cast<decltype(other)>(other).rest)
    {}

    constexpr tuple_impl& operator=(tuple_impl const& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_COPY_ASSIGNABLE, ),
                                    std::is_nothrow_copy_assignable<Ts>...>)
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ASSIGN, other)
        rest = other.rest;
        return *this;
    }

    constexpr tuple_impl& operator=(tuple_impl&& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_MOVE_ASSIGNABLE, ),
                                    std::is_nothrow_move_assignable<Ts>...>)
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ASSIGN, static_cast<decltype(other)>(other))
        rest = static_cast<decltype(other)>(other).rest;
        return *this;
    }

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr tuple_impl& operator=(
        tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)> const& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE, &&),
                                    std::is_nothrow_assignable<Ts&, Us const&>...>)
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ASSIGN, other)
        rest = other.rest;
        return *this;
    }

    template<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2),
             class... Us>
    constexpr tuple_impl&
    operator=(tuple_impl<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2)>&& other)
        noexcept(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE, &&),
                                    std::is_nothrow_assignable<Ts&, Us&&>...>)
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ASSIGN, static_cast<decltype(other)>(other))
        rest = static_cast<decltype(other)>(other).rest;
        return *this;
    }

    template<class UTuple>
    constexpr tuple_impl& operator=(UTuple&& other)
    {
        [&, this]<std::size_t... Is>(std::index_sequence<Is...>) { assign(alloy::get<Is>(static_cast<UTuple>(other))...); }(std::index_sequence_for<Ts...>{});
        return *this;
    }

    constexpr void swap(tuple_impl& other) noexcept(
        std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_NOTHROW_SWAPPABLE, ), std::is_nothrow_swappable<Ts>...>)
    {
        static_assert(std::conjunction_v<IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_SWAPPABLE, ), std::is_swappable<Ts>...>);
        using std::swap;
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_SWAP, other)
        rest.swap(other.rest);
    }

    template<std::size_t I>
    constexpr pack_indexing_t<
        I,
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1),
        Ts...
    >&
    get() & noexcept
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_LVALUE_GET, )
        else return rest.template get<I - IRIS_ALLOY_TUPLE_LIMIT>();
    }

    template<std::size_t I>
    constexpr pack_indexing_t<
        I,
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1),
        Ts...
    > const&
    get() const& noexcept
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_LVALUE_GET, )
        else return rest.template get<I - IRIS_ALLOY_TUPLE_LIMIT>();
    }

    template<std::size_t I>
    constexpr pack_indexing_t<
        I,
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1),
        Ts...
    >&&
    get() && noexcept
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_FORWARDING_GET, )
        else return std::move(rest).template get<I - IRIS_ALLOY_TUPLE_LIMIT>();
    }

    template<std::size_t I>
    constexpr pack_indexing_t<
        I,
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_ARGS, IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1),
        Ts...
    > const&&
    get() const&& noexcept
    {
        IRIS_PP_REPEAT(IRIS_ALLOY_TUPLE_LIMIT, IRIS_ALLOY_DETAIL_FORWARDING_GET, const)
        else return std::move(rest).template get<I - IRIS_ALLOY_TUPLE_LIMIT>();
    }
};

#undef IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_1
#undef IRIS_ALLOY_DETAIL_TEMPLATE_PARAM_2
#undef IRIS_ALLOY_DETAIL_FUNCTION_PARAM_1
#undef IRIS_ALLOY_DETAIL_FUNCTION_PARAM_2
#undef IRIS_ALLOY_DETAIL_MEMBER_PREFIX
#undef IRIS_ALLOY_DETAIL_TEMPLATE_PARAMS
#undef IRIS_ALLOY_DETAIL_ARGS
#undef IRIS_ALLOY_DETAIL_MEM_DEFS
#undef IRIS_ALLOY_DETAIL_FWD_PARAMS
#undef IRIS_ALLOY_DETAIL_FWD_INITS
#undef IRIS_ALLOY_DETAIL_INITS
#undef IRIS_ALLOY_DETAIL_ASSIGN
#undef IRIS_ALLOY_DETAIL_ASSIGN_GET
#undef IRIS_ALLOY_DETAIL_NOTHROW_DEFAULT_CONSTRUCTIBLE
#undef IRIS_ALLOY_DETAIL_NOTHROW_CONSTRUCTIBLE
#undef IRIS_ALLOY_DETAIL_NOTHROW_COPY_ASSIGNABLE
#undef IRIS_ALLOY_DETAIL_NOTHROW_MOVE_ASSIGNABLE
#undef IRIS_ALLOY_DETAIL_NOTHROW_ASSIGNABLE
#undef IRIS_ALLOY_DETAIL_VALUE_INITS
#undef IRIS_ALLOY_DETAIL_LVALUE_GET
#undef IRIS_ALLOY_DETAIL_TUPLE_IMPL_DEF

} // detail

} // iris::alloy

#endif

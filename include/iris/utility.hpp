#ifndef IRIS_ZZ_UTILITY_HPP
#define IRIS_ZZ_UTILITY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <utility> // IWYU pragma: export
#include <type_traits>
#include <array>
#include <functional> // std::invoke

#include <cassert>

namespace iris {

namespace detail::adl_get {

using std::get;

template<std::size_t I, class T>
concept gettable_impl = requires(T&& t) {
    get<I>(std::forward<T>(t));
};

} // detail::adl_get

template<std::size_t I, class T>
concept gettable = detail::adl_get::gettable_impl<I, T>;


namespace detail {

template<class F>
using invoke_with_index_result_t = decltype(
    std::invoke(std::declval<F>(), std::declval<std::integral_constant<std::size_t, 0>>())
);

template<std::size_t I, class F>
struct invoke_with_index_noexcept_impl
{
    static constexpr bool value = noexcept(
        std::invoke(std::declval<F>(), std::declval<std::integral_constant<std::size_t, I>>())
    );
};

template<class Seq, class F>
struct invoke_with_index_noexcept_impl_dispatch;

template<class F, std::size_t... Is>
struct invoke_with_index_noexcept_impl_dispatch<std::index_sequence<Is...>, F>
    : std::conjunction<invoke_with_index_noexcept_impl<Is, F>...>
{};

template<std::size_t N, class F>
constexpr bool invoke_with_index_all_noexcept = invoke_with_index_noexcept_impl_dispatch<
    std::make_index_sequence<N>,
    F
>::value;

template<int Strategy>
struct invoke_with_index_impl;

template<class F, class Seq>
struct invoke_with_index_table;

template<class F, std::size_t... Is>
struct invoke_with_index_table<F, std::index_sequence<Is...>>
{
    static constexpr bool all_noexcept = invoke_with_index_all_noexcept<sizeof...(Is), F>;

    static constexpr auto table = std::array{
        +[](F&& f) static constexpr noexcept(all_noexcept) -> invoke_with_index_result_t<F> {
            return std::invoke(std::forward<F>(f), std::integral_constant<std::size_t, Is>{});
        }...
    };
};

template<>
struct invoke_with_index_impl<-1>
{
    template<std::size_t N, class F>
    [[nodiscard]] IRIS_FORCEINLINE static constexpr invoke_with_index_result_t<F>
    apply(std::size_t const i, F&& f)
        noexcept(invoke_with_index_all_noexcept<N, F>)
    {
        assert(i < N);
        return std::invoke(
            invoke_with_index_table<F, std::make_index_sequence<N>>::table[i],
            std::forward<F>(f)
        );
    }
};

#define IRIS_INVOKE_WITH_INDEX_CASE(n) \
    case (n): \
        if constexpr ((n) < N) { \
            return static_cast<F&&>(f)(std::integral_constant<std::size_t, (n)>{}); \
        } else std::unreachable(); [[fallthrough]]

#define IRIS_INVOKE_WITH_INDEX_DEF(strategy) \
    template<> \
    struct invoke_with_index_impl<(strategy)> \
    { \
        template<std::size_t N, class F> \
        [[nodiscard]] IRIS_FORCEINLINE static constexpr invoke_with_index_result_t<F> \
        apply(std::size_t const i, [[maybe_unused]] F&& f) \
            noexcept(invoke_with_index_all_noexcept<N, F>) \
        { \
            switch (i) { \
            IRIS_INVOKE_WITH_INDEX_CASES_ ## strategy (IRIS_INVOKE_WITH_INDEX_CASE, 0); \
            default: std::unreachable(); \
            } \
        } \
    };


// We choose not to introduce the "256" branch, since we usually have a
// limited amount of statically sized array (or tuple) to "dispatch",
// in contrast to `variant` which ordinarily bloats the combination
// count for multi-visits.
template<std::size_t N>
constexpr int invoke_with_index_strategy =
    N <= 4 ? 0 :
    N <= 16 ? 1 :
    N <= 64 ? 2 :
    -1;

#define IRIS_INVOKE_WITH_INDEX_CASES_0(def, ofs) \
    def(ofs); \
    def((ofs) + 1); \
    def((ofs) + 2); \
    def((ofs) + 3)

#define IRIS_INVOKE_WITH_INDEX_CASES_1(def, ofs) \
    IRIS_INVOKE_WITH_INDEX_CASES_0(def, ofs); \
    IRIS_INVOKE_WITH_INDEX_CASES_0(def, (ofs) + 4); \
    IRIS_INVOKE_WITH_INDEX_CASES_0(def, (ofs) + 8); \
    IRIS_INVOKE_WITH_INDEX_CASES_0(def, (ofs) + 12)

#define IRIS_INVOKE_WITH_INDEX_CASES_2(def, ofs) \
    IRIS_INVOKE_WITH_INDEX_CASES_1(def, ofs); \
    IRIS_INVOKE_WITH_INDEX_CASES_1(def, (ofs) + 16); \
    IRIS_INVOKE_WITH_INDEX_CASES_1(def, (ofs) + 32); \
    IRIS_INVOKE_WITH_INDEX_CASES_1(def, (ofs) + 48)

IRIS_INVOKE_WITH_INDEX_DEF(0);
IRIS_INVOKE_WITH_INDEX_DEF(1);
IRIS_INVOKE_WITH_INDEX_DEF(2);

#undef IRIS_INVOKE_WITH_INDEX_CASES_0
#undef IRIS_INVOKE_WITH_INDEX_CASES_1
#undef IRIS_INVOKE_WITH_INDEX_CASES_2

#undef IRIS_INVOKE_WITH_INDEX_CASE
#undef IRIS_INVOKE_WITH_INDEX_DEF

} // detail

// Invokes `F` with static index `std::integral_constant<std::size_t, I>` deduced by runtime index `i`.
template<std::size_t N, class F>
IRIS_FORCEINLINE constexpr detail::invoke_with_index_result_t<F>
invoke_with_index(std::size_t i, F&& f)
    noexcept(detail::invoke_with_index_all_noexcept<N, F>)
{
    return detail::invoke_with_index_impl<detail::invoke_with_index_strategy<N>>::template apply<N>(
        i, std::forward<F>(f)
    );
}

} // iris

#endif

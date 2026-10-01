#ifndef IRIS_ZZ_TYPE_TRAITS_HPP
#define IRIS_ZZ_TYPE_TRAITS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/bits/is_function_object.hpp>  // IWYU pragma: export
#include <iris/bits/specialization_of.hpp>  // IWYU pragma: export

#include <concepts>
#include <initializer_list>
#include <type_traits> // IWYU pragma: export
#include <utility>

#include <cstddef>

namespace iris {

template<class T>
concept signed_numeric_integral =
    std::signed_integral<T> &&
    !std::same_as<std::remove_cv_t<T>, char> &&
    !std::same_as<std::remove_cv_t<T>, wchar_t>;

template<class T>
concept unsigned_numeric_integral =
    std::unsigned_integral<T> &&
    !std::same_as<std::remove_cv_t<T>, bool> &&
    !std::same_as<std::remove_cv_t<T>, char> &&
    !std::same_as<std::remove_cv_t<T>, wchar_t> &&
    !std::same_as<std::remove_cv_t<T>, char8_t> &&
    !std::same_as<std::remove_cv_t<T>, char16_t> &&
    !std::same_as<std::remove_cv_t<T>, char32_t>;

template<class T>
concept numeric_integral = signed_numeric_integral<T> || unsigned_numeric_integral<T>;

template<class T>
concept numeric_arithmetic = numeric_integral<T> || std::floating_point<T>;

template<class T>
struct remove_cv
{
    using type = T;

    template<template <class> class F>
    using apply = F<T>;
};

template<class T>
struct remove_cv<T const>
{
    using type = T;

    template<template <class> class F>
    using apply = F<T> const;
};

template<class T>
struct remove_cv<T volatile>
{
    using type = T;

    template<template <class> class F>
    using apply = F<T> volatile;
};

template<class T>
struct remove_cv<T const volatile>
{
    using type = T;

    template<template <class> class F>
    using apply = F<T> const volatile;
};

// -----------------------------------------------------------

namespace detail {

template<class Cand, class T>
concept dominant_type_candidate_dominates =
    requires { typename std::common_type_t<T, std::decay_t<Cand>>; } &&
    std::same_as<std::common_type_t<T, std::decay_t<Cand>>, std::decay_t<Cand>>;

template<class Cand, class... Ts>
struct dominant_type_candidate
    : std::bool_constant<(dominant_type_candidate_dominates<Cand, Ts> && ...)>
{
    using type = std::decay_t<Cand>;
};

} // detail

// The type among `Ts...` that dominates all the others, i.e., the `Cand` in `Ts...`
// such that `std::common_type_t<T, Cand>` is `Cand` for every `T`.
//
// The result is always one of `Ts...` and does not depend on their order; no `type`
// member if no such type exists (e.g. `short` and `char`, whose common type `int` is
// not among them).
template<class... Ts>
struct dominant_type
{
    // No `::type`
};
template<class... Ts>
using dominant_type_t = dominant_type<Ts...>::type;

template<class... Ts>
    requires std::disjunction<detail::dominant_type_candidate<Ts, Ts...>...>::value
struct dominant_type<Ts...>
    : std::disjunction<detail::dominant_type_candidate<Ts, Ts...>...>
{};

// `T` dominates every type in `Ts...`, i.e., `std::common_type_t<U, T>` is `T` for
// every `U` in `Ts...`.
//
// Can be used to constrain the explicitness of conversion or assignment.
template<class T, class... Ts>
concept dominant = (detail::dominant_type_candidate_dominates<T, Ts> && ...);

// Denotes `iris::dominant_type<Ts...>` if it exists, otherwise equivalent to `std::common_type`.
//
//   `std::common_type_t<float, std::float16_t, std::bfloat16_t>`
//     -> `float`
//   `std::common_type_t<std::float16_t, std::bfloat16_t, float>`
//     -> ill-formed
//   `iris::symmetric_common_type_t<std::float16_t, std::bfloat16_t, float>`
//     -> `float` (in any order)
//
// Use `iris::dominant_type` instead when the result must be one of `Ts...`.
template<class... Ts>
struct symmetric_common_type : std::common_type<Ts...>
{};
template<class... Ts>
using symmetric_common_type_t = symmetric_common_type<Ts...>::type;

template<class... Ts>
    requires requires {
        typename dominant_type<Ts...>::type;
    }
struct symmetric_common_type<Ts...> : dominant_type<Ts...>
{};

// -----------------------------------------------------------

template<template<class A, class B> class F, class U, class... Ts>
struct disjunction_for : std::disjunction<F<U, Ts>...> {};

template<template<class A, class B> class F, class U, class... Ts>
inline constexpr bool disjunction_for_v = disjunction_for<F, U, Ts...>::value;

template<template<class A, class B> class F, class U, class... Ts>
struct conjunction_for : std::conjunction<F<U, Ts>...> {};

template<template<class A, class B> class F, class U, class... Ts>
inline constexpr bool conjunction_for_v = conjunction_for<F, U, Ts...>::value;

// -----------------------------------------------------------

// Provides definition equivalent to MSVC's STL for semantic compatibility
namespace detail::has_ADL_swap_detail {

#if defined(__clang__) || defined(__EDG__)
void swap() = delete; // poison pill
#else
void swap();
#endif

template<class, class = void> struct has_ADL_swap : std::false_type {};
template<class T> struct has_ADL_swap<T, std::void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>> : std::true_type {};

} // detail::has_ADL_swap_detail


template<class T>
struct is_trivially_swappable : std::conjunction<
    std::is_trivially_destructible<T>,
    std::is_trivially_move_constructible<T>,
    std::is_trivially_move_assignable<T>,
    // std::is_swappable cannot be used for this purpose because it assumes `using std::swap`
    std::negation<detail::has_ADL_swap_detail::has_ADL_swap<T>>
>
{};
template<> struct is_trivially_swappable<std::byte> : std::true_type {};

template<class T>
inline constexpr bool is_trivially_swappable_v = is_trivially_swappable<T>::value;

// ----------------------------------------------

template<class T, class U>
concept weakly_assignable_from = std::is_assignable_v<T, U>;


template<class T>
// ReSharper disable once CppFunctionDoesntReturnValue
[[nodiscard]] T declval_exact() noexcept
{
    // ReSharper disable once CppStaticAssertFailure
    static_assert(false, "declval_exact() must not be odr-used");
}

template<class T>
// ReSharper disable once CppFunctionDoesntReturnValue
[[nodiscard]] T copy_initialize(T) noexcept
{
    // ReSharper disable once CppStaticAssertFailure
    static_assert(false, "copy_initialize() must not be odr-used");
}

// ----------------------------------------------

namespace detail {

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2026/p0870r8.html

template<class From, class To>
struct is_convertible_without_narrowing_array_check
    : std::false_type
{};

template<class From, class To>
    requires requires (From&& x) {
        { std::type_identity_t<To[]>{static_cast<From&&>(x)} } -> std::same_as<To[1]>;
    }
struct is_convertible_without_narrowing_array_check<From, To>
    : std::true_type
{};

// ----------------------------------------------

template<class From, class To>
using is_never_narrowing_family = std::disjunction<
    std::is_same<std::remove_cvref_t<From>, std::remove_cvref_t<To>>,
    std::is_base_of<std::remove_cvref_t<From>, std::remove_cvref_t<To>>,
    std::is_function<std::remove_cvref_t<To>>,
    std::is_array<std::remove_cvref_t<To>>
>;

template<bool Direct, class From, class To>
using reference_binds_to_temporary = std::conditional_t<
    Direct,
    std::reference_constructs_from_temporary<To, From>,
    std::reference_converts_from_temporary<To, From>
>;

// `Check<From, To>` judges the initialization of a non-reference `To`.
// Array of reference cannot be formed, so a reference is judged by the temporary it binds to.
template<template<class, class> class Check, bool Direct, class From, class To>
struct narrowing_check_dispatch
    : Check<From, To>
{
    static_assert(!std::is_reference_v<To>);
};

template<template<class, class> class Check, bool Direct, class From, class To>
    requires
        std::is_reference_v<To> &&
        (is_never_narrowing_family<From, To>::value || !reference_binds_to_temporary<Direct, From, To>::value)
struct narrowing_check_dispatch<Check, Direct, From, To>
    : std::true_type
{};

template<template<class, class> class Check, bool Direct, class From, class To>
    requires
        std::is_reference_v<To> &&
        (!is_never_narrowing_family<From, To>::value) &&
        reference_binds_to_temporary<Direct, From, To>::value
struct narrowing_check_dispatch<Check, Direct, From, To>
    : Check<From, std::remove_reference_t<To>> // temporary is copy-initialized
{};

// ----------------------------------------------

template<template<class, class> class Check, class From, class To>
struct convertible_narrowing_check
    : narrowing_check_dispatch<Check, false, From, To>
{};

// Corner case mentioned on the paper: void
// cv variants are already handled via `std::is_convertible`.
template<template<class, class> class Check, class From, class To>
    requires std::is_void_v<To>
struct convertible_narrowing_check<Check, From, To>
    : std::true_type
{};

// DR11: Converting from T* to bool should be considered narrowing
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1957r2.html
//
// This is already applied to all major vendors, but some implementations
// disagree with `std::nullptr_t`. Note that `std::nullptr_t` is NOT a
// pointer type, so it cannot be checked with `std::is_pointer`.
template<template<class, class> class Check, class From, class To>
    requires std::is_null_pointer_v<From> && std::same_as<std::remove_cvref_t<To>, bool>
struct convertible_narrowing_check<Check, From, To>
    : std::false_type
{};

} // namespace detail

template<class From, class To>
struct is_convertible_without_narrowing : std::false_type
{};

template<class From, class To>
    requires std::is_convertible_v<From, To>
struct is_convertible_without_narrowing<From, To>
    : detail::convertible_narrowing_check<detail::is_convertible_without_narrowing_array_check, From, To>
{};

template<class From, class To>
inline constexpr bool is_convertible_without_narrowing_v = is_convertible_without_narrowing<From, To>::value;

template<class T, class U>
struct is_assignable_without_narrowing : std::false_type
{};

template<class T, class U>
    requires
        std::is_assignable_v<T, U> &&
        (
            !std::is_scalar_v<std::remove_reference_t<T>> ||
            is_convertible_without_narrowing_v<U, std::remove_reference_t<T>>
        )
struct is_assignable_without_narrowing<T, U>
    : std::true_type
{};

template<class T, class U>
inline constexpr bool is_assignable_without_narrowing_v = is_assignable_without_narrowing<T, U>::value;

// ----------------------------------------------

namespace detail {

template<class T>
concept class_or_union = std::is_class_v<T> || std::is_union_v<T>;

// Unlike `std::is_convertible_v`, false for an incomplete or abstract `T`
template<class T, class From>
concept copy_initializable = requires { iris::copy_initialize<T>(std::declval<From>()); };

template<class T, class From>
concept copy_list_initializable = requires { iris::copy_initialize<std::remove_cv_t<T>>({std::declval<From>()}); };

template<class T, class... Args>
concept direct_list_initializable = requires { std::type_identity_t<std::remove_cv_t<T>>{std::declval<Args>()...}; };

template<class T, class U>
concept list_assignable = requires { std::declval<T>() = {std::declval<U>()}; };

// ----------------------------------------------

struct opaque_checker
{
    opaque_checker() = default;
    opaque_checker(opaque_checker const&) = delete;
};

template<class... Args>
struct initializer_list_checker : opaque_checker
{
    template<class E>
        requires (copy_initializable<E, Args> && ...)
    operator std::initializer_list<E>() const;
};

// `T{args...}` selects an initializer-list constructor regardless of narrowing
template<class T, class... Args>
concept selects_initializer_list =
    !std::is_aggregate_v<T> &&
    (
        (!std::is_constructible_v<T, opaque_checker const&> &&
         std::is_constructible_v<T, initializer_list_checker<Args...> const&>) ||
        (direct_list_initializable<T, Args..., Args...> && !std::is_constructible_v<T, Args..., Args...>)
    );

template<class T, class U>
concept selects_initializer_list_assignment =
    selects_initializer_list<std::remove_cvref_t<T>, U> ||
    std::is_assignable_v<T, initializer_list_checker<U> const&>;

// ----------------------------------------------

template<class From, class To>
concept copy_initializable_without_any_narrowing =
    !selects_initializer_list<To, From> &&
    (
        (class_or_union<To> && class_or_union<std::remove_cvref_t<From>>) ||
        copy_list_initializable<To, From>
    );

template<class From, class To>
struct copy_initialization_narrowing_check
    : std::bool_constant<copy_initializable_without_any_narrowing<From, To>>
{};

// ----------------------------------------------

template<class V, bool Strict>
struct class_checker : opaque_checker
{
    template<class P>
        requires std::is_scalar_v<P> && std::is_convertible_v<V, P>
    operator P() const = delete;

    template<class Q>
        requires
            class_or_union<Q> && copy_initializable<Q, V> &&
            (!Strict || copy_initializable_without_any_narrowing<V, Q>)
    operator Q() const;
};

template<class V>
struct value_checker : opaque_checker
{
    operator std::add_rvalue_reference_t<V>() const;
};

template<class T, class... Args>
struct direct_initialization_narrowing_check
    : std::false_type
{};

template<class T, class... Args>
    requires
        (!selects_initializer_list<T, Args...>) &&
        direct_list_initializable<T, Args...> &&
        std::is_constructible_v<
            T,
            std::conditional_t<class_or_union<std::remove_cvref_t<Args>>, Args, value_checker<Args>>...
        >
struct direct_initialization_narrowing_check<T, Args...>
    : std::true_type
{};

template<class T>
struct direct_initialization_narrowing_check<T>
    : std::true_type
{};

template<class T, class Arg>
concept direct_initializable_without_any_narrowing =
    !selects_initializer_list<T, Arg> &&
    direct_list_initializable<T, Arg> &&
    (
        // `T{v}` may not check narrowing in the converting constructor of a class-type parameter
        !std::is_constructible_v<T, class_checker<Arg, false>> ||
        std::is_constructible_v<T, class_checker<Arg, true>>
    );

template<class T, class Arg>
struct direct_initialization_narrowing_check<T, Arg>
    : std::bool_constant<direct_initializable_without_any_narrowing<T, Arg>>
{};

template<class T, class Arg>
    requires std::is_reference_v<T>
struct direct_initialization_narrowing_check<T, Arg>
    : narrowing_check_dispatch<copy_initialization_narrowing_check, true, Arg, T>
{};

// See the note on P1957R2 above
template<class T, class Arg>
    requires std::is_null_pointer_v<std::remove_cvref_t<Arg>> && std::same_as<std::remove_cv_t<T>, bool>
struct direct_initialization_narrowing_check<T, Arg>
    : std::false_type
{};

} // detail

// is_convertible_without_narrowing + also rejects user-defined conversion in constructor
template<class From, class To>
struct is_convertible_without_any_narrowing : std::false_type
{};

template<class From, class To>
    requires std::is_convertible_v<From, To>
struct is_convertible_without_any_narrowing<From, To>
    : detail::convertible_narrowing_check<detail::copy_initialization_narrowing_check, From, To>
{};

template<class From, class To>
inline constexpr bool is_convertible_without_any_narrowing_v = is_convertible_without_any_narrowing<From, To>::value;

template<class T, class... Args>
struct is_constructible_without_any_narrowing : std::false_type
{};

template<class T, class... Args>
    requires std::is_constructible_v<T, Args...>
struct is_constructible_without_any_narrowing<T, Args...>
    : detail::direct_initialization_narrowing_check<T, Args...>
{};

template<class T, class... Args>
inline constexpr bool is_constructible_without_any_narrowing_v = is_constructible_without_any_narrowing<T, Args...>::value;

template<class T, class U>
struct is_assignable_without_any_narrowing : std::false_type
{};

template<class T, class U>
    requires
        std::is_assignable_v<T, U> &&
        (!detail::selects_initializer_list_assignment<T, U>) &&
        detail::list_assignable<T, U>
struct is_assignable_without_any_narrowing<T, U>
    : std::true_type
{};

template<class T, class U>
inline constexpr bool is_assignable_without_any_narrowing_v = is_assignable_without_any_narrowing<T, U>::value;


namespace detail {

template<std::size_t I, class Ti>
struct no_narrowing_tag
{
    static constexpr std::size_t index = I;
    using type = Ti;
};

template<std::size_t I, class Ti>
struct no_narrowing_overload
{
    template<class T>
    auto operator()(Ti, T&&) -> no_narrowing_tag<I, Ti>
        requires is_convertible_without_narrowing_v<T, Ti>
    {
        return {}; // silence MSVC warning
    }
};

template<class Is, class... Ts>
struct no_narrowing_fun;

// Imaginary function FUN of https://eel.is/c++draft/variant#ctor-14
template<std::size_t... Is, class... Ts>
struct no_narrowing_fun<std::index_sequence<Is...>, Ts...>
    : no_narrowing_overload<Is, Ts>...
{
    using no_narrowing_overload<Is, Ts>::operator()...;
};

template<class... Ts>
using no_narrowing_fun_for = no_narrowing_fun<std::index_sequence_for<Ts...>, Ts...>;

template<class Enabled, class T, class... Ts>
struct no_narrowing_resolution {};

template<class T, class... Ts>
struct no_narrowing_resolution<
    std::void_t<decltype(no_narrowing_fun_for<Ts...>{}(std::declval<T>(), std::declval<T>()))>, T, Ts...
> {
    using tag = decltype(no_narrowing_fun_for<Ts...>{}(std::declval<T>(), std::declval<T>()));
    using type = tag::type;
    static constexpr std::size_t index = tag::index;
};

} // detail

// We intentionally don't provide the convenient `_t` and `_v` aliases
// because they would lead to unnecessarily nested instantiation for
// legitimate infinite recursion errors on recursive types.
template<class T, class... Ts>
struct no_narrowing_resolution : detail::no_narrowing_resolution<void, T, Ts...> {};

template<std::size_t I, class T>
struct conversion_overload
{
    static std::integral_constant<std::size_t, I> select(T); // not defined
};

template<class... Overloads>
struct conversion_overloads : Overloads...
{
    using Overloads::select...;
};

template<class Overloads, class U>
concept conversion_resolves = requires { Overloads::select(std::declval<U>()); };

template<class Overloads, class U, std::size_t I>
concept conversion_selects = requires {
    { Overloads::select(std::declval<U>()) } -> std::same_as<std::integral_constant<std::size_t, I>>;
};

// ----------------------------------------------

namespace detail {

// The conversion part of INVOKE<R>: `To` is cv void, or VAL<From> can be
// implicitly converted to `To` without binding a reference to a temporary.
template<class From, class To>
concept invoke_convertible =
    std::is_void_v<To> ||
    requires {
        iris::copy_initialize<To>(iris::declval_exact<From>());
        requires !std::reference_converts_from_temporary_v<To, From>;
    };

} // detail

template<class F, class... Args>
concept directly_invocable = requires(F&& f, Args&&... args) {
    typename std::void_t<decltype(static_cast<F&&>(f)(static_cast<Args&&>(args)...))>;
};

template<class F, class... Args>
struct directly_invoke_result {};

template<class F, class... Args>
    requires directly_invocable<F, Args...>
struct directly_invoke_result<F, Args...>
{
    using type = decltype(std::declval<F>()(std::declval<Args>()...));
};
template<class F, class... Args>
using directly_invoke_result_t = directly_invoke_result<F, Args...>::type;

template<class R, class F, class... Args>
concept directly_invocable_r =
    directly_invocable<F, Args...> &&
    detail::invoke_convertible<decltype(std::declval<F>()(std::declval<Args>()...)), R>;

template<class F, class... Args>
struct is_directly_invocable : std::bool_constant<directly_invocable<F, Args...>> {};
template<class F, class... Args>
inline constexpr bool is_directly_invocable_v = is_directly_invocable<F, Args...>::value;

template<class R, class F, class... Args>
struct is_directly_invocable_r : std::bool_constant<directly_invocable_r<R, F, Args...>> {};
template<class R, class F, class... Args>
inline constexpr bool is_directly_invocable_r_v = is_directly_invocable_r<R, F, Args...>::value;

template<class F, class... Args>
struct is_nothrow_directly_invocable
    : std::bool_constant<requires(F&& f, Args&&... args) {
        { static_cast<F&&>(f)(static_cast<Args&&>(args)...) } noexcept;
    }>
{};
template<class F, class... Args>
inline constexpr bool is_nothrow_directly_invocable_v = is_nothrow_directly_invocable<F, Args...>::value;

template<class R, class F, class... Args>
struct is_nothrow_directly_invocable_r : std::false_type {};

template<class R, class F, class... Args>
    requires std::is_void_v<R>
struct is_nothrow_directly_invocable_r<R, F, Args...>
    : is_nothrow_directly_invocable<F, Args...>
{};

template<class R, class F, class... Args>
    requires
        (!std::is_void_v<R>) && directly_invocable_r<R, F, Args...> &&
        requires(F&& f, Args&&... args) {
            { iris::copy_initialize<R>(static_cast<F&&>(f)(static_cast<Args&&>(args)...)) } noexcept;
        }
struct is_nothrow_directly_invocable_r<R, F, Args...>
    : std::true_type
{};

template<class R, class F, class... Args>
inline constexpr bool is_nothrow_directly_invocable_r_v = is_nothrow_directly_invocable_r<R, F, Args...>::value;

} // iris

#endif

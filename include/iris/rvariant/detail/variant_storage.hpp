#ifndef IRIS_ZZ_RVARIANT_DETAIL_VARIANT_STORAGE_HPP
#define IRIS_ZZ_RVARIANT_DETAIL_VARIANT_STORAGE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

// IWYU pragma: private, include <iris/rvariant.hpp>

#include <iris/rvariant/rvariant_fwd.hpp>

#include <iris/type_list.hpp>
#include <iris/bits/specialization_of.hpp>

#include <memory>
#include <type_traits>
#include <utility>

#include <cstddef> // IWYU pragma: keep

#if defined(_MSC_VER)
# define IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN \
    _Pragma("warning(push)") \
    _Pragma("warning(disable: 4702)")
# define IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END \
    _Pragma("warning(pop)")
#else
# define IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
# define IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END
#endif

namespace iris::detail {

template<bool NeverValueless, class T>
[[nodiscard]] IRIS_FORCEINLINE constexpr std::size_t valueless_bias(T i) noexcept
{
    if constexpr (NeverValueless) {
        return i;
    } else {
        return ++i;
    }
}

template<class Variant, class T>
[[nodiscard]] IRIS_FORCEINLINE constexpr std::size_t valueless_bias(T i) noexcept
{
    if constexpr (std::remove_cvref_t<Variant>::never_valueless) {
        return i;
    } else {
        return ++i;
    }
}

template<bool NeverValueless, class T>
[[nodiscard]] IRIS_FORCEINLINE constexpr std::size_t valueless_unbias(T i) noexcept
{
    if constexpr (NeverValueless) {
        return i;
    } else {
        return --i;
    }
}

template<class Variant, class T>
[[nodiscard]] IRIS_FORCEINLINE constexpr std::size_t valueless_unbias(T i) noexcept
{
    if constexpr (std::remove_cvref_t<Variant>::never_valueless) {
        return i;
    } else {
        return --i;
    }
}

// Any non type-changing operation
//   => never becomes valueless (delegates to underlying type's exception safety)
//
// Construction
//   => no need to consider type traits, because lifetime never starts on exception
//
// Assignment (type-changing & RHS is not valueless)
//   => valueless iff move constructor throws
//
// Emplace (if VT(Args...) is throwing)
//   rvariant tmp(std::in_place_index<I>, static_cast<Args&&>(args)...);
//   *this = std::move(tmp);
//        ^^^ needs to be NOT observable on user's part, as per "Effects" https://eel.is/c++draft/variant.mod#7
//                        ^^^^^^^^^^^^^^
//                           if type-changing: VT is trivially move constructible
//                       if NOT type-changing: VT is trivially move assignable
//                                    ... and trivially destructible.
// So the final condition is:
//    move constructor is noexcept && (<= discarded; weaker than triviality)
//    trivially move constructible &&
//    trivially move assignable &&
//    trivially destructible.
//
// Note that "move" operation can fall back to "copy" if "move" is
// non-trivial AND "copy" is trivial.
//
// Furthermore, we have modified the spec for `.emplace` so that
// `recursive_wrapper` can be always treated as never_valueless part,
// so we include that optimization for PoC.

// Additional size limit
inline constexpr std::size_t never_valueless_trivial_size_limit = 256;

template<class T>
concept is_never_valueless_impl =
    is_ttp_specialization_of_v<T, recursive_wrapper> ||
    (
        sizeof(T) <= never_valueless_trivial_size_limit &&
        std::is_trivially_destructible_v<T> &&
        (
            std::is_trivially_move_constructible_v<T> ||
            std::is_trivially_copy_constructible_v<T>
        ) &&
        (
            std::is_trivially_move_assignable_v<T> ||
            std::is_trivially_copy_assignable_v<T>
        )
    );

template<class... Ts>
struct is_never_valueless;

template<>
struct is_never_valueless<> : std::true_type {};

template<class T, class... Ts>
    requires is_never_valueless_impl<T>
struct is_never_valueless<T, Ts...>
    : std::bool_constant<
        is_never_valueless<Ts...>::value
    >
{};

template<class T, class... Ts>
    requires (!is_never_valueless_impl<T>)
struct is_never_valueless<T, Ts...>
    : std::false_type
{};

template<class... Ts>
constexpr bool is_never_valueless_v = is_never_valueless<Ts...>::value;

// for bypassing access control
template<class Storage>
struct storage_never_valueless : std::bool_constant<std::remove_cvref_t<Storage>::never_valueless> {};

// --------------------------------------------

template<bool TriviallyDestructible, class... Ts>
struct variadic_union {};

template<class... Ts>
using make_variadic_union_t = variadic_union<
    (std::is_trivially_destructible_v<Ts> && ...),
    Ts...
>;

template<class T, class... Ts>
struct variadic_union<true, T, Ts...>
{
#if IRIS_CI
    static_assert(std::conjunction_v<std::is_trivially_destructible<T>, std::is_trivially_destructible<Ts>...>);
#endif

    static constexpr std::size_t size = sizeof...(Ts) + 1;
    static constexpr bool never_valueless = is_never_valueless_v<T, Ts...>;

    // no active member
    // ReSharper disable once CppPossiblyUninitializedMember
    constexpr explicit variadic_union() noexcept {}

#ifdef __RESHARPER__
    // These are required for propagating traits
    variadic_union(variadic_union const&) = default;
    variadic_union(variadic_union&&) = default;
    variadic_union& operator=(variadic_union const&) = default;
    variadic_union& operator=(variadic_union&&) = default;

    // According to the standard, a union with non-trivially-(copy|move)-(constructible|assignable)
    // members has *implicitly*-deleted corresponding special functions.
    // Although it should work only by the "= default" declaration, some compilers
    // (e.g. ReSharper's "Code Inspection") fail to detect such traits,
    // resulting in red squiggles everywhere.
    variadic_union(variadic_union const&)            requires(!std::is_trivially_copy_constructible_v<T> || (!std::is_trivially_copy_constructible_v<Ts> || ...)) = delete;
    variadic_union(variadic_union&&)                 requires(!std::is_trivially_move_constructible_v<T> || (!std::is_trivially_move_constructible_v<Ts> || ...)) = delete;
    variadic_union& operator=(variadic_union const&) requires(!std::is_trivially_copy_assignable_v<T> || (!std::is_trivially_copy_assignable_v<Ts> || ...)) = delete;
    variadic_union& operator=(variadic_union&&)      requires(!std::is_trivially_move_assignable_v<T> || (!std::is_trivially_move_assignable_v<Ts> || ...)) = delete;
#endif

    union {
        T first;
        make_variadic_union_t<Ts...> rest;
    };
};

template<class T, class... Ts>
struct variadic_union<false, T, Ts...>
{
#if IRIS_CI
    static_assert(!std::conjunction_v<std::is_trivially_destructible<T>, std::is_trivially_destructible<Ts>...>);
#endif

    static constexpr std::size_t size = sizeof...(Ts) + 1;
    static constexpr bool never_valueless = is_never_valueless_v<T, Ts...>;

    // no active member
    // ReSharper disable once CppPossiblyUninitializedMember
    constexpr explicit variadic_union() noexcept {}

    constexpr ~variadic_union() noexcept {}

    variadic_union(variadic_union const&) = default;
    variadic_union(variadic_union&&) = default;
    variadic_union& operator=(variadic_union const&) = default;
    variadic_union& operator=(variadic_union&&) = default;

#ifdef __RESHARPER__
    variadic_union(variadic_union const&)            requires(!std::is_trivially_copy_constructible_v<T> || (!std::is_trivially_copy_constructible_v<Ts> || ...)) = delete;
    variadic_union(variadic_union&&)                 requires(!std::is_trivially_move_constructible_v<T> || (!std::is_trivially_move_constructible_v<Ts> || ...)) = delete;
    variadic_union& operator=(variadic_union const&) requires(!std::is_trivially_copy_assignable_v<T> || (!std::is_trivially_copy_assignable_v<Ts> || ...)) = delete;
    variadic_union& operator=(variadic_union&&)      requires(!std::is_trivially_move_assignable_v<T> || (!std::is_trivially_move_assignable_v<Ts> || ...)) = delete;
#endif

    union {
        T first;
        make_variadic_union_t<Ts...> rest;
    };
};

template<class Variant>
struct forward_storage_t_impl
{
    static_assert(
        is_ttp_specialization_of_v<std::remove_cvref_t<Variant>, rvariant>,
        "`forward_storage` only accepts types which are exactly `rvariant`. Maybe you forgot `as_rvariant_t`?"
    );
    using type = decltype(std::declval<Variant>().storage());
};

template<class Variant>
using forward_storage_t = forward_storage_t_impl<Variant>::type;

template<class Variant>
[[nodiscard]] IRIS_FORCEINLINE constexpr forward_storage_t<Variant>&&
forward_storage(std::remove_reference_t<Variant>& v IRIS_LIFETIMEBOUND) noexcept
{
    return static_cast<Variant&&>(v).storage();
}

template<class Variant>
[[nodiscard]] IRIS_FORCEINLINE constexpr forward_storage_t<Variant>&&
forward_storage(std::remove_reference_t<Variant>&& v IRIS_LIFETIMEBOUND) noexcept  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    return static_cast<Variant&&>(v).storage();
}

template<std::size_t K, class Storage>
[[nodiscard]] IRIS_FORCEINLINE constexpr auto&& raw_level(Storage&& storage IRIS_LIFETIMEBOUND) noexcept
{
         if constexpr (K ==  0) return static_cast<Storage&&>(storage);
    else if constexpr (K ==  1) return static_cast<Storage&&>(storage).rest;
    else if constexpr (K ==  2) return static_cast<Storage&&>(storage).rest.rest;
    else if constexpr (K ==  3) return static_cast<Storage&&>(storage).rest.rest.rest;
    else if constexpr (K ==  4) return static_cast<Storage&&>(storage).rest.rest.rest.rest;
    else if constexpr (K ==  5) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest;
    else if constexpr (K ==  6) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest;
    else if constexpr (K ==  7) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K ==  8) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K ==  9) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 10) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 11) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 12) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 13) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 14) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 15) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 16) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 17) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 18) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 19) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 20) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 21) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 22) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 23) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 24) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 25) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 26) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 27) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 28) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 29) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 30) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 31) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K == 32) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest;
    else if constexpr (K < 64)  return detail::raw_level<K - 32>(
                                       static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest);
    else                        return detail::raw_level<K - 64>(
                                       static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest
                                                                      .rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest);
}

template<std::size_t I, class Storage>
[[nodiscard]] IRIS_FORCEINLINE constexpr auto&& raw_get(Storage&& storage IRIS_LIFETIMEBOUND) noexcept
{
         if constexpr (I ==  0) return static_cast<Storage&&>(storage).first;
    else if constexpr (I ==  1) return static_cast<Storage&&>(storage).rest.first;
    else if constexpr (I ==  2) return static_cast<Storage&&>(storage).rest.rest.first;
    else if constexpr (I ==  3) return static_cast<Storage&&>(storage).rest.rest.rest.first;
    else if constexpr (I ==  4) return static_cast<Storage&&>(storage).rest.rest.rest.rest.first;
    else if constexpr (I ==  5) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.first;
    else if constexpr (I ==  6) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I ==  7) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I ==  8) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I ==  9) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 10) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 11) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 12) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 13) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 14) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 15) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 16) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 17) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 18) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 19) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 20) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 21) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 22) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 23) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 24) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 25) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 26) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 27) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 28) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 29) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 30) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I == 31) return static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.first;
    else if constexpr (I < 64)  return detail::raw_get<I - 32>(
                                       static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest);
    else                        return detail::raw_get<I - 64>(
                                       static_cast<Storage&&>(storage).rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest
                                                                      .rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest.rest);
}

template<std::size_t I, class Storage>
using raw_get_t = decltype(detail::raw_get<I>(std::declval<Storage>()));

// Starts the lifetime of the storage and each enclosing `rest`, then constructs the I-th alternative in place.
// Doing both in one function lets GCC see the whole storage start a new lifetime, so ending a trivial old alternative needs no code.
template<std::size_t I, class Ks = std::make_index_sequence<I + 1>>
struct alternative_constructor;

template<std::size_t I, std::size_t... Ks>
struct alternative_constructor<I, std::index_sequence<Ks...>>
{
IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
    template<class Storage, class... Args>
    static constexpr void construct(Storage& storage, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<raw_get_t<I, Storage&>>, Args...>)
    {
        (std::construct_at(std::addressof(detail::raw_level<Ks>(storage))), ...);
        // value-initializes when Args is empty; https://eel.is/c++draft/variant.ctor#3
        std::construct_at(std::addressof(detail::raw_get<I>(storage)), static_cast<Args&&>(args)...);
    }
IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END
};

// --------------------------------------------------
// --------------------------------------------------
// --------------------------------------------------

// https://eel.is/c++draft/variant.visit

namespace as_variant_impl {

template<class... Ts>
constexpr auto&& as_variant(rvariant<Ts...>& var) { return var; }

template<class... Ts>
constexpr auto&& as_variant(rvariant<Ts...> const& var) { return var; }

template<class... Ts>
constexpr auto&& as_variant(rvariant<Ts...>&& var) { return std::move(var); }

template<class... Ts>
constexpr auto&& as_variant(rvariant<Ts...> const&& var) { return std::move(var); }

} // as_variant_impl

template<class T>
using as_variant_t = decltype(as_variant_impl::as_variant(std::declval<T>()));

} // iris::detail

#endif

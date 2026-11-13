#ifndef IRIS_ZZ_HASH_HPP
#define IRIS_ZZ_HASH_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/requirements.hpp>
#include <iris/bits/specialization_of.hpp>
#include <iris/bits/is_function_object.hpp>

#include <functional>
#include <ranges>
#include <utility>
#include <concepts>

#include <cstddef>
#include <cstdint>

namespace iris {

// "disabled" version, https://eel.is/c++draft/unord.hash#4
template<class Key>
struct is_hash_enabled : std::false_type
{
    static_assert(!is_ttp_specialization_of_v<Key, std::hash>);
    static_assert(requires { typename std::hash<Key>; }, "Specialization of `hash` should exist; https://eel.is/c++draft/unord.hash#note-2");

private:
    using H = std::hash<Key>;
    static_assert(!std::is_default_constructible_v<H>);
    static_assert(!std::is_copy_constructible_v<H>);
    static_assert(!std::is_move_constructible_v<H>);
    static_assert(!std::is_copy_assignable_v<H>);
    static_assert(!std::is_move_assignable_v<H>);
    static_assert(!is_function_object_v<H, Key>);
    static_assert(!is_function_object_v<H, Key const>);
    static_assert(!is_function_object_v<H const, Key>);
    static_assert(!is_function_object_v<H const, Key const>);
};

// "enabled" version, https://eel.is/c++draft/unord.hash#5
template<class Key>
    requires
        requires { typename std::hash<Key>; } &&
        req::Cpp17Hash<std::hash<Key>> &&
        req::Cpp17DefaultConstructible<std::hash<Key>> &&
        req::Cpp17CopyAssignable<std::hash<Key>> &&
        req::Cpp17Swappable<std::hash<Key>>
struct is_hash_enabled<Key> : std::true_type
{
    static_assert(!is_ttp_specialization_of_v<Key, std::hash>);
};

template<class Key>
constexpr bool is_hash_enabled_v = is_hash_enabled<Key>::value;


template<class T>
struct is_nothrow_hashable
    : std::bool_constant<requires(T const& x) {
        { std::hash<T>{}(x) } noexcept;
    }>
{
    static_assert(
        !std::is_const_v<T> && !std::is_volatile_v<T> && !std::is_reference_v<T>,
        "Although the standard has no restriction on hashing potentially "
        "cv-qualified and/or reference types, we intentionally disallow "
        "them in our metafunction to prevent error-prone instantiations."
    );

    static_assert(is_hash_enabled_v<T>, "is_nothrow_hashable must be used only after proper overload resolution in SFINAE-friendly context");
};

template<class T>
constexpr bool is_nothrow_hashable_v = is_nothrow_hashable<T>::value;

namespace detail {

template<std::size_t Base>
constexpr std::size_t hash_mix(std::size_t) noexcept = delete;

// https://jonkagstrom.com/mx3/mx3_rev2.html

#if SIZE_MAX >= 0xFFFFFFFFFFFFFFFFULL
template<>
[[nodiscard]] constexpr std::size_t hash_mix<8>(std::size_t x) noexcept
{
    x ^= x >> 32;
    x *= 0xe9846af9b1a615duz;
    x ^= x >> 32;
    x *= 0xe9846af9b1a615duz;
    x ^= x >> 28;
    return x;
}
#endif

template<>
[[nodiscard]] constexpr std::size_t hash_mix<4>(std::size_t x) noexcept
{
    x ^= x >> 16;
    x *= 0x21f0aaaduz;
    x ^= x >> 15;
    x *= 0x735a2d97uz;
    x ^= x >> 15;
    return x;
}

} // detail

// https://github.com/boostorg/container_hash/blob/5d8b8ac2b9d9d7cb3818f88fd7e6372e5f072ff5/include/boost/container_hash/hash.hpp#L472C53-L472C63
// https://softwareengineering.stackexchange.com/questions/402542/where-do-magic-hashing-constants-like-0x9e3779b9-and-0x9e3779b1-come-from

[[nodiscard]] constexpr std::size_t hash_combine(
    std::same_as<std::size_t> auto const seed, std::same_as<std::size_t> auto const additional_hash
) noexcept
{
#if SIZE_MAX >= 0xFFFFFFFFFFFFFFFFULL
    return detail::hash_mix<sizeof(std::size_t)>(
        seed + 0x9e3779b97f4a7c55uz + additional_hash
    );
#else
    return detail::hash_mix<sizeof(std::size_t)>(
        seed + 0x9e3779b9uz + additional_hash
    );
#endif
}

namespace detail {

namespace adl_hashable_detail {

void hash_value() = delete; // poison pill

template<class T>
concept adl_hashable = requires(T const& x) {
    { hash_value(x) } -> std::convertible_to<std::size_t>;
};

} // adl_hashable_detail

using adl_hashable_detail::adl_hashable;

// This must be CPO to find nested `hash_value` (not solvable with free function)
struct hash_value_fn
{
    template<class T>
        requires adl_hashable<T>
    [[nodiscard]] static constexpr std::size_t operator()(T const& x)
        noexcept(noexcept(hash_value(x)))
    {
        return hash_value(x);
    }

    template<class T>
        requires (!adl_hashable<T>) && is_hash_enabled_v<T>
    [[nodiscard]] static constexpr std::size_t operator()(T const& x)
        noexcept(is_nothrow_hashable_v<T>)
    {
        return std::hash<T>{}(x);
    }

    template<class R>
        requires (!adl_hashable<R>) && (!is_hash_enabled_v<R>) && std::ranges::input_range<R const>
    [[nodiscard]] static constexpr std::size_t operator()(R const& r)
        noexcept(
            noexcept(std::ranges::begin(r)) &&
            noexcept(std::ranges::end(r)) &&
            std::is_nothrow_move_constructible_v<std::ranges::iterator_t<R const>> &&
            std::is_nothrow_move_constructible_v<std::ranges::sentinel_t<R const>> &&
            noexcept(++std::declval<std::ranges::iterator_t<R const>&>()) &&
            noexcept(std::declval<std::ranges::iterator_t<R const>&>() != std::declval<std::ranges::sentinel_t<R const> const&>()) &&
            noexcept(std::declval<hash_value_fn const&>()(*std::declval<std::ranges::iterator_t<R const>&>()))
        )
    {
        std::size_t seed = 0;
        auto it = std::ranges::begin(r);
        auto const se = std::ranges::end(r);
        for (; it != se; ++it) {
            seed = iris::hash_combine(seed, hash_value_fn{}(*it));
        }
        return seed;
    }
};

} // detail

inline constexpr detail::hash_value_fn hash_value{};

template<class T, class... Ts>
[[nodiscard]] constexpr std::size_t hash_all(T const& first, Ts const&... rest)
    noexcept(noexcept(iris::hash_value(first)) && (noexcept(iris::hash_value(rest)) && ...))
{
    std::size_t seed = iris::hash_value(first);
    ((seed = iris::hash_combine(seed, iris::hash_value(rest))), ...);
    return seed;
}

} // iris

#endif

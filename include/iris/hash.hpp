#ifndef IRIS_HASH_HPP
#define IRIS_HASH_HPP

// SPDX-License-Identifier: MIT

#include <iris/requirements.hpp>
#include <iris/type_traits.hpp>

#include <functional>
#include <bit>

#include <cstddef>

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


template<class T, class Enabled = void>
struct is_nothrow_hashable : std::false_type
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
struct is_nothrow_hashable<T, std::void_t<decltype(std::hash<T>{}(std::declval<T const&>()))>>
    : std::bool_constant<noexcept(std::hash<T>{}(std::declval<T const&>()))>
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

} // detail

// https://github.com/boostorg/container_hash/blob/5d8b8ac2b9d9d7cb3818f88fd7e6372e5f072ff5/include/boost/container_hash/hash.hpp#L472C53-L472C63
// https://softwareengineering.stackexchange.com/questions/402542/where-do-magic-hashing-constants-like-0x9e3779b9-and-0x9e3779b1-come-from

template<class T>
[[nodiscard]] constexpr std::size_t hash_combine(std::size_t const seed, T const& v) noexcept
{
    static_assert(is_hash_enabled_v<T>);
    return detail::hash_mix<sizeof(std::size_t)>(
        seed + 0x9e3779b97f4a7c55uz + std::hash<T>{}(v)
    );
}

} // iris

#endif

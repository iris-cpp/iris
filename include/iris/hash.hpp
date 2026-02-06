#ifndef IRIS_HASH_HPP
#define IRIS_HASH_HPP

// SPDX-License-Identifier: MIT

#include <iris/core/hash_fwd.hpp>

#include <iris/requirements.hpp>
#include <iris/type_traits.hpp>

#include <type_traits>

namespace iris {

namespace req {

namespace detail {

template<class Key>
struct Cpp17Hash_convertible_to_Key
{
    // ReSharper disable once CppFunctionIsNotImplemented
    operator Key();
    // ReSharper disable once CppFunctionIsNotImplemented
    operator Key const() const;
};

// https://eel.is/c++draft/hash.requirements
template<class H>
struct Cpp17Hash_impl : std::false_type
{
    static_assert(is_ttp_specialization_of_v<H, std::hash>);
};

template<class Key>
    requires
        is_function_object_v<std::hash<Key>, Key> &&
        is_function_object_v<std::hash<Key>, Key const> &&
        is_function_object_v<std::hash<Key> const, Key> &&
        is_function_object_v<std::hash<Key> const, Key const> &&
        req::Cpp17CopyConstructible<std::hash<Key>> && req::Cpp17Destructible<std::hash<Key>> &&
        requires {
            { std::declval<std::hash<Key>      >()(std::declval<Cpp17Hash_convertible_to_Key<Key>      >()) } -> std::same_as<std::size_t>;
            { std::declval<std::hash<Key>      >()(std::declval<Cpp17Hash_convertible_to_Key<Key> const>()) } -> std::same_as<std::size_t>;
            { std::declval<std::hash<Key> const>()(std::declval<Cpp17Hash_convertible_to_Key<Key>      >()) } -> std::same_as<std::size_t>;
            { std::declval<std::hash<Key> const>()(std::declval<Cpp17Hash_convertible_to_Key<Key> const>()) } -> std::same_as<std::size_t>;
            { std::declval<std::hash<Key>      >()(std::declval<Key&>()) } -> std::same_as<std::size_t>;
            { std::declval<std::hash<Key> const>()(std::declval<Key&>()) } -> std::same_as<std::size_t>;
        }
struct Cpp17Hash_impl<std::hash<Key>> : std::true_type
{
    static_assert(!std::is_const_v<Key> && !std::is_reference_v<Key>);
};

} // detail

template<class H>
concept Cpp17Hash = detail::Cpp17Hash_impl<H>::value;

}  // req

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

}  // iris

#endif

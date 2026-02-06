#ifndef IRIS_REQUIREMENTS_HPP
#define IRIS_REQUIREMENTS_HPP

// SPDX-License-Identifier: MIT

#include <iris/hash_fwd.hpp>
#include <iris/bits/is_function_object.hpp>
#include <iris/bits/specialization_of.hpp>

#include <concepts>
#include <iosfwd>
#include <type_traits>
#include <utility>

namespace iris {

namespace detail {

template<class T>
concept boolean_testable_impl = std::convertible_to<T, bool>;

}  // detail

// https://eel.is/c++draft/concept.booleantestable#concept:boolean-testable
template<class T>
concept boolean_testable = detail::boolean_testable_impl<T> && requires(T&& t) {
    { !std::forward<T>(t) } -> detail::boolean_testable_impl;
};

namespace req {

// https://eel.is/c++draft/utility.arg.requirements#tab:cpp17.equalitycomparable
template<class T>
concept Cpp17EqualityComparable =
    requires(T a, T b)             { requires boolean_testable<decltype(a == b)>; } &&
    requires(T const a, T b)       { requires boolean_testable<decltype(a == b)>; } &&
    requires(T a, T const b)       { requires boolean_testable<decltype(a == b)>; } &&
    requires(T const a, T const b) { requires boolean_testable<decltype(a == b)>; };

// https://eel.is/c++draft/utility.requirements#tab:cpp17.lessthancomparable
template<class T>
concept Cpp17LessThanComparable =
    requires(T a, T b)             { requires boolean_testable<decltype(a < b)>; } &&
    requires(T const a, T b)       { requires boolean_testable<decltype(a < b)>; } &&
    requires(T a, T const b)       { requires boolean_testable<decltype(a < b)>; } &&
    requires(T const a, T const b) { requires boolean_testable<decltype(a < b)>; };


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SEE ALSO: https://cplusplus.github.io/LWG/issue2146
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// https://eel.is/c++draft/utility.requirements#tab:cpp17.defaultconstructible
template<class T>
concept Cpp17DefaultConstructible = requires {
    ::new(static_cast<void*>(nullptr)) T; // T t; is valid
    ::new(static_cast<void*>(nullptr)) T{}; // T u{}; is valid
    T();
    T{};
};

// https://eel.is/c++draft/utility.requirements#tab:cpp17.moveconstructible
template<class T>
concept Cpp17MoveConstructible = requires {
    requires std::is_convertible_v<T, T> && std::is_constructible_v<T, T>;
};

// https://eel.is/c++draft/utility.requirements#tab:cpp17.copyconstructible
template<class T>
concept Cpp17CopyConstructible = Cpp17MoveConstructible<T> && requires {
    requires std::is_convertible_v<T&, T>       && std::is_constructible_v<T, T&>;
    requires std::is_convertible_v<T const&, T> && std::is_constructible_v<T, T const&>;
    requires std::is_convertible_v<T const, T>  && std::is_constructible_v<T, T const>;
};

// https://eel.is/c++draft/utility.requirements#tab:cpp17.moveassignable
template<class T>
concept Cpp17MoveAssignable = requires(T t, T&& rv) {
    { t = static_cast<T&&>(rv) } -> std::same_as<T&>;
};

// https://eel.is/c++draft/utility.requirements#tab:cpp17.copyassignable
template<class T>
concept Cpp17CopyAssignable = Cpp17MoveAssignable<T> && requires {
    requires requires(T t, T& v)        { { t = v } -> std::same_as<T&>; };
    requires requires(T t, T const& v)  { { t = v } -> std::same_as<T&>; };
    requires requires(T t, T const&& v) { { t = static_cast<T const&&>(v) } -> std::same_as<T&>; };
};

// https://eel.is/c++draft/utility.requirements#tab:cpp17.destructible
template<class T>
concept Cpp17Destructible = (!std::is_array_v<T>) && std::is_object_v<T> && requires(T u) {
    { u.~T() };
};

// https://eel.is/c++draft/swappable.requirements#5
template<class X>
concept Cpp17Swappable = std::is_swappable_v<X&>;

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

} // req

} // iris

#endif

#ifndef IRIS_ZZ_STDINT_HPP
#define IRIS_ZZ_STDINT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_traits.hpp>

#include <concepts>
#include <type_traits>

#include <cstdint> // IWYU pragma: export
#include <cstddef> // IWYU pragma: keep

namespace iris {

// `std::make_signed<T>` family requires T to be nonbool integer or enum

namespace detail {

template<bool IsSigned, std::size_t Bytes>
struct integer_of_size_impl;

template<>
struct integer_of_size_impl<true, 1>
{
    using type = std::int8_t;
};

template<>
struct integer_of_size_impl<true, 2>
{
    using type = std::int16_t;
};

template<>
struct integer_of_size_impl<true, 4>
{
    using type = std::int32_t;
};

template<>
struct integer_of_size_impl<true, 8>
{
    using type = std::int64_t;
};

// ----------------------------------------------

template<>
struct integer_of_size_impl<false, 1>
{
    using type = std::uint8_t;
};

template<>
struct integer_of_size_impl<false, 2>
{
    using type = std::uint16_t;
};

template<>
struct integer_of_size_impl<false, 4>
{
    using type = std::uint32_t;
};

template<>
struct integer_of_size_impl<false, 8>
{
    using type = std::uint64_t;
};

template<class T>
struct signedness_of_integral;

template<std::integral T>
struct signedness_of_integral<T> : std::bool_constant<std::is_signed_v<T>>
{};

template<class T>
    requires std::is_enum_v<T>
struct signedness_of_integral<T> : std::bool_constant<std::is_signed_v<std::underlying_type_t<T>>>
{};

template<class T>
using make_signed_of_size_impl_t = integer_of_size_impl<true, sizeof(T)>::type;

template<class T>
using make_unsigned_of_size_impl_t = integer_of_size_impl<false, sizeof(T)>::type;

template<class T>
using make_integer_of_size_impl_t = integer_of_size_impl<signedness_of_integral<T>::value, sizeof(T)>::type;

} // detail

template<class T>
struct make_signed_of_size
{
    static_assert(std::integral<T> && !std::same_as<std::remove_cv_t<T>, bool> || std::is_enum_v<T>);
    using type = remove_cv<T>::template apply<detail::make_signed_of_size_impl_t>;
};

template<class T>
using make_signed_of_size_t = make_signed_of_size<T>::type;

template<class T>
struct make_unsigned_of_size
{
    static_assert(std::integral<T> && !std::same_as<std::remove_cv_t<T>, bool> || std::is_enum_v<T>);
    using type = remove_cv<T>::template apply<detail::make_unsigned_of_size_impl_t>;
};

template<class T>
using make_unsigned_of_size_t = make_unsigned_of_size<T>::type;

template<class T>
struct make_integer_of_size
{
    static_assert(std::integral<T> && !std::same_as<std::remove_cv_t<T>, bool> || std::is_enum_v<T>);
    using type = remove_cv<T>::template apply<detail::make_integer_of_size_impl_t>;
};

template<class T>
using make_integer_of_size_t = make_integer_of_size<T>::type;

} // iris

#endif

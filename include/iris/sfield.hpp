#ifndef IRIS_ZZ_SFIELD_HPP
#define IRIS_ZZ_SFIELD_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/alloy/adapt.hpp> // IWYU pragma: export

#include <iris/pp/tuple.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <cassert>


namespace iris::sfield {

namespace detail {

template<class T>
struct is_value_semantics_preferred : std::false_type
{};

template<class T>
    requires
        std::is_fundamental_v<T> ||
        std::is_reference_v<T> ||
        std::is_pointer_v<T> ||
        std::is_enum_v<T>
struct is_value_semantics_preferred<T> : std::true_type
{};

template<class T>
using getter_return_t = std::conditional_t<
    is_value_semantics_preferred<T>::value,
    T,
    T const&
>;

template<class T>
using setter_param_t = std::conditional_t<
    is_value_semantics_preferred<T>::value,
    T,
    T const&
>;

} // detail


#define IRIS_ZZ_SFIELD_EXPAND_TYPE(...) __VA_ARGS__

#define IRIS_ZZ_SFIELD_DATA_MEMBER_NAME(field_name) field_name ## _

#define IRIS_ZZ_SFIELD_DATA_MEMBER(paren_type, field_name, ...) \
    private: \
        IRIS_ZZ_SFIELD_EXPAND_TYPE paren_type IRIS_ZZ_SFIELD_DATA_MEMBER_NAME(field_name){__VA_ARGS__};

#define IRIS_ZZ_SFIELD_GETTER(paren_type, field_name) \
    public: \
        [[nodiscard]] constexpr iris::sfield::detail::getter_return_t<IRIS_ZZ_SFIELD_EXPAND_TYPE paren_type> get_ ## field_name () const noexcept \
        { \
            return IRIS_ZZ_SFIELD_DATA_MEMBER_NAME(field_name); \
        } \

#define IRIS_ZZ_SFIELD_GETTER_BOOL(paren_type, field_name) \
    public: \
        [[nodiscard]] constexpr bool is_ ## field_name () const noexcept \
        { \
            return IRIS_ZZ_SFIELD_DATA_MEMBER_NAME(field_name); \
        }

#define IRIS_ZZ_SFIELD_SETTER(paren_type, field_name) \
    public: \
        constexpr void set_ ## field_name (iris::sfield::detail::setter_param_t<IRIS_ZZ_SFIELD_EXPAND_TYPE paren_type> new_value) \
            noexcept(std::is_nothrow_copy_assignable_v<IRIS_ZZ_SFIELD_EXPAND_TYPE paren_type>) \
        { \
            IRIS_ZZ_SFIELD_DATA_MEMBER_NAME(field_name) = new_value; \
        }

#define IRIS_SFIELD_GET(paren_type, field_name, ...) \
    IRIS_ZZ_SFIELD_DATA_MEMBER(paren_type, field_name, __VA_ARGS__) \
    IRIS_ZZ_SFIELD_GETTER(paren_type, field_name)

#define IRIS_SFIELD_GET_SET(paren_type, field_name, ...) \
    IRIS_ZZ_SFIELD_DATA_MEMBER(paren_type, field_name, __VA_ARGS__) \
    IRIS_ZZ_SFIELD_GETTER(paren_type, field_name) \
    IRIS_ZZ_SFIELD_SETTER(paren_type, field_name)

#define IRIS_SFIELD(paren_type, field_name, ...) \
    IRIS_SFIELD_GET_SET(paren_type, field_name, __VA_ARGS__)


// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_SFIELD_BOOL_GET(paren_type, field_name, default_value) \
    IRIS_ZZ_SFIELD_DATA_MEMBER(paren_type, field_name, default_value) \
    IRIS_ZZ_SFIELD_GETTER_BOOL(paren_type, field_name)

// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_SFIELD_BOOL_GET_SET(paren_type, field_name, default_value) \
    IRIS_ZZ_SFIELD_DATA_MEMBER(paren_type, field_name, default_value) \
    IRIS_ZZ_SFIELD_GETTER_BOOL(paren_type, field_name) \
    IRIS_ZZ_SFIELD_SETTER(paren_type, field_name)

// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_SFIELD_BOOL(paren_type, field_name, default_value) \
    IRIS_SFIELD_BOOL_GET_SET(paren_type, field_name, default_value)

} // iris::sfield

#endif

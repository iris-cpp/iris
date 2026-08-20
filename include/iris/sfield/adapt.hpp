#ifndef IRIS_ZZ_SFIELD_ADAPT_HPP
#define IRIS_ZZ_SFIELD_ADAPT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/sfield/serialize_traits.hpp>

#include <iris/alloy/adapt.hpp> // IWYU pragma: export

#include <iris/pp/tuple.hpp>
#include <iris/pp/stringize.hpp>
#include <iris/pp/cat.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace iris::sfield {

namespace detail {

template<class T>
struct is_value_semantics_preferred : std::false_type
{};

template<class T>
    requires
        std::is_arithmetic_v<T> ||
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


template<auto Mem>
struct field_definition
{
    std::string_view name;
};

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
        } \
    \
    private: \
        [[nodiscard]] constexpr bool get_ ## field_name () const noexcept \
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


#define IRIS_SFIELD_CLASS(class_name) \
    template<class Class> \
    friend struct ::iris::sfield::detail::adapted_class;

// ------------------------------------------------------------

#define IRIS_ZZ_SFIELD_ADAPT_FIELD(field_name, class_name) \
    ::iris::sfield::detail::field_definition< \
        &class_name::IRIS_PP_CAT(get_, field_name) \
    >{IRIS_PP_STRINGIZE(field_name)},

#define IRIS_SFIELD_ADAPT(class_name, ...) \
    template<> \
    struct iris::sfield::detail::adapted_class<class_name> \
    { \
        inline static constexpr auto fields = ::iris::alloy::tuple{ \
            IRIS_PP_SEQ_FOR_EACH( \
                IRIS_PP_TUPLE_TO_SEQ((__VA_ARGS__)), \
                IRIS_ZZ_SFIELD_ADAPT_FIELD, \
                class_name \
            ) \
        }; \
    };

} // iris::sfield

#endif

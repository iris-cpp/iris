#ifndef IRIS_ZZ_MARSHAL_ADAPT_HPP
#define IRIS_ZZ_MARSHAL_ADAPT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp> // IWYU pragma: keep
#include <iris/marshal/detail/field.hpp> // IWYU pragma: keep

#include <iris/alloy/adapt.hpp> // IWYU pragma: export

#include <iris/pp/tuple.hpp>
#include <iris/pp/stringize.hpp>
#include <iris/pp/cat.hpp>
#include <iris/pp/arg.hpp>

#include <type_traits>

namespace iris::marshal {

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

} // detail


#define IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name) field_name ## _

#define IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, ...) \
    private: \
        IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type) IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name){__VA_ARGS__};

#define IRIS_ZZ_MARSHAL_FIELD_GETTER(maybe_paren_type, field_name) \
    public: \
        [[nodiscard]] constexpr iris::marshal::detail::getter_return_t<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)> get_ ## field_name () const noexcept \
        { \
            return IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name); \
        } \

#define IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(maybe_paren_type, field_name) \
    public: \
        [[nodiscard]] constexpr bool is_ ## field_name () const noexcept \
        { \
            return IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name); \
        } \
    \
    private: \
        [[nodiscard]] constexpr bool get_ ## field_name () const noexcept \
        { \
            return IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name); \
        }

#define IRIS_ZZ_MARSHAL_FIELD_SETTER(maybe_paren_type, field_name) \
    public: \
        constexpr void set_ ## field_name (iris::marshal::detail::setter_param_t<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)> new_value) \
            noexcept(std::is_nothrow_copy_assignable_v<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)>) \
        { \
            IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name) = new_value; \
        }


// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_ZZ_MARSHAL_FIELD_BOOL_GET(maybe_paren_type, field_name, default_value) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, default_value) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(maybe_paren_type, field_name)

// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET(maybe_paren_type, field_name, default_value) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, default_value) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(maybe_paren_type, field_name) \
    IRIS_ZZ_MARSHAL_FIELD_SETTER(maybe_paren_type, field_name)

// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_ZZ_MARSHAL_FIELD_BOOL(maybe_paren_type, field_name, default_value) \
    IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET(maybe_paren_type, field_name, default_value)


#define IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_bool (bool)

#define IRIS_ZZ_MARSHAL_FIELD_GET_I(maybe_paren_type, field_name, ...) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, __VA_ARGS__) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER(maybe_paren_type, field_name)

#define IRIS_MARSHAL_FIELD_GET(maybe_paren_type, field_name, ...) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_I \
    ) (maybe_paren_type, field_name, __VA_ARGS__)

#define IRIS_ZZ_MARSHAL_FIELD_GET_SET_I(maybe_paren_type, field_name, ...) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, __VA_ARGS__) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER(maybe_paren_type, field_name) \
    IRIS_ZZ_MARSHAL_FIELD_SETTER(maybe_paren_type, field_name)

#define IRIS_MARSHAL_FIELD_GET_SET(maybe_paren_type, field_name, ...) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_SET_I \
    ) (maybe_paren_type, field_name, __VA_ARGS__)

#define IRIS_MARSHAL_FIELD(maybe_paren_type, field_name, ...) \
    IRIS_MARSHAL_FIELD_GET_SET(maybe_paren_type, field_name, __VA_ARGS__)

#define IRIS_MARSHAL_CLASS(class_name) \
    template<class ClassT> \
    friend struct ::iris::marshal::adapted_class_traits; \
    \
    [[nodiscard]] constexpr bool operator==(class_name const&) const noexcept = default;

// ------------------------------------------------------------

// Select `decltype(class_name::field_name)` if it exists; otherwise `decltype(class_name::field_name##_)`
#define IRIS_ZZ_MARSHAL_SELECT_MEMBER(class_name, field_name) []<class ClassT>() consteval { \
        if constexpr (requires { &ClassT::field_name; }) { \
            return std::type_identity<decltype(ClassT::field_name)>{}; \
        } else { \
            return std::type_identity<decltype(ClassT::IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name))>{}; \
        } \
    }.template operator()<class_name>()

// Select `&class_name::field_name` if it exists; otherwise `&class_name::(get|set)_field_name`
#define IRIS_ZZ_MARSHAL_SELECT_MEMBER_ACCESS(class_name, field_name, access) []<class ClassT>() consteval { \
        if constexpr (requires { &ClassT::field_name; }) { \
            return &ClassT::field_name; \
        } else { \
            return &ClassT::IRIS_PP_CAT(access ## _, field_name); \
        } \
    }.template operator()<class_name>()

#define IRIS_ZZ_MARSHAL_ADAPT_FIELD(field_name, class_name) \
    ::iris::marshal::detail::field_definition< \
        typename decltype(IRIS_ZZ_MARSHAL_SELECT_MEMBER(class_name, field_name))::type, \
        IRIS_ZZ_MARSHAL_SELECT_MEMBER_ACCESS(class_name, field_name, get), \
        IRIS_ZZ_MARSHAL_SELECT_MEMBER_ACCESS(class_name, field_name, set) \
    >{IRIS_PP_STRINGIZE(field_name)},

#define IRIS_MARSHAL_ADAPT(class_name, ...) \
    template<> \
    struct iris::marshal::adapted_class_traits<class_name> \
    { \
        inline static constexpr auto fields = ::iris::alloy::tuple{ \
            IRIS_PP_SEQ_FOR_EACH( \
                IRIS_PP_TUPLE_TO_SEQ((__VA_ARGS__)), \
                IRIS_ZZ_MARSHAL_ADAPT_FIELD, \
                class_name \
            ) \
        }; \
    };

} // iris::marshal

#endif

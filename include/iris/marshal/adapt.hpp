#ifndef IRIS_ZZ_MARSHAL_ADAPT_HPP
#define IRIS_ZZ_MARSHAL_ADAPT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp> // IWYU pragma: keep
#include <iris/marshal/detail/field.hpp> // IWYU pragma: keep

#include <iris/alloy/adapt.hpp> // IWYU pragma: export

#include <iris/pp/tuple.hpp>
#include <iris/pp/seq.hpp>
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


#define IRIS_ZZ_MARSHAL_FIELD_EXPAND_CLASS_QUALIFIED_SCOPE(class_name) class_name::
#define IRIS_ZZ_MARSHAL_FIELD_EXPAND_DECLARE_ONLY(...) ;
#define IRIS_ZZ_MARSHAL_FIELD_EXPAND_BODY(...) __VA_ARGS__

#define IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name) field_name ## _

#define IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, ...) \
    private: \
        IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type) IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME(field_name){__VA_ARGS__};

#define IRIS_ZZ_MARSHAL_FIELD_GETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    access_mod_macro(public:) \
        [[nodiscard]] iris::marshal::detail::getter_return_t<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)> \
        qualified_scope_macro(class_name) IRIS_PP_CAT(get_, field_name) () const noexcept \
        body_macro({ \
            return variable_name_macro(field_name); \
        })

#define IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    access_mod_macro(public:) \
        [[nodiscard]] bool qualified_scope_macro(class_name) IRIS_PP_CAT(is_, field_name) () const noexcept \
        body_macro({ \
            return variable_name_macro(field_name); \
        }) \
    \
    access_mod_macro(private:) \
        [[nodiscard]] bool qualified_scope_macro(class_name) IRIS_PP_CAT(get_, field_name) () const noexcept \
        body_macro({ \
            return variable_name_macro(field_name); \
        })

#define IRIS_ZZ_MARSHAL_FIELD_SETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    access_mod_macro(public:) \
        void qualified_scope_macro(class_name) IRIS_PP_CAT(set_, field_name) (iris::marshal::detail::setter_param_t<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)> new_value) \
            noexcept(std::is_nothrow_copy_assignable_v<IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)>) \
        body_macro({ \
            variable_name_macro(field_name) = new_value; \
        })


// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_ZZ_MARSHAL_FIELD_BOOL_GET(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro)

// Default value for bool field is mandatory because it is error prone if omitted
#define IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER_BOOL(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_SETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro)


#define IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_bool (bool)

#define IRIS_ZZ_MARSHAL_FIELD_GET_I(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro)

#define IRIS_MARSHAL_FIELD_GET(maybe_paren_type, field_name, ...) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, __VA_ARGS__) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_I \
    ) (~, maybe_paren_type, field_name, IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME, IRIS_ZZ_MARSHAL_FIELD_EXPAND_BODY, IRIS_PP_IDENTITY, IRIS_PP_EMPTY)

#define IRIS_ZZ_MARSHAL_FIELD_GET_SET_I(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_GETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro) \
    IRIS_ZZ_MARSHAL_FIELD_SETTER(class_name, maybe_paren_type, field_name, variable_name_macro, body_macro, access_mod_macro, qualified_scope_macro)

#define IRIS_MARSHAL_FIELD_GET_SET(maybe_paren_type, field_name, ...) \
    IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, __VA_ARGS__) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_SET_I \
    ) (~, maybe_paren_type, field_name, IRIS_ZZ_MARSHAL_FIELD_DATA_MEMBER_NAME, IRIS_ZZ_MARSHAL_FIELD_EXPAND_BODY, IRIS_PP_IDENTITY, IRIS_PP_EMPTY)

#define IRIS_MARSHAL_FIELD(maybe_paren_type, field_name, ...) \
    IRIS_MARSHAL_FIELD_GET_SET(maybe_paren_type, field_name, __VA_ARGS__)


#define IRIS_ZZ_MARSHAL_CLASS_COMMON(class_name) \
    public: \
        template<class ClassT> \
        friend struct ::iris::marshal::adapted_class_traits;

#define IRIS_MARSHAL_CLASS(class_name) \
    IRIS_ZZ_MARSHAL_CLASS_COMMON(class_name) \
    public: \
        [[nodiscard]] constexpr bool operator==(class_name const&) const noexcept = default;

#define IRIS_MARSHAL_PIMPL_CLASS(class_name) \
    IRIS_ZZ_MARSHAL_CLASS_COMMON(class_name) \
    public: \
        class_name(); \
        class_name(class_name const&) = delete; \
        class_name(class_name&&) noexcept; \
        ~class_name(); \
    \
        class_name& operator=(class_name const&) = delete; \
        class_name& operator=(class_name&&) noexcept; \
    \
    private: \
        struct Impl; \
        std::unique_ptr<Impl> impl_; \
    \
    public:

// ------------------------------------------------------------
// pimpl declare

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_DECLARE_FIELD_I(maybe_paren_type, field_name, ...) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_SET_I \
    ) (~, maybe_paren_type, field_name, IRIS_PP_EMPTY, IRIS_ZZ_MARSHAL_FIELD_EXPAND_DECLARE_ONLY, IRIS_PP_IDENTITY, IRIS_PP_EMPTY)

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_DECLARE_I(field_def, macro_data) \
    IRIS_ZZ_MARSHAL_PIMPL_FIELD_DECLARE_FIELD_I field_def

#define IRIS_MARSHAL_PIMPL_FIELD_DECLARE(field_defs) \
    IRIS_PP_SEQ_FOR_EACH(field_defs, IRIS_ZZ_MARSHAL_PIMPL_FIELD_DECLARE_I, ~)

// ------------------------------------------------------------
// pimpl define

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_DATA_MEMBER(maybe_paren_type, field_name, ...) \
    public: \
        IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type) field_name{__VA_ARGS__};

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_DEFS_I(field_def, macro_data) \
    IRIS_ZZ_MARSHAL_PIMPL_FIELD_DATA_MEMBER field_def

#define IRIS_MARSHAL_PIMPL_FIELD_DEFS(field_defs) \
    IRIS_PP_SEQ_FOR_EACH(field_defs, IRIS_ZZ_MARSHAL_PIMPL_FIELD_DEFS_I, ~)


#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_VARIABLE(field_name) impl_->field_name

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_ACCESS_DEFINE_FIELD_I(class_name, maybe_paren_type, field_name, ...) \
    IRIS_PP_IF( \
        IRIS_PP_IS_PAREN( IRIS_PP_CAT_ONLY_TWO(IRIS_ZZ_MARSHAL_FIELD_TYPE_IS_, IRIS_PP_UNPAREN_IF_PAREN(maybe_paren_type)) ), \
        IRIS_ZZ_MARSHAL_FIELD_BOOL_GET_SET, \
        IRIS_ZZ_MARSHAL_FIELD_GET_SET_I \
    ) (class_name, maybe_paren_type, field_name, IRIS_ZZ_MARSHAL_PIMPL_FIELD_VARIABLE, IRIS_ZZ_MARSHAL_FIELD_EXPAND_BODY, IRIS_PP_EMPTY, IRIS_ZZ_MARSHAL_FIELD_EXPAND_CLASS_QUALIFIED_SCOPE)

#define IRIS_ZZ_CONCAT_TUPLE_1_3_II(class_name, ...) (class_name, __VA_ARGS__)

#define IRIS_ZZ_CONCAT_TUPLE_1_3_I(class_name, ...) \
    IRIS_ZZ_CONCAT_TUPLE_1_3_II(class_name, __VA_ARGS__)

#define IRIS_ZZ_CONCAT_TUPLE_1_3(class_name, field_def) \
    IRIS_ZZ_CONCAT_TUPLE_1_3_I(class_name, IRIS_PP_EXPAND field_def)

#define IRIS_ZZ_MARSHAL_PIMPL_INVOKE(...) __VA_ARGS__

#define IRIS_ZZ_MARSHAL_PIMPL_FIELD_ACCESS_DEFINE_I(field_def, class_name) \
    IRIS_ZZ_MARSHAL_PIMPL_INVOKE( \
        IRIS_ZZ_MARSHAL_PIMPL_FIELD_ACCESS_DEFINE_FIELD_I \
        IRIS_ZZ_CONCAT_TUPLE_1_3(class_name, field_def) \
    )

#define IRIS_MARSHAL_PIMPL_FIELD_ACCESS_DEFINE(class_name, field_defs) \
    IRIS_PP_SEQ_FOR_EACH(field_defs, IRIS_ZZ_MARSHAL_PIMPL_FIELD_ACCESS_DEFINE_I, class_name)


#define IRIS_MARSHAL_PIMPL_CLASS_DEFINE(class_name) \
    class_name::class_name() : impl_(std::make_unique<Impl>()) {} \
    class_name::class_name(class_name&&) noexcept = default; \
    class_name::~class_name() = default; \
    class_name& class_name::operator=(class_name&&) noexcept = default;

// ------------------------------------------------------------

#define IRIS_ZZ_MARSHAL_SELECT_FIELD_TYPE(class_name, field_name) typename decltype([]<class ClassT>() consteval { \
        if constexpr (requires { &ClassT::field_name; }) { \
            return std::type_identity<decltype(ClassT::field_name)>{}; \
        } else { \
            return std::type_identity<std::remove_cvref_t<decltype(std::invoke(&ClassT::IRIS_PP_CAT(get_, field_name), std::declval<ClassT const&>()))>>{}; \
        } \
    }.template operator()<class_name>())::type

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
        IRIS_ZZ_MARSHAL_SELECT_FIELD_TYPE(class_name, field_name), \
        IRIS_ZZ_MARSHAL_SELECT_MEMBER_ACCESS(class_name, field_name, get), \
        IRIS_ZZ_MARSHAL_SELECT_MEMBER_ACCESS(class_name, field_name, set) \
    >{IRIS_PP_STRINGIZE(field_name)},

#define IRIS_ZZ_MARSHAL_ADAPT_I(class_name, field_defs) \
    template<> \
    struct iris::marshal::adapted_class_traits<class_name> \
    { \
        inline static constexpr auto fields = ::iris::alloy::tuple{ \
            IRIS_PP_SEQ_FOR_EACH( \
                field_defs, \
                IRIS_ZZ_MARSHAL_ADAPT_FIELD, \
                class_name \
            ) \
        }; \
    };

#define IRIS_MARSHAL_ADAPT(class_name, ...) \
    IRIS_ZZ_MARSHAL_ADAPT_I( \
        class_name, \
        IRIS_PP_TUPLE_TO_SEQ((__VA_ARGS__)) \
    )

#define IRIS_ZZ_MARSHAL_ADAPT_DEFS_EXTRACT_FIELD_NAME(field_def, macro_data) \
    (IRIS_PP_TUPLE_ELEM(1, field_def))

#define IRIS_MARSHAL_ADAPT_DEFS(class_name, field_defs) \
    IRIS_ZZ_MARSHAL_ADAPT_I( \
        class_name, \
        IRIS_PP_SEQ_FOR_EACH(field_defs, IRIS_ZZ_MARSHAL_ADAPT_DEFS_EXTRACT_FIELD_NAME, ~) \
    )

} // iris::marshal

#endif

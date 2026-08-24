#ifndef IRIS_ZZ_ALLOY_ADAPT_HPP
#define IRIS_ZZ_ALLOY_ADAPT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_traits.hpp> // IWYU pragma: keep

#include <iris/pp/comma.hpp>
#include <iris/pp/seq.hpp>
#include <iris/pp/tuple.hpp>

namespace iris::alloy {

template<class T>
struct adaptor;

}  // iris::alloy

#define IRIS_ZZ_ALLOY_ADAPT_STRUCT_I(index, data_member, class_name) \
    IRIS_PP_COMMA_IF(index) & class_name::data_member

#define IRIS_ALLOY_ADAPT_STRUCT(class_name, ...) \
    template<> \
    struct iris::alloy::adaptor<class_name> \
    { \
        using getters_list = iris::constant_list< \
            IRIS_PP_SEQ_FOR_EACH_WITH_INDEX( \
                IRIS_PP_TUPLE_TO_SEQ((__VA_ARGS__)), \
                IRIS_ZZ_ALLOY_ADAPT_STRUCT_I, \
                class_name \
            ) \
        >; \
    };

#endif

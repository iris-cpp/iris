#ifndef IRIS_ZZ_ALLOY_ADAPT_HPP
#define IRIS_ZZ_ALLOY_ADAPT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_list.hpp> // IWYU pragma: keep

#include <iris/pp/va.hpp>

namespace iris::alloy {

template<class T>
struct adaptor;

}  // iris::alloy

#define IRIS_ZZ_ALLOY_ADAPT_STRUCT_I(data_member, class_name) & class_name::data_member

#define IRIS_ALLOY_ADAPT_STRUCT(class_name, ...) \
    template<> \
    struct iris::alloy::adaptor<class_name> \
    { \
        using getters_list = iris::constant_list< \
            IRIS_PP_VA_ENUM(IRIS_ZZ_ALLOY_ADAPT_STRUCT_I, class_name, __VA_ARGS__) \
        >; \
    };

#endif

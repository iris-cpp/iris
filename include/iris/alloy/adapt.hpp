#ifndef IRIS_ZZ_ALLOY_ADAPT_HPP
#define IRIS_ZZ_ALLOY_ADAPT_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <iris/pp/comma.hpp>
#include <iris/pp/seq.hpp>
#include <iris/pp/tuple.hpp>

namespace iris::alloy {

namespace detail {

template<auto... Vs>
struct non_type_list;

}  // detail

template<class T>
struct adaptor;

template<auto... Getters>
using make_getters_list = detail::non_type_list<Getters...>;

}  // iris::alloy

#define IRIS_ALLOY_ADAPT_STRUCT(class_name, ...)                                                                                                         \
    template<>                                                                                                                                             \
    struct iris::alloy::adaptor<class_name> {                                                                                                              \
      using getters_list = make_getters_list<IRIS_PP_SEQ_FOR_EACH_WITH_INDEX(IRIS_PP_TUPLE_TO_SEQ((__VA_ARGS__)), IRIS_ALLOY_ADAPT_STRUCT_I, class_name)>; \
    };

#define IRIS_ALLOY_ADAPT_STRUCT_I(index, data_member, class_name) IRIS_PP_COMMA_IF(index) & class_name::data_member

#endif

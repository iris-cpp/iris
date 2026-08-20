#ifndef IRIS_ZZ_ALLOY_DETAIL_INTEGER_SEQ_TRANSFORM_HPP
#define IRIS_ZZ_ALLOY_DETAIL_INTEGER_SEQ_TRANSFORM_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <utility>

namespace iris::alloy::detail {

template<auto... Vs>
struct non_type_list;

template<class IntegerSeq, template<typename IntegerSeq::value_type> class F>
struct integer_seq_transform;

template<class T, T... Is, template<T> class F>
struct integer_seq_transform<std::integer_sequence<T, Is...>, F>
{
    using type = detail::non_type_list<F<Is>::value...>;
};

template<class IntegerSeq, template<typename IntegerSeq::value_type> class F>
using integer_seq_transform_t = typename integer_seq_transform<IntegerSeq, F>::type;

} // iris::alloy::detail

#endif

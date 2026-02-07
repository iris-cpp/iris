// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/type_traits.hpp>

namespace {

template<class... Ts>
struct tuple;

template<class... Ts>
struct type_list;


template<int... Ns>
struct n_tuple;

template<int... Ns>
struct n_list;

} // anonymous


TEST_CASE("specialization_of")
{
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<>, tuple>);
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<int>, tuple>);
    STATIC_CHECK(iris::is_ttp_specialization_of_v<tuple<int, double>, tuple>);

    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<>, type_list>);
    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<int>, type_list>);
    STATIC_CHECK(!iris::is_ttp_specialization_of_v<tuple<int, double>, type_list>);

    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<>, n_tuple>);
    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<0>, n_tuple>);
    STATIC_CHECK(iris::is_ctp_specialization_of_v<n_tuple<0, 1>, n_tuple>);

    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<>, n_list>);
    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<0>, n_list>);
    STATIC_CHECK(!iris::is_ctp_specialization_of_v<n_tuple<0, 1>, n_list>);
}

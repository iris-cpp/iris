// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/type_list.hpp>

#include <concepts>

using iris::type_list;
using iris::concat_type_list;

TEST_CASE("type_list")
{
    STATIC_CHECK(std::same_as<concat_type_list<>::type, type_list<>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>>::type, type_list<>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>, type_list<>>::type, type_list<>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>, type_list<>, type_list<>>::type, type_list<>>);

    STATIC_CHECK(std::same_as<concat_type_list<type_list<int>, type_list<>>::type, type_list<int>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>, type_list<int>>::type, type_list<int>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<int>, type_list<int>>::type, type_list<int, int>>);

    STATIC_CHECK(std::same_as<concat_type_list<type_list<int>, type_list<>, type_list<>>::type, type_list<int>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>, type_list<int>, type_list<>>::type, type_list<int>>);
    STATIC_CHECK(std::same_as<concat_type_list<type_list<>, type_list<>, type_list<int>>::type, type_list<int>>);
}

TEST_CASE("pack_indexing")
{
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, int>, int>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, int, float>, int>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<1, int, float>, float>);

    // make sure non-object and non-referenceable type is working
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, void>, void>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, void, int>, void>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<1, void, int>, int>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, int, void>, int>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<1, int, void>, void>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<0, void, void>, void>);
    STATIC_CHECK(std::same_as<iris::pack_indexing_t<1, void, void>, void>);

    STATIC_CHECK(std::same_as<iris::at_c_t<0, type_list<int>>, int>);
    STATIC_CHECK(std::same_as<iris::at_c_t<0, type_list<int, float>>, int>);
    STATIC_CHECK(std::same_as<iris::at_c_t<1, type_list<int, float>>, float>);
}

TEST_CASE("exactly_once")
{
    STATIC_CHECK(iris::exactly_once_v<int, type_list<int, float>>);
    STATIC_CHECK_FALSE(iris::exactly_once_v<int, type_list<int, int>>);
}

TEST_CASE("is_in")
{
    STATIC_CHECK(iris::is_in_v<int, int, float>);
    STATIC_CHECK(!iris::is_in_v<int, float>);
}

TEST_CASE("find_index")
{
    STATIC_CHECK(iris::find_index_v<int,    type_list<int, float, double>> == 0);
    STATIC_CHECK(iris::find_index_v<float,  type_list<int, float, double>> == 1);
    STATIC_CHECK(iris::find_index_v<double, type_list<int, float, double>> == 2);
    STATIC_CHECK(iris::find_index_v<int,    type_list<float, double>> == iris::find_npos);

    STATIC_CHECK(iris::find_index_v<int, type_list<int, int, double>> == 0);
}

// SPDX-License-Identifier: MIT

#include <iris/pp/add.hpp>
#include <iris/pp/bool.hpp>
#include <iris/pp/cat.hpp>
#include <iris/pp/decrement.hpp>
#include <iris/pp/for.hpp>
#include <iris/pp/if.hpp>
#include <iris/pp/increment.hpp>
#include <iris/pp/not_equal.hpp>
#include <iris/pp/repeat.hpp>
#include <iris/pp/seq.hpp>
#include <iris/pp/stringize.hpp>
#include <iris/pp/sub.hpp>
#include <iris/pp/tuple.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace std::string_view_literals;

#define IRIS_TEST_REPEAT_DECLARE_VAR(index, name) constexpr int IRIS_PP_CAT(name, index) = index;

TEST_CASE("repeat", "[preprocess]")
{
    IRIS_PP_REPEAT(5, IRIS_TEST_REPEAT_DECLARE_VAR, foo)
    STATIC_CHECK(foo0 == 0);
    STATIC_CHECK(foo1 == 1);
    STATIC_CHECK(foo2 == 2);
    STATIC_CHECK(foo3 == 3);
    STATIC_CHECK(foo4 == 4);

    IRIS_PP_REPEAT_FROM_TO(3, 5, IRIS_TEST_REPEAT_DECLARE_VAR, bar)
    STATIC_CHECK(bar3 == 3);
    STATIC_CHECK(bar4 == 4);
}

TEST_CASE("tuple", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_TUPLE_SIZE(()) == 0);
    STATIC_CHECK(IRIS_PP_TUPLE_SIZE((a1)) == 1);
    STATIC_CHECK(IRIS_PP_TUPLE_SIZE((a1, a2)) == 2);
    STATIC_CHECK(IRIS_PP_TUPLE_SIZE((a1, a2, a3)) == 3);

    {
        STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_ELEM(0, (a, b, c))) == "a"sv);
        STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_ELEM(1, (a, b, c))) == "b"sv);
        STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_ELEM(2, (a, b, c))) == "c"sv);
    }
}

TEST_CASE("if", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_IF(0, true, false) == false);
    STATIC_CHECK(IRIS_PP_IF(1, true, false) == true);
    STATIC_CHECK(IRIS_PP_IF(2, true, false) == true);
    STATIC_CHECK(IRIS_PP_IF(3, true, false) == true);
}

TEST_CASE("not_equal", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_NOT_EQUAL(0, 0) == 0);
    STATIC_CHECK(IRIS_PP_NOT_EQUAL(0, 1) == 1);
    STATIC_CHECK(IRIS_PP_NOT_EQUAL(1, 0) == 1);
    STATIC_CHECK(IRIS_PP_NOT_EQUAL(1, 1) == 0);
}

TEST_CASE("increment", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_INCREMENT(0) == 1);
    STATIC_CHECK(IRIS_PP_INCREMENT(1) == 2);
    STATIC_CHECK(IRIS_PP_INCREMENT(2) == 3);
}

TEST_CASE("decrement", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_DECREMENT(3) == 2);
    STATIC_CHECK(IRIS_PP_DECREMENT(2) == 1);
    STATIC_CHECK(IRIS_PP_DECREMENT(1) == 0);
}

#define IRIS_TEST_FOR_PRED(state) IRIS_PP_NOT_EQUAL(IRIS_PP_TUPLE_ELEM(0, state), 5)
#define IRIS_TEST_FOR_UPDATE(state) (IRIS_PP_INCREMENT(IRIS_PP_TUPLE_ELEM(0, state)), IRIS_PP_TUPLE_ELEM(1, state))
#define IRIS_TEST_FOR_USE_STATE(state) constexpr int IRIS_PP_CAT(IRIS_PP_TUPLE_ELEM(1, state), IRIS_PP_TUPLE_ELEM(0, state)) = IRIS_PP_TUPLE_ELEM(0, state);

TEST_CASE("for", "[preprocess]")
{
    IRIS_PP_FOR((0, foo), IRIS_TEST_FOR_PRED, IRIS_TEST_FOR_UPDATE, IRIS_TEST_FOR_USE_STATE);
    STATIC_CHECK(foo0 == 0);
    STATIC_CHECK(foo1 == 1);
    STATIC_CHECK(foo2 == 2);
    STATIC_CHECK(foo3 == 3);
    STATIC_CHECK(foo4 == 4);
}

#define IRIS_TEST_SEQ_EXEC(elem, data) constexpr int IRIS_PP_CAT(data, elem) = elem;

TEST_CASE("seq", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_SEQ_SIZE() == 0);
    STATIC_CHECK(IRIS_PP_SEQ_SIZE((a)) == 1);
    STATIC_CHECK(IRIS_PP_SEQ_SIZE((a)(b)) == 2);
    STATIC_CHECK(IRIS_PP_SEQ_SIZE((a)(b)(c)) == 3);

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ELEM(0, (a)(b)(c))) == "a"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ELEM(1, (a)(b)(c))) == "b"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ELEM(2, (a)(b)(c))) == "c"sv);

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_HEAD((a)(b)(c))) == "a"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_TAIL((a)(b)(c))) == "(b)(c)"sv);

    IRIS_PP_SEQ_FOR_EACH((0)(1)(2), IRIS_TEST_SEQ_EXEC, foo)

    STATIC_CHECK(foo0 == 0);
    STATIC_CHECK(foo1 == 1);
    STATIC_CHECK(foo2 == 2);
}

TEST_CASE("while", "[preprocess]")
{
    // TODO
}

TEST_CASE("add", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_ADD(0, 0) == 0);
    STATIC_CHECK(IRIS_PP_ADD(0, 1) == 1);
    STATIC_CHECK(IRIS_PP_ADD(1, 0) == 1);
    STATIC_CHECK(IRIS_PP_ADD(1, 1) == 2);

    STATIC_CHECK(IRIS_PP_ADD(3, 2) == 5);
}

TEST_CASE("sub", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_SUB(0, 0) == 0);
    STATIC_CHECK(IRIS_PP_SUB(1, 0) == 1);
    STATIC_CHECK(IRIS_PP_SUB(1, 1) == 0);

    STATIC_CHECK(IRIS_PP_SUB(3, 2) == 1);
}



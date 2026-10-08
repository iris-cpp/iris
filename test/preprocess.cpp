// SPDX-License-Identifier: MIT

#include <iris/pp/add.hpp>
#include <iris/pp/bool.hpp>
#include <iris/pp/cat.hpp>
#include <iris/pp/comma.hpp>
#include <iris/pp/decrement.hpp>
#include <iris/pp/for.hpp>
#include <iris/pp/if.hpp>
#include <iris/pp/increment.hpp>
#include <iris/pp/not_equal.hpp>
#include <iris/pp/rec.hpp>
#include <iris/pp/repeat.hpp>
#include <iris/pp/seq.hpp>
#include <iris/pp/stringize.hpp>
#include <iris/pp/sub.hpp>
#include <iris/pp/tuple.hpp>
#include <iris/pp/va.hpp>
#include <iris/pp/while.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iterator>

using namespace std::string_view_literals;

#define IRIS_TEST_REPEAT_INNER(j, i) constexpr int IRIS_PP_CAT(IRIS_PP_CAT(IRIS_PP_CAT(baz_, i), _), j) = i * 10 + j;
#define IRIS_TEST_REPEAT_OUTER(i, d) IRIS_PP_REPEAT(i, IRIS_TEST_REPEAT_INNER, i)

#define IRIS_TEST_REPEAT_DECLARE_VAR(index, name) constexpr int IRIS_PP_CAT(name, index) = index;

#define IRIS_TEST_REPEAT_LEVEL_3(k, ij) constexpr int IRIS_PP_CAT(IRIS_PP_CAT(qux_, ij), k) = 1;
#define IRIS_TEST_REPEAT_LEVEL_2(j, i) IRIS_PP_REPEAT(1, IRIS_TEST_REPEAT_LEVEL_3, IRIS_PP_CAT(IRIS_PP_CAT(i, _), j))
#define IRIS_TEST_REPEAT_LEVEL_1(i, d) IRIS_PP_REPEAT(1, IRIS_TEST_REPEAT_LEVEL_2, i)

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

    IRIS_PP_REPEAT(3, IRIS_TEST_REPEAT_OUTER, data)
    STATIC_CHECK(baz_1_0 == 10);
    STATIC_CHECK(baz_2_0 == 20);
    STATIC_CHECK(baz_2_1 == 21);

    IRIS_PP_REPEAT_FROM_TO(1, 3, IRIS_TEST_REPEAT_LEVEL_1, data)
    STATIC_CHECK(qux_1_00 == 1);
    STATIC_CHECK(qux_2_00 == 1);
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
        STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_ELEM(1, (a, (b, c), d))) == "(b, c)"sv);
        STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_ELEM(31, (
            a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15,
            a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31
        ))) == "a31"sv);
    }

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_TUPLE_TO_SEQ((a, b))) == "(a)(b)"sv);
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

#define IRIS_TEST_SEQ_EXEC(elem, data) constexpr std::string_view IRIS_PP_CAT(data, elem) = IRIS_PP_STRINGIZE(elem);
#define IRIS_TEST_SEQ_EXEC2(index, elem, data) constexpr int IRIS_PP_CAT(data, elem) = index;
#define IRIS_TEST_SEQ_EXEC_WHILE(elem, data) constexpr int IRIS_PP_CAT(data, elem) = IRIS_PP_WHILE(0, IRIS_TEST_WHILE_NEQ_5, IRIS_PP_INCREMENT);
#define IRIS_TEST_WHILE_NEQ_5(expr) IRIS_PP_NOT_EQUAL(expr, 5)

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

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ENUM()) == ""sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ENUM((a))) == "a"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ENUM((a)(b)(c))) == "a, b, c"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_SEQ_ENUM(((a, b))(c))) == "(a, b), c"sv);

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_VARIADIC_SEQ_TO_SEQ()) == ""sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_VARIADIC_SEQ_TO_SEQ((a))) == "((a))"sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_VARIADIC_SEQ_TO_SEQ((a, b)(c)(d, e, f))) == "((a, b)) ((c)) ((d, e, f))"sv);
    STATIC_CHECK(IRIS_PP_SEQ_SIZE(IRIS_PP_VARIADIC_SEQ_TO_SEQ((a, b)(c)(d, e, f))) == 3);

    IRIS_PP_SEQ_FOR_EACH((a)(b)(c), IRIS_TEST_SEQ_EXEC, foo)

    STATIC_CHECK(fooa == "a");
    STATIC_CHECK(foob == "b");
    STATIC_CHECK(fooc == "c");

    IRIS_PP_SEQ_FOR_EACH_WITH_INDEX((a)(b)(c), IRIS_TEST_SEQ_EXEC2, bar)

    STATIC_CHECK(bara == 0);
    STATIC_CHECK(barb == 1);
    STATIC_CHECK(barc == 2);

    IRIS_PP_SEQ_FOR_EACH(, IRIS_TEST_SEQ_EXEC, unused)
    IRIS_PP_SEQ_FOR_EACH_WITH_INDEX(, IRIS_TEST_SEQ_EXEC2, unused)

    IRIS_PP_SEQ_FOR_EACH((a)(b), IRIS_TEST_SEQ_EXEC_WHILE, baz)
    STATIC_CHECK(baza == 5);
    STATIC_CHECK(bazb == 5);
}

#define IRIS_TEST_VA_EXEC(elem, data) constexpr int IRIS_PP_CAT(data, elem) = 1;
#define IRIS_TEST_VA_ENUM(elem, data) data + elem

TEST_CASE("va", "[preprocess]")
{
    IRIS_PP_VA_FOR_EACH(IRIS_TEST_VA_EXEC, foo, a, b, c)
    STATIC_CHECK(fooa == 1);
    STATIC_CHECK(foob == 1);
    STATIC_CHECK(fooc == 1);

    IRIS_PP_VA_FOR_EACH(IRIS_TEST_VA_EXEC, unused)

    constexpr int arr[]{IRIS_PP_VA_ENUM(IRIS_TEST_VA_ENUM, 10, 1, 2, 3)};
    STATIC_CHECK(std::size(arr) == 3);
    STATIC_CHECK(arr[0] == 11);
    STATIC_CHECK(arr[1] == 12);
    STATIC_CHECK(arr[2] == 13);

    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_VA_ENUM(IRIS_TEST_VA_ENUM, x)) == ""sv);
    STATIC_CHECK(IRIS_PP_STRINGIZE(IRIS_PP_VA_ENUM(IRIS_TEST_VA_ENUM, x, (a, b))) == "x + (a, b)"sv);
}

#define IRIS_TEST_REC_AVAILABLE_FROM_1(n) IRIS_PP_SUB(n, 0)
#define IRIS_TEST_REC_AVAILABLE_FROM_5(n) IRIS_PP_SUB(n, 4)
#define IRIS_TEST_REC_AVAILABLE_FROM_7(n) IRIS_PP_SUB(n, 6)
#define IRIS_TEST_REC_AVAILABLE_FROM_8(n) IRIS_PP_SUB(n, 7)

TEST_CASE("rec", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_1, 8) == 1);
    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_5, 8) == 5);
    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_7, 8) == 7);
    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_8, 8) == 8);

    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_1, 4) == 1);
    STATIC_CHECK(IRIS_PP_REC(IRIS_TEST_REC_AVAILABLE_FROM_5, 4) == 4);
}

TEST_CASE("while", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_WHILE(0, IRIS_TEST_WHILE_NEQ_5, IRIS_PP_INCREMENT) == 5);
}

TEST_CASE("add", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_ADD(0, 0) == 0);
    STATIC_CHECK(IRIS_PP_ADD(0, 1) == 1);
    STATIC_CHECK(IRIS_PP_ADD(1, 0) == 1);
    STATIC_CHECK(IRIS_PP_ADD(1, 1) == 2);

    STATIC_CHECK(IRIS_PP_ADD(3, 2) == 5);
    STATIC_CHECK(IRIS_PP_ADD(16, 16) == 32);
    STATIC_CHECK(IRIS_PP_ADD(30, 5) == 32);
}

TEST_CASE("sub", "[preprocess]")
{
    STATIC_CHECK(IRIS_PP_SUB(0, 0) == 0);
    STATIC_CHECK(IRIS_PP_SUB(1, 0) == 1);
    STATIC_CHECK(IRIS_PP_SUB(1, 1) == 0);

    STATIC_CHECK(IRIS_PP_SUB(3, 2) == 1);
    STATIC_CHECK(IRIS_PP_SUB(32, 31) == 1);
    STATIC_CHECK(IRIS_PP_SUB(2, 5) == 0);
}

#define IRIS_TEST_COMMA_IF(index, data) IRIS_PP_COMMA_IF(index) index

TEST_CASE("comma_if", "[preprocess]")
{
    constexpr int arr[]{IRIS_PP_REPEAT(3, IRIS_TEST_COMMA_IF, data)};
    STATIC_CHECK(arr[0] == 0);
    STATIC_CHECK(arr[1] == 1);
    STATIC_CHECK(arr[2] == 2);
}

#include <iris/preprocess/bool.hpp>
#include <iris/preprocess/cat.hpp>
#include <iris/preprocess/decrement.hpp>
#include <iris/preprocess/for.hpp>
#include <iris/preprocess/if.hpp>
#include <iris/preprocess/increment.hpp>
#include <iris/preprocess/not_equal.hpp>
#include <iris/preprocess/repeat.hpp>
#include <iris/preprocess/seq.hpp>
#include <iris/preprocess/stringize.hpp>
#include <iris/preprocess/tuple.hpp>

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

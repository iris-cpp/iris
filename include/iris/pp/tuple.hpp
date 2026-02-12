#ifndef IRIS_PREPROCESS_TUPLE_HPP
#define IRIS_PREPROCESS_TUPLE_HPP

#include <iris/pp/cat.hpp>

#define IRIS_PP_TUPLE_SIZE(tuple) IRIS_PP_TUPLE_SIZE_I tuple
#define IRIS_PP_TUPLE_SIZE_I(...)                                                                                                                         \
  IRIS_PP_TUPLE_SIZE_I_I(                                                                                                                                 \
      __VA_ARGS__ __VA_OPT__(, ) 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 \
  )
#define IRIS_PP_TUPLE_SIZE_I_I(                                                                                                                            \
    a32, a31, a30, a29, a28, a27, a26, a25, a24, a23, a22, a21, a20, a19, a18, a17, a16, a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, \
    size, ...                                                                                                                                              \
)                                                                                                                                                          \
  size

#define IRIS_PP_TUPLE_ELEM(index, tuple)                                                            \
  IRIS_PP_CAT(IRIS_PP_CAT(IRIS_PP_CAT(IRIS_PP_TUPLE_ELEM_I_, IRIS_PP_TUPLE_SIZE(tuple)), _), index) \
  tuple

#define IRIS_PP_TUPLE_ELEM_I_1_0(a0) a0
#define IRIS_PP_TUPLE_ELEM_I_2_0(a0, a1) a0
#define IRIS_PP_TUPLE_ELEM_I_2_1(a0, a1) a1
#define IRIS_PP_TUPLE_ELEM_I_3_0(a0, a1, a2) a0
#define IRIS_PP_TUPLE_ELEM_I_3_1(a0, a1, a2) a1
#define IRIS_PP_TUPLE_ELEM_I_3_2(a0, a1, a2) a2
#define IRIS_PP_TUPLE_ELEM_I_4_0(a0, a1, a2, a3) a0
#define IRIS_PP_TUPLE_ELEM_I_4_1(a0, a1, a2, a3) a1
#define IRIS_PP_TUPLE_ELEM_I_4_2(a0, a1, a2, a3) a2
#define IRIS_PP_TUPLE_ELEM_I_4_3(a0, a1, a2, a3) a3
#define IRIS_PP_TUPLE_ELEM_I_5_0(a0, a1, a2, a3, a4) a0
#define IRIS_PP_TUPLE_ELEM_I_5_1(a0, a1, a2, a3, a4) a1
#define IRIS_PP_TUPLE_ELEM_I_5_2(a0, a1, a2, a3, a4) a2
#define IRIS_PP_TUPLE_ELEM_I_5_3(a0, a1, a2, a3, a4) a3
#define IRIS_PP_TUPLE_ELEM_I_5_4(a0, a1, a2, a3, a4) a4
#define IRIS_PP_TUPLE_ELEM_I_6_0(a0, a1, a2, a3, a4, a5) a0
#define IRIS_PP_TUPLE_ELEM_I_6_1(a0, a1, a2, a3, a4, a5) a1
#define IRIS_PP_TUPLE_ELEM_I_6_2(a0, a1, a2, a3, a4, a5) a2
#define IRIS_PP_TUPLE_ELEM_I_6_3(a0, a1, a2, a3, a4, a5) a3
#define IRIS_PP_TUPLE_ELEM_I_6_4(a0, a1, a2, a3, a4, a5) a4
#define IRIS_PP_TUPLE_ELEM_I_6_5(a0, a1, a2, a3, a4, a5) a5
#define IRIS_PP_TUPLE_ELEM_I_7_0(a0, a1, a2, a3, a4, a5, a6) a0
#define IRIS_PP_TUPLE_ELEM_I_7_1(a0, a1, a2, a3, a4, a5, a6) a1
#define IRIS_PP_TUPLE_ELEM_I_7_2(a0, a1, a2, a3, a4, a5, a6) a2
#define IRIS_PP_TUPLE_ELEM_I_7_3(a0, a1, a2, a3, a4, a5, a6) a3
#define IRIS_PP_TUPLE_ELEM_I_7_4(a0, a1, a2, a3, a4, a5, a6) a4
#define IRIS_PP_TUPLE_ELEM_I_7_5(a0, a1, a2, a3, a4, a5, a6) a5
#define IRIS_PP_TUPLE_ELEM_I_7_6(a0, a1, a2, a3, a4, a5, a6) a6
#define IRIS_PP_TUPLE_ELEM_I_8_0(a0, a1, a2, a3, a4, a5, a6, a7) a0
#define IRIS_PP_TUPLE_ELEM_I_8_1(a0, a1, a2, a3, a4, a5, a6, a7) a1
#define IRIS_PP_TUPLE_ELEM_I_8_2(a0, a1, a2, a3, a4, a5, a6, a7) a2
#define IRIS_PP_TUPLE_ELEM_I_8_3(a0, a1, a2, a3, a4, a5, a6, a7) a3
#define IRIS_PP_TUPLE_ELEM_I_8_4(a0, a1, a2, a3, a4, a5, a6, a7) a4
#define IRIS_PP_TUPLE_ELEM_I_8_5(a0, a1, a2, a3, a4, a5, a6, a7) a5
#define IRIS_PP_TUPLE_ELEM_I_8_6(a0, a1, a2, a3, a4, a5, a6, a7) a6
#define IRIS_PP_TUPLE_ELEM_I_8_7(a0, a1, a2, a3, a4, a5, a6, a7) a7
#define IRIS_PP_TUPLE_ELEM_I_9_0(a0, a1, a2, a3, a4, a5, a6, a7, a8) a0
#define IRIS_PP_TUPLE_ELEM_I_9_1(a0, a1, a2, a3, a4, a5, a6, a7, a8) a1
#define IRIS_PP_TUPLE_ELEM_I_9_2(a0, a1, a2, a3, a4, a5, a6, a7, a8) a2
#define IRIS_PP_TUPLE_ELEM_I_9_3(a0, a1, a2, a3, a4, a5, a6, a7, a8) a3
#define IRIS_PP_TUPLE_ELEM_I_9_4(a0, a1, a2, a3, a4, a5, a6, a7, a8) a4
#define IRIS_PP_TUPLE_ELEM_I_9_5(a0, a1, a2, a3, a4, a5, a6, a7, a8) a5
#define IRIS_PP_TUPLE_ELEM_I_9_6(a0, a1, a2, a3, a4, a5, a6, a7, a8) a6
#define IRIS_PP_TUPLE_ELEM_I_9_7(a0, a1, a2, a3, a4, a5, a6, a7, a8) a7
#define IRIS_PP_TUPLE_ELEM_I_9_8(a0, a1, a2, a3, a4, a5, a6, a7, a8) a8
#define IRIS_PP_TUPLE_ELEM_I_10_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a0
#define IRIS_PP_TUPLE_ELEM_I_10_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a1
#define IRIS_PP_TUPLE_ELEM_I_10_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a2
#define IRIS_PP_TUPLE_ELEM_I_10_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a3
#define IRIS_PP_TUPLE_ELEM_I_10_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a4
#define IRIS_PP_TUPLE_ELEM_I_10_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a5
#define IRIS_PP_TUPLE_ELEM_I_10_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a6
#define IRIS_PP_TUPLE_ELEM_I_10_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a7
#define IRIS_PP_TUPLE_ELEM_I_10_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a8
#define IRIS_PP_TUPLE_ELEM_I_10_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) a9
#define IRIS_PP_TUPLE_ELEM_I_11_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a0
#define IRIS_PP_TUPLE_ELEM_I_11_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a1
#define IRIS_PP_TUPLE_ELEM_I_11_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a2
#define IRIS_PP_TUPLE_ELEM_I_11_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a3
#define IRIS_PP_TUPLE_ELEM_I_11_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a4
#define IRIS_PP_TUPLE_ELEM_I_11_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a5
#define IRIS_PP_TUPLE_ELEM_I_11_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a6
#define IRIS_PP_TUPLE_ELEM_I_11_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a7
#define IRIS_PP_TUPLE_ELEM_I_11_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a8
#define IRIS_PP_TUPLE_ELEM_I_11_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a9
#define IRIS_PP_TUPLE_ELEM_I_11_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a10
#define IRIS_PP_TUPLE_ELEM_I_12_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a0
#define IRIS_PP_TUPLE_ELEM_I_12_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a1
#define IRIS_PP_TUPLE_ELEM_I_12_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a2
#define IRIS_PP_TUPLE_ELEM_I_12_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a3
#define IRIS_PP_TUPLE_ELEM_I_12_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a4
#define IRIS_PP_TUPLE_ELEM_I_12_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a5
#define IRIS_PP_TUPLE_ELEM_I_12_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a6
#define IRIS_PP_TUPLE_ELEM_I_12_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a7
#define IRIS_PP_TUPLE_ELEM_I_12_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a8
#define IRIS_PP_TUPLE_ELEM_I_12_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a9
#define IRIS_PP_TUPLE_ELEM_I_12_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a10
#define IRIS_PP_TUPLE_ELEM_I_12_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a11
#define IRIS_PP_TUPLE_ELEM_I_13_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a0
#define IRIS_PP_TUPLE_ELEM_I_13_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a1
#define IRIS_PP_TUPLE_ELEM_I_13_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a2
#define IRIS_PP_TUPLE_ELEM_I_13_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a3
#define IRIS_PP_TUPLE_ELEM_I_13_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a4
#define IRIS_PP_TUPLE_ELEM_I_13_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a5
#define IRIS_PP_TUPLE_ELEM_I_13_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a6
#define IRIS_PP_TUPLE_ELEM_I_13_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a7
#define IRIS_PP_TUPLE_ELEM_I_13_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a8
#define IRIS_PP_TUPLE_ELEM_I_13_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a9
#define IRIS_PP_TUPLE_ELEM_I_13_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a10
#define IRIS_PP_TUPLE_ELEM_I_13_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a11
#define IRIS_PP_TUPLE_ELEM_I_13_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a12
#define IRIS_PP_TUPLE_ELEM_I_14_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a0
#define IRIS_PP_TUPLE_ELEM_I_14_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a1
#define IRIS_PP_TUPLE_ELEM_I_14_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a2
#define IRIS_PP_TUPLE_ELEM_I_14_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a3
#define IRIS_PP_TUPLE_ELEM_I_14_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a4
#define IRIS_PP_TUPLE_ELEM_I_14_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a5
#define IRIS_PP_TUPLE_ELEM_I_14_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a6
#define IRIS_PP_TUPLE_ELEM_I_14_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a7
#define IRIS_PP_TUPLE_ELEM_I_14_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a8
#define IRIS_PP_TUPLE_ELEM_I_14_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a9
#define IRIS_PP_TUPLE_ELEM_I_14_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a10
#define IRIS_PP_TUPLE_ELEM_I_14_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a11
#define IRIS_PP_TUPLE_ELEM_I_14_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a12
#define IRIS_PP_TUPLE_ELEM_I_14_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a13
#define IRIS_PP_TUPLE_ELEM_I_15_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a0
#define IRIS_PP_TUPLE_ELEM_I_15_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a1
#define IRIS_PP_TUPLE_ELEM_I_15_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a2
#define IRIS_PP_TUPLE_ELEM_I_15_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a3
#define IRIS_PP_TUPLE_ELEM_I_15_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a4
#define IRIS_PP_TUPLE_ELEM_I_15_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a5
#define IRIS_PP_TUPLE_ELEM_I_15_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a6
#define IRIS_PP_TUPLE_ELEM_I_15_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a7
#define IRIS_PP_TUPLE_ELEM_I_15_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a8
#define IRIS_PP_TUPLE_ELEM_I_15_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a9
#define IRIS_PP_TUPLE_ELEM_I_15_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a10
#define IRIS_PP_TUPLE_ELEM_I_15_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a11
#define IRIS_PP_TUPLE_ELEM_I_15_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a12
#define IRIS_PP_TUPLE_ELEM_I_15_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a13
#define IRIS_PP_TUPLE_ELEM_I_15_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a14
#define IRIS_PP_TUPLE_ELEM_I_16_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a0
#define IRIS_PP_TUPLE_ELEM_I_16_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a1
#define IRIS_PP_TUPLE_ELEM_I_16_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a2
#define IRIS_PP_TUPLE_ELEM_I_16_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a3
#define IRIS_PP_TUPLE_ELEM_I_16_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a4
#define IRIS_PP_TUPLE_ELEM_I_16_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a5
#define IRIS_PP_TUPLE_ELEM_I_16_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a6
#define IRIS_PP_TUPLE_ELEM_I_16_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a7
#define IRIS_PP_TUPLE_ELEM_I_16_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a8
#define IRIS_PP_TUPLE_ELEM_I_16_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a9
#define IRIS_PP_TUPLE_ELEM_I_16_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a10
#define IRIS_PP_TUPLE_ELEM_I_16_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a11
#define IRIS_PP_TUPLE_ELEM_I_16_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a12
#define IRIS_PP_TUPLE_ELEM_I_16_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a13
#define IRIS_PP_TUPLE_ELEM_I_16_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a14
#define IRIS_PP_TUPLE_ELEM_I_16_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a15
#define IRIS_PP_TUPLE_ELEM_I_17_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a0
#define IRIS_PP_TUPLE_ELEM_I_17_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a1
#define IRIS_PP_TUPLE_ELEM_I_17_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a2
#define IRIS_PP_TUPLE_ELEM_I_17_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a3
#define IRIS_PP_TUPLE_ELEM_I_17_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a4
#define IRIS_PP_TUPLE_ELEM_I_17_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a5
#define IRIS_PP_TUPLE_ELEM_I_17_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a6
#define IRIS_PP_TUPLE_ELEM_I_17_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a7
#define IRIS_PP_TUPLE_ELEM_I_17_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a8
#define IRIS_PP_TUPLE_ELEM_I_17_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a9
#define IRIS_PP_TUPLE_ELEM_I_17_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a10
#define IRIS_PP_TUPLE_ELEM_I_17_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a11
#define IRIS_PP_TUPLE_ELEM_I_17_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a12
#define IRIS_PP_TUPLE_ELEM_I_17_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a13
#define IRIS_PP_TUPLE_ELEM_I_17_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a14
#define IRIS_PP_TUPLE_ELEM_I_17_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a15
#define IRIS_PP_TUPLE_ELEM_I_17_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a16
#define IRIS_PP_TUPLE_ELEM_I_18_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a0
#define IRIS_PP_TUPLE_ELEM_I_18_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a1
#define IRIS_PP_TUPLE_ELEM_I_18_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a2
#define IRIS_PP_TUPLE_ELEM_I_18_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a3
#define IRIS_PP_TUPLE_ELEM_I_18_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a4
#define IRIS_PP_TUPLE_ELEM_I_18_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a5
#define IRIS_PP_TUPLE_ELEM_I_18_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a6
#define IRIS_PP_TUPLE_ELEM_I_18_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a7
#define IRIS_PP_TUPLE_ELEM_I_18_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a8
#define IRIS_PP_TUPLE_ELEM_I_18_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a9
#define IRIS_PP_TUPLE_ELEM_I_18_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a10
#define IRIS_PP_TUPLE_ELEM_I_18_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a11
#define IRIS_PP_TUPLE_ELEM_I_18_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a12
#define IRIS_PP_TUPLE_ELEM_I_18_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a13
#define IRIS_PP_TUPLE_ELEM_I_18_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a14
#define IRIS_PP_TUPLE_ELEM_I_18_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a15
#define IRIS_PP_TUPLE_ELEM_I_18_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a16
#define IRIS_PP_TUPLE_ELEM_I_18_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a17
#define IRIS_PP_TUPLE_ELEM_I_19_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a0
#define IRIS_PP_TUPLE_ELEM_I_19_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a1
#define IRIS_PP_TUPLE_ELEM_I_19_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a2
#define IRIS_PP_TUPLE_ELEM_I_19_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a3
#define IRIS_PP_TUPLE_ELEM_I_19_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a4
#define IRIS_PP_TUPLE_ELEM_I_19_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a5
#define IRIS_PP_TUPLE_ELEM_I_19_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a6
#define IRIS_PP_TUPLE_ELEM_I_19_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a7
#define IRIS_PP_TUPLE_ELEM_I_19_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a8
#define IRIS_PP_TUPLE_ELEM_I_19_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a9
#define IRIS_PP_TUPLE_ELEM_I_19_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a10
#define IRIS_PP_TUPLE_ELEM_I_19_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a11
#define IRIS_PP_TUPLE_ELEM_I_19_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a12
#define IRIS_PP_TUPLE_ELEM_I_19_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a13
#define IRIS_PP_TUPLE_ELEM_I_19_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a14
#define IRIS_PP_TUPLE_ELEM_I_19_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a15
#define IRIS_PP_TUPLE_ELEM_I_19_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a16
#define IRIS_PP_TUPLE_ELEM_I_19_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a17
#define IRIS_PP_TUPLE_ELEM_I_19_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a18
#define IRIS_PP_TUPLE_ELEM_I_20_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a0
#define IRIS_PP_TUPLE_ELEM_I_20_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a1
#define IRIS_PP_TUPLE_ELEM_I_20_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a2
#define IRIS_PP_TUPLE_ELEM_I_20_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a3
#define IRIS_PP_TUPLE_ELEM_I_20_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a4
#define IRIS_PP_TUPLE_ELEM_I_20_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a5
#define IRIS_PP_TUPLE_ELEM_I_20_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a6
#define IRIS_PP_TUPLE_ELEM_I_20_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a7
#define IRIS_PP_TUPLE_ELEM_I_20_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a8
#define IRIS_PP_TUPLE_ELEM_I_20_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a9
#define IRIS_PP_TUPLE_ELEM_I_20_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a10
#define IRIS_PP_TUPLE_ELEM_I_20_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a11
#define IRIS_PP_TUPLE_ELEM_I_20_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a12
#define IRIS_PP_TUPLE_ELEM_I_20_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a13
#define IRIS_PP_TUPLE_ELEM_I_20_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a14
#define IRIS_PP_TUPLE_ELEM_I_20_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a15
#define IRIS_PP_TUPLE_ELEM_I_20_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a16
#define IRIS_PP_TUPLE_ELEM_I_20_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a17
#define IRIS_PP_TUPLE_ELEM_I_20_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a18
#define IRIS_PP_TUPLE_ELEM_I_20_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a19
#define IRIS_PP_TUPLE_ELEM_I_21_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a0
#define IRIS_PP_TUPLE_ELEM_I_21_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a1
#define IRIS_PP_TUPLE_ELEM_I_21_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a2
#define IRIS_PP_TUPLE_ELEM_I_21_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a3
#define IRIS_PP_TUPLE_ELEM_I_21_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a4
#define IRIS_PP_TUPLE_ELEM_I_21_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a5
#define IRIS_PP_TUPLE_ELEM_I_21_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a6
#define IRIS_PP_TUPLE_ELEM_I_21_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a7
#define IRIS_PP_TUPLE_ELEM_I_21_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a8
#define IRIS_PP_TUPLE_ELEM_I_21_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a9
#define IRIS_PP_TUPLE_ELEM_I_21_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a10
#define IRIS_PP_TUPLE_ELEM_I_21_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a11
#define IRIS_PP_TUPLE_ELEM_I_21_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a12
#define IRIS_PP_TUPLE_ELEM_I_21_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a13
#define IRIS_PP_TUPLE_ELEM_I_21_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a14
#define IRIS_PP_TUPLE_ELEM_I_21_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a15
#define IRIS_PP_TUPLE_ELEM_I_21_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a16
#define IRIS_PP_TUPLE_ELEM_I_21_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a17
#define IRIS_PP_TUPLE_ELEM_I_21_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a18
#define IRIS_PP_TUPLE_ELEM_I_21_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a19
#define IRIS_PP_TUPLE_ELEM_I_21_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a20
#define IRIS_PP_TUPLE_ELEM_I_22_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a0
#define IRIS_PP_TUPLE_ELEM_I_22_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a1
#define IRIS_PP_TUPLE_ELEM_I_22_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a2
#define IRIS_PP_TUPLE_ELEM_I_22_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a3
#define IRIS_PP_TUPLE_ELEM_I_22_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a4
#define IRIS_PP_TUPLE_ELEM_I_22_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a5
#define IRIS_PP_TUPLE_ELEM_I_22_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a6
#define IRIS_PP_TUPLE_ELEM_I_22_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a7
#define IRIS_PP_TUPLE_ELEM_I_22_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a8
#define IRIS_PP_TUPLE_ELEM_I_22_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a9
#define IRIS_PP_TUPLE_ELEM_I_22_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a10
#define IRIS_PP_TUPLE_ELEM_I_22_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a11
#define IRIS_PP_TUPLE_ELEM_I_22_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a12
#define IRIS_PP_TUPLE_ELEM_I_22_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a13
#define IRIS_PP_TUPLE_ELEM_I_22_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a14
#define IRIS_PP_TUPLE_ELEM_I_22_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a15
#define IRIS_PP_TUPLE_ELEM_I_22_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a16
#define IRIS_PP_TUPLE_ELEM_I_22_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a17
#define IRIS_PP_TUPLE_ELEM_I_22_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a18
#define IRIS_PP_TUPLE_ELEM_I_22_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a19
#define IRIS_PP_TUPLE_ELEM_I_22_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a20
#define IRIS_PP_TUPLE_ELEM_I_22_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a21
#define IRIS_PP_TUPLE_ELEM_I_23_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a0
#define IRIS_PP_TUPLE_ELEM_I_23_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a1
#define IRIS_PP_TUPLE_ELEM_I_23_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a2
#define IRIS_PP_TUPLE_ELEM_I_23_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a3
#define IRIS_PP_TUPLE_ELEM_I_23_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a4
#define IRIS_PP_TUPLE_ELEM_I_23_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a5
#define IRIS_PP_TUPLE_ELEM_I_23_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a6
#define IRIS_PP_TUPLE_ELEM_I_23_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a7
#define IRIS_PP_TUPLE_ELEM_I_23_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a8
#define IRIS_PP_TUPLE_ELEM_I_23_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a9
#define IRIS_PP_TUPLE_ELEM_I_23_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a10
#define IRIS_PP_TUPLE_ELEM_I_23_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a11
#define IRIS_PP_TUPLE_ELEM_I_23_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a12
#define IRIS_PP_TUPLE_ELEM_I_23_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a13
#define IRIS_PP_TUPLE_ELEM_I_23_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a14
#define IRIS_PP_TUPLE_ELEM_I_23_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a15
#define IRIS_PP_TUPLE_ELEM_I_23_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a16
#define IRIS_PP_TUPLE_ELEM_I_23_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a17
#define IRIS_PP_TUPLE_ELEM_I_23_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a18
#define IRIS_PP_TUPLE_ELEM_I_23_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a19
#define IRIS_PP_TUPLE_ELEM_I_23_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a20
#define IRIS_PP_TUPLE_ELEM_I_23_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a21
#define IRIS_PP_TUPLE_ELEM_I_23_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a22
#define IRIS_PP_TUPLE_ELEM_I_24_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a0
#define IRIS_PP_TUPLE_ELEM_I_24_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a1
#define IRIS_PP_TUPLE_ELEM_I_24_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a2
#define IRIS_PP_TUPLE_ELEM_I_24_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a3
#define IRIS_PP_TUPLE_ELEM_I_24_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a4
#define IRIS_PP_TUPLE_ELEM_I_24_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a5
#define IRIS_PP_TUPLE_ELEM_I_24_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a6
#define IRIS_PP_TUPLE_ELEM_I_24_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a7
#define IRIS_PP_TUPLE_ELEM_I_24_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a8
#define IRIS_PP_TUPLE_ELEM_I_24_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a9
#define IRIS_PP_TUPLE_ELEM_I_24_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a10
#define IRIS_PP_TUPLE_ELEM_I_24_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a11
#define IRIS_PP_TUPLE_ELEM_I_24_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a12
#define IRIS_PP_TUPLE_ELEM_I_24_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a13
#define IRIS_PP_TUPLE_ELEM_I_24_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a14
#define IRIS_PP_TUPLE_ELEM_I_24_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a15
#define IRIS_PP_TUPLE_ELEM_I_24_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a16
#define IRIS_PP_TUPLE_ELEM_I_24_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a17
#define IRIS_PP_TUPLE_ELEM_I_24_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a18
#define IRIS_PP_TUPLE_ELEM_I_24_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a19
#define IRIS_PP_TUPLE_ELEM_I_24_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a20
#define IRIS_PP_TUPLE_ELEM_I_24_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a21
#define IRIS_PP_TUPLE_ELEM_I_24_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a22
#define IRIS_PP_TUPLE_ELEM_I_24_23(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a23
#define IRIS_PP_TUPLE_ELEM_I_25_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a0
#define IRIS_PP_TUPLE_ELEM_I_25_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a1
#define IRIS_PP_TUPLE_ELEM_I_25_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a2
#define IRIS_PP_TUPLE_ELEM_I_25_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a3
#define IRIS_PP_TUPLE_ELEM_I_25_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a4
#define IRIS_PP_TUPLE_ELEM_I_25_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a5
#define IRIS_PP_TUPLE_ELEM_I_25_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a6
#define IRIS_PP_TUPLE_ELEM_I_25_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a7
#define IRIS_PP_TUPLE_ELEM_I_25_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a8
#define IRIS_PP_TUPLE_ELEM_I_25_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a9
#define IRIS_PP_TUPLE_ELEM_I_25_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a10
#define IRIS_PP_TUPLE_ELEM_I_25_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a11
#define IRIS_PP_TUPLE_ELEM_I_25_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a12
#define IRIS_PP_TUPLE_ELEM_I_25_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a13
#define IRIS_PP_TUPLE_ELEM_I_25_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a14
#define IRIS_PP_TUPLE_ELEM_I_25_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a15
#define IRIS_PP_TUPLE_ELEM_I_25_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a16
#define IRIS_PP_TUPLE_ELEM_I_25_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a17
#define IRIS_PP_TUPLE_ELEM_I_25_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a18
#define IRIS_PP_TUPLE_ELEM_I_25_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a19
#define IRIS_PP_TUPLE_ELEM_I_25_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a20
#define IRIS_PP_TUPLE_ELEM_I_25_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a21
#define IRIS_PP_TUPLE_ELEM_I_25_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a22
#define IRIS_PP_TUPLE_ELEM_I_25_23(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a23
#define IRIS_PP_TUPLE_ELEM_I_25_24(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a24
#define IRIS_PP_TUPLE_ELEM_I_26_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a0
#define IRIS_PP_TUPLE_ELEM_I_26_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a1
#define IRIS_PP_TUPLE_ELEM_I_26_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a2
#define IRIS_PP_TUPLE_ELEM_I_26_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a3
#define IRIS_PP_TUPLE_ELEM_I_26_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a4
#define IRIS_PP_TUPLE_ELEM_I_26_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a5
#define IRIS_PP_TUPLE_ELEM_I_26_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a6
#define IRIS_PP_TUPLE_ELEM_I_26_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a7
#define IRIS_PP_TUPLE_ELEM_I_26_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a8
#define IRIS_PP_TUPLE_ELEM_I_26_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a9
#define IRIS_PP_TUPLE_ELEM_I_26_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a10
#define IRIS_PP_TUPLE_ELEM_I_26_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a11
#define IRIS_PP_TUPLE_ELEM_I_26_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a12
#define IRIS_PP_TUPLE_ELEM_I_26_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a13
#define IRIS_PP_TUPLE_ELEM_I_26_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a14
#define IRIS_PP_TUPLE_ELEM_I_26_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a15
#define IRIS_PP_TUPLE_ELEM_I_26_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a16
#define IRIS_PP_TUPLE_ELEM_I_26_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a17
#define IRIS_PP_TUPLE_ELEM_I_26_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a18
#define IRIS_PP_TUPLE_ELEM_I_26_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a19
#define IRIS_PP_TUPLE_ELEM_I_26_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a20
#define IRIS_PP_TUPLE_ELEM_I_26_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a21
#define IRIS_PP_TUPLE_ELEM_I_26_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a22
#define IRIS_PP_TUPLE_ELEM_I_26_23(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a23
#define IRIS_PP_TUPLE_ELEM_I_26_24(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a24
#define IRIS_PP_TUPLE_ELEM_I_26_25(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a25
#define IRIS_PP_TUPLE_ELEM_I_27_0(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a0
#define IRIS_PP_TUPLE_ELEM_I_27_1(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a1
#define IRIS_PP_TUPLE_ELEM_I_27_2(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a2
#define IRIS_PP_TUPLE_ELEM_I_27_3(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a3
#define IRIS_PP_TUPLE_ELEM_I_27_4(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a4
#define IRIS_PP_TUPLE_ELEM_I_27_5(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a5
#define IRIS_PP_TUPLE_ELEM_I_27_6(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a6
#define IRIS_PP_TUPLE_ELEM_I_27_7(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a7
#define IRIS_PP_TUPLE_ELEM_I_27_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a8
#define IRIS_PP_TUPLE_ELEM_I_27_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  a9
#define IRIS_PP_TUPLE_ELEM_I_27_10(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a10
#define IRIS_PP_TUPLE_ELEM_I_27_11(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a11
#define IRIS_PP_TUPLE_ELEM_I_27_12(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a12
#define IRIS_PP_TUPLE_ELEM_I_27_13(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a13
#define IRIS_PP_TUPLE_ELEM_I_27_14(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a14
#define IRIS_PP_TUPLE_ELEM_I_27_15(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a15
#define IRIS_PP_TUPLE_ELEM_I_27_16(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a16
#define IRIS_PP_TUPLE_ELEM_I_27_17(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a17
#define IRIS_PP_TUPLE_ELEM_I_27_18(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a18
#define IRIS_PP_TUPLE_ELEM_I_27_19(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a19
#define IRIS_PP_TUPLE_ELEM_I_27_20(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a20
#define IRIS_PP_TUPLE_ELEM_I_27_21(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a21
#define IRIS_PP_TUPLE_ELEM_I_27_22(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a22
#define IRIS_PP_TUPLE_ELEM_I_27_23(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a23
#define IRIS_PP_TUPLE_ELEM_I_27_24(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a24
#define IRIS_PP_TUPLE_ELEM_I_27_25(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a25
#define IRIS_PP_TUPLE_ELEM_I_27_26(                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26 \
)                                                                                                                               \
  a26
#define IRIS_PP_TUPLE_ELEM_I_28_0(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a0
#define IRIS_PP_TUPLE_ELEM_I_28_1(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a1
#define IRIS_PP_TUPLE_ELEM_I_28_2(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a2
#define IRIS_PP_TUPLE_ELEM_I_28_3(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a3
#define IRIS_PP_TUPLE_ELEM_I_28_4(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a4
#define IRIS_PP_TUPLE_ELEM_I_28_5(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a5
#define IRIS_PP_TUPLE_ELEM_I_28_6(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a6
#define IRIS_PP_TUPLE_ELEM_I_28_7(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a7
#define IRIS_PP_TUPLE_ELEM_I_28_8(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a8
#define IRIS_PP_TUPLE_ELEM_I_28_9(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a9
#define IRIS_PP_TUPLE_ELEM_I_28_10(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a10
#define IRIS_PP_TUPLE_ELEM_I_28_11(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a11
#define IRIS_PP_TUPLE_ELEM_I_28_12(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a12
#define IRIS_PP_TUPLE_ELEM_I_28_13(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a13
#define IRIS_PP_TUPLE_ELEM_I_28_14(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a14
#define IRIS_PP_TUPLE_ELEM_I_28_15(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a15
#define IRIS_PP_TUPLE_ELEM_I_28_16(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a16
#define IRIS_PP_TUPLE_ELEM_I_28_17(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a17
#define IRIS_PP_TUPLE_ELEM_I_28_18(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a18
#define IRIS_PP_TUPLE_ELEM_I_28_19(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a19
#define IRIS_PP_TUPLE_ELEM_I_28_20(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a20
#define IRIS_PP_TUPLE_ELEM_I_28_21(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a21
#define IRIS_PP_TUPLE_ELEM_I_28_22(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a22
#define IRIS_PP_TUPLE_ELEM_I_28_23(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a23
#define IRIS_PP_TUPLE_ELEM_I_28_24(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a24
#define IRIS_PP_TUPLE_ELEM_I_28_25(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a25
#define IRIS_PP_TUPLE_ELEM_I_28_26(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a26
#define IRIS_PP_TUPLE_ELEM_I_28_27(                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  a27
#define IRIS_PP_TUPLE_ELEM_I_29_0(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a0
#define IRIS_PP_TUPLE_ELEM_I_29_1(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a1
#define IRIS_PP_TUPLE_ELEM_I_29_2(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a2
#define IRIS_PP_TUPLE_ELEM_I_29_3(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a3
#define IRIS_PP_TUPLE_ELEM_I_29_4(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a4
#define IRIS_PP_TUPLE_ELEM_I_29_5(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a5
#define IRIS_PP_TUPLE_ELEM_I_29_6(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a6
#define IRIS_PP_TUPLE_ELEM_I_29_7(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a7
#define IRIS_PP_TUPLE_ELEM_I_29_8(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a8
#define IRIS_PP_TUPLE_ELEM_I_29_9(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a9
#define IRIS_PP_TUPLE_ELEM_I_29_10(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a10
#define IRIS_PP_TUPLE_ELEM_I_29_11(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a11
#define IRIS_PP_TUPLE_ELEM_I_29_12(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a12
#define IRIS_PP_TUPLE_ELEM_I_29_13(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a13
#define IRIS_PP_TUPLE_ELEM_I_29_14(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a14
#define IRIS_PP_TUPLE_ELEM_I_29_15(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a15
#define IRIS_PP_TUPLE_ELEM_I_29_16(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a16
#define IRIS_PP_TUPLE_ELEM_I_29_17(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a17
#define IRIS_PP_TUPLE_ELEM_I_29_18(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a18
#define IRIS_PP_TUPLE_ELEM_I_29_19(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a19
#define IRIS_PP_TUPLE_ELEM_I_29_20(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a20
#define IRIS_PP_TUPLE_ELEM_I_29_21(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a21
#define IRIS_PP_TUPLE_ELEM_I_29_22(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a22
#define IRIS_PP_TUPLE_ELEM_I_29_23(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a23
#define IRIS_PP_TUPLE_ELEM_I_29_24(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a24
#define IRIS_PP_TUPLE_ELEM_I_29_25(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a25
#define IRIS_PP_TUPLE_ELEM_I_29_26(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a26
#define IRIS_PP_TUPLE_ELEM_I_29_27(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a27
#define IRIS_PP_TUPLE_ELEM_I_29_28(                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  a28
#define IRIS_PP_TUPLE_ELEM_I_30_0(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a0
#define IRIS_PP_TUPLE_ELEM_I_30_1(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a1
#define IRIS_PP_TUPLE_ELEM_I_30_2(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a2
#define IRIS_PP_TUPLE_ELEM_I_30_3(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a3
#define IRIS_PP_TUPLE_ELEM_I_30_4(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a4
#define IRIS_PP_TUPLE_ELEM_I_30_5(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a5
#define IRIS_PP_TUPLE_ELEM_I_30_6(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a6
#define IRIS_PP_TUPLE_ELEM_I_30_7(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a7
#define IRIS_PP_TUPLE_ELEM_I_30_8(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a8
#define IRIS_PP_TUPLE_ELEM_I_30_9(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a9
#define IRIS_PP_TUPLE_ELEM_I_30_10(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a10
#define IRIS_PP_TUPLE_ELEM_I_30_11(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a11
#define IRIS_PP_TUPLE_ELEM_I_30_12(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a12
#define IRIS_PP_TUPLE_ELEM_I_30_13(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a13
#define IRIS_PP_TUPLE_ELEM_I_30_14(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a14
#define IRIS_PP_TUPLE_ELEM_I_30_15(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a15
#define IRIS_PP_TUPLE_ELEM_I_30_16(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a16
#define IRIS_PP_TUPLE_ELEM_I_30_17(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a17
#define IRIS_PP_TUPLE_ELEM_I_30_18(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a18
#define IRIS_PP_TUPLE_ELEM_I_30_19(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a19
#define IRIS_PP_TUPLE_ELEM_I_30_20(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a20
#define IRIS_PP_TUPLE_ELEM_I_30_21(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a21
#define IRIS_PP_TUPLE_ELEM_I_30_22(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a22
#define IRIS_PP_TUPLE_ELEM_I_30_23(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a23
#define IRIS_PP_TUPLE_ELEM_I_30_24(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a24
#define IRIS_PP_TUPLE_ELEM_I_30_25(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a25
#define IRIS_PP_TUPLE_ELEM_I_30_26(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a26
#define IRIS_PP_TUPLE_ELEM_I_30_27(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a27
#define IRIS_PP_TUPLE_ELEM_I_30_28(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a28
#define IRIS_PP_TUPLE_ELEM_I_30_29(                                                                                                            \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  a29
#define IRIS_PP_TUPLE_ELEM_I_31_0(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a0
#define IRIS_PP_TUPLE_ELEM_I_31_1(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a1
#define IRIS_PP_TUPLE_ELEM_I_31_2(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a2
#define IRIS_PP_TUPLE_ELEM_I_31_3(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a3
#define IRIS_PP_TUPLE_ELEM_I_31_4(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a4
#define IRIS_PP_TUPLE_ELEM_I_31_5(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a5
#define IRIS_PP_TUPLE_ELEM_I_31_6(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a6
#define IRIS_PP_TUPLE_ELEM_I_31_7(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a7
#define IRIS_PP_TUPLE_ELEM_I_31_8(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a8
#define IRIS_PP_TUPLE_ELEM_I_31_9(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a9
#define IRIS_PP_TUPLE_ELEM_I_31_10(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a10
#define IRIS_PP_TUPLE_ELEM_I_31_11(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a11
#define IRIS_PP_TUPLE_ELEM_I_31_12(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a12
#define IRIS_PP_TUPLE_ELEM_I_31_13(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a13
#define IRIS_PP_TUPLE_ELEM_I_31_14(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a14
#define IRIS_PP_TUPLE_ELEM_I_31_15(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a15
#define IRIS_PP_TUPLE_ELEM_I_31_16(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a16
#define IRIS_PP_TUPLE_ELEM_I_31_17(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a17
#define IRIS_PP_TUPLE_ELEM_I_31_18(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a18
#define IRIS_PP_TUPLE_ELEM_I_31_19(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a19
#define IRIS_PP_TUPLE_ELEM_I_31_20(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a20
#define IRIS_PP_TUPLE_ELEM_I_31_21(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a21
#define IRIS_PP_TUPLE_ELEM_I_31_22(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a22
#define IRIS_PP_TUPLE_ELEM_I_31_23(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a23
#define IRIS_PP_TUPLE_ELEM_I_31_24(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a24
#define IRIS_PP_TUPLE_ELEM_I_31_25(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a25
#define IRIS_PP_TUPLE_ELEM_I_31_26(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a26
#define IRIS_PP_TUPLE_ELEM_I_31_27(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a27
#define IRIS_PP_TUPLE_ELEM_I_31_28(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a28
#define IRIS_PP_TUPLE_ELEM_I_31_29(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a29
#define IRIS_PP_TUPLE_ELEM_I_31_30(                                                                                                                 \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  a30

#endif

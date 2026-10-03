#ifndef IRIS_ZZ_PREPROCESS_TUPLE_HPP
#define IRIS_ZZ_PREPROCESS_TUPLE_HPP

// SPDX-License-Identifier: MIT

#include <iris/pp/cat.hpp>

#define IRIS_PP_TUPLE_SIZE(tuple) IRIS_ZZ_PP_TUPLE_SIZE_I tuple
#define IRIS_ZZ_PP_TUPLE_SIZE_I(...)                                                                                                                         \
  IRIS_ZZ_PP_TUPLE_SIZE_I_I(                                                                                                                                 \
      __VA_ARGS__ __VA_OPT__(, ) 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 \
  )
#define IRIS_ZZ_PP_TUPLE_SIZE_I_I(                                                                                                                            \
    a32, a31, a30, a29, a28, a27, a26, a25, a24, a23, a22, a21, a20, a19, a18, a17, a16, a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, \
    size, ...                                                                                                                                              \
)                                                                                                                                                          \
  size

#define IRIS_PP_TUPLE_ELEM(index, tuple) IRIS_PP_CAT(IRIS_ZZ_PP_TUPLE_ELEM_I_, index) tuple

#define IRIS_PP_TUPLE_TO_SEQ(tuple) IRIS_PP_CAT(IRIS_ZZ_PP_TUPLE_TO_SEQ_I_, IRIS_PP_TUPLE_SIZE(tuple)) tuple

#define IRIS_ZZ_PP_TUPLE_ELEM_I_0(a0, ...) a0
#define IRIS_ZZ_PP_TUPLE_ELEM_I_1(a0, a1, ...) a1
#define IRIS_ZZ_PP_TUPLE_ELEM_I_2(a0, a1, a2, ...) a2
#define IRIS_ZZ_PP_TUPLE_ELEM_I_3(a0, a1, a2, a3, ...) a3
#define IRIS_ZZ_PP_TUPLE_ELEM_I_4(a0, a1, a2, a3, a4, ...) a4
#define IRIS_ZZ_PP_TUPLE_ELEM_I_5(a0, a1, a2, a3, a4, a5, ...) a5
#define IRIS_ZZ_PP_TUPLE_ELEM_I_6(a0, a1, a2, a3, a4, a5, a6, ...) a6
#define IRIS_ZZ_PP_TUPLE_ELEM_I_7(a0, a1, a2, a3, a4, a5, a6, a7, ...) a7
#define IRIS_ZZ_PP_TUPLE_ELEM_I_8(a0, a1, a2, a3, a4, a5, a6, a7, a8, ...) a8
#define IRIS_ZZ_PP_TUPLE_ELEM_I_9(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, ...) a9
#define IRIS_ZZ_PP_TUPLE_ELEM_I_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10
#define IRIS_ZZ_PP_TUPLE_ELEM_I_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define IRIS_ZZ_PP_TUPLE_ELEM_I_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...) a12
#define IRIS_ZZ_PP_TUPLE_ELEM_I_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, ...) a13
#define IRIS_ZZ_PP_TUPLE_ELEM_I_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, ...) a14
#define IRIS_ZZ_PP_TUPLE_ELEM_I_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, ...) a15
#define IRIS_ZZ_PP_TUPLE_ELEM_I_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, ...) a16
#define IRIS_ZZ_PP_TUPLE_ELEM_I_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, ...) a17
#define IRIS_ZZ_PP_TUPLE_ELEM_I_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, ...) a18
#define IRIS_ZZ_PP_TUPLE_ELEM_I_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, ...) a19
#define IRIS_ZZ_PP_TUPLE_ELEM_I_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, ...) a20
#define IRIS_ZZ_PP_TUPLE_ELEM_I_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, ...) a21
#define IRIS_ZZ_PP_TUPLE_ELEM_I_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, ...) a22
#define IRIS_ZZ_PP_TUPLE_ELEM_I_23(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, ...) a23
#define IRIS_ZZ_PP_TUPLE_ELEM_I_24(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, ...) a24
#define IRIS_ZZ_PP_TUPLE_ELEM_I_25(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, ...) a25
#define IRIS_ZZ_PP_TUPLE_ELEM_I_26(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, ...) a26
#define IRIS_ZZ_PP_TUPLE_ELEM_I_27(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, ...) a27
#define IRIS_ZZ_PP_TUPLE_ELEM_I_28(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, ...) a28
#define IRIS_ZZ_PP_TUPLE_ELEM_I_29(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, ...) a29
#define IRIS_ZZ_PP_TUPLE_ELEM_I_30(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, ...) a30
#define IRIS_ZZ_PP_TUPLE_ELEM_I_31(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, ...) a31

#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_1(a0) (a0)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_2(a0, a1) (a0)(a1)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_3(a0, a1, a2) (a0)(a1)(a2)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_4(a0, a1, a2, a3) (a0)(a1)(a2)(a3)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_5(a0, a1, a2, a3, a4) (a0)(a1)(a2)(a3)(a4)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_6(a0, a1, a2, a3, a4, a5) (a0)(a1)(a2)(a3)(a4)(a5)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_7(a0, a1, a2, a3, a4, a5, a6) (a0)(a1)(a2)(a3)(a4)(a5)(a6)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_8(a0, a1, a2, a3, a4, a5, a6, a7) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_9(a0, a1, a2, a3, a4, a5, a6, a7, a8) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_17(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_18(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_19(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_20(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_21(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_22(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_23(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_24(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_25(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_26(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_27(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_28(                                                                                                   \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27 \
)                                                                                                                                    \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)(a27)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_29(                                                                                                        \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28 \
)                                                                                                                                         \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)(a27)(a28)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_30(                                                                                                             \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29 \
)                                                                                                                                              \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)(a27)(a28)(a29)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_31(                                                                                                                  \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30 \
)                                                                                                                                                   \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)(a27)(a28)(a29)(a30)
#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_32(                                                                                                                       \
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31 \
)                                                                                                                                                        \
  (a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)(a8)(a9)(a10)(a11)(a12)(a13)(a14)(a15)(a16)(a17)(a18)(a19)(a20)(a21)(a22)(a23)(a24)(a25)(a26)(a27)(a28)(a29)(a30)(a31)

#endif

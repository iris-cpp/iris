#ifndef IRIS_PREPROCESS_REPEAT_HPP
#define IRIS_PREPROCESS_REPEAT_HPP

#include <iris/preprocess/cat.hpp>

#define IRIS_PP_REPEAT(count, macro, data) IRIS_PP_CAT(IRIS_PP_REPEAT_I_, count)(macro, data)

#define IRIS_PP_REPEAT_I_32(macro, data) IRIS_PP_REPEAT_I_31(macro, data) macro(31, data)
#define IRIS_PP_REPEAT_I_31(macro, data) IRIS_PP_REPEAT_I_30(macro, data) macro(30, data)
#define IRIS_PP_REPEAT_I_30(macro, data) IRIS_PP_REPEAT_I_29(macro, data) macro(29, data)
#define IRIS_PP_REPEAT_I_29(macro, data) IRIS_PP_REPEAT_I_28(macro, data) macro(28, data)
#define IRIS_PP_REPEAT_I_28(macro, data) IRIS_PP_REPEAT_I_27(macro, data) macro(27, data)
#define IRIS_PP_REPEAT_I_27(macro, data) IRIS_PP_REPEAT_I_26(macro, data) macro(26, data)
#define IRIS_PP_REPEAT_I_26(macro, data) IRIS_PP_REPEAT_I_25(macro, data) macro(25, data)
#define IRIS_PP_REPEAT_I_25(macro, data) IRIS_PP_REPEAT_I_24(macro, data) macro(24, data)
#define IRIS_PP_REPEAT_I_24(macro, data) IRIS_PP_REPEAT_I_23(macro, data) macro(23, data)
#define IRIS_PP_REPEAT_I_23(macro, data) IRIS_PP_REPEAT_I_22(macro, data) macro(22, data)
#define IRIS_PP_REPEAT_I_22(macro, data) IRIS_PP_REPEAT_I_21(macro, data) macro(21, data)
#define IRIS_PP_REPEAT_I_21(macro, data) IRIS_PP_REPEAT_I_20(macro, data) macro(20, data)
#define IRIS_PP_REPEAT_I_20(macro, data) IRIS_PP_REPEAT_I_19(macro, data) macro(19, data)
#define IRIS_PP_REPEAT_I_19(macro, data) IRIS_PP_REPEAT_I_18(macro, data) macro(18, data)
#define IRIS_PP_REPEAT_I_18(macro, data) IRIS_PP_REPEAT_I_17(macro, data) macro(17, data)
#define IRIS_PP_REPEAT_I_17(macro, data) IRIS_PP_REPEAT_I_16(macro, data) macro(16, data)
#define IRIS_PP_REPEAT_I_16(macro, data) IRIS_PP_REPEAT_I_15(macro, data) macro(15, data)
#define IRIS_PP_REPEAT_I_15(macro, data) IRIS_PP_REPEAT_I_14(macro, data) macro(14, data)
#define IRIS_PP_REPEAT_I_14(macro, data) IRIS_PP_REPEAT_I_13(macro, data) macro(13, data)
#define IRIS_PP_REPEAT_I_13(macro, data) IRIS_PP_REPEAT_I_12(macro, data) macro(12, data)
#define IRIS_PP_REPEAT_I_12(macro, data) IRIS_PP_REPEAT_I_11(macro, data) macro(11, data)
#define IRIS_PP_REPEAT_I_11(macro, data) IRIS_PP_REPEAT_I_10(macro, data) macro(10, data)
#define IRIS_PP_REPEAT_I_10(macro, data) IRIS_PP_REPEAT_I_9(macro, data) macro(9, data)
#define IRIS_PP_REPEAT_I_9(macro, data) IRIS_PP_REPEAT_I_8(macro, data) macro(8, data)
#define IRIS_PP_REPEAT_I_8(macro, data) IRIS_PP_REPEAT_I_7(macro, data) macro(7, data)
#define IRIS_PP_REPEAT_I_7(macro, data) IRIS_PP_REPEAT_I_6(macro, data) macro(6, data)
#define IRIS_PP_REPEAT_I_6(macro, data) IRIS_PP_REPEAT_I_5(macro, data) macro(5, data)
#define IRIS_PP_REPEAT_I_5(macro, data) IRIS_PP_REPEAT_I_4(macro, data) macro(4, data)
#define IRIS_PP_REPEAT_I_4(macro, data) IRIS_PP_REPEAT_I_3(macro, data) macro(3, data)
#define IRIS_PP_REPEAT_I_3(macro, data) IRIS_PP_REPEAT_I_2(macro, data) macro(2, data)
#define IRIS_PP_REPEAT_I_2(macro, data) IRIS_PP_REPEAT_I_1(macro, data) macro(1, data)
#define IRIS_PP_REPEAT_I_1(macro, data) macro(0, data)
#define IRIS_PP_REPEAT_I_0(macro, data)

#endif

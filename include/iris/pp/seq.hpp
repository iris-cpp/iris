#ifndef IRIS_ZZ_PREPROCESS_SEQ_HPP
#define IRIS_ZZ_PREPROCESS_SEQ_HPP

// SPDX-License-Identifier: MIT

#include <iris/pp/cat.hpp>
#include <iris/pp/increment.hpp>

#define IRIS_PP_SEQ_ELEM(index, seq) IRIS_ZZ_PP_SEQ_ELEM_I(IRIS_PP_CAT(IRIS_ZZ_PP_SEQ_ELEM_I_, index) seq)
#define IRIS_ZZ_PP_SEQ_ELEM_I(intermediate) IRIS_ZZ_PP_SEQ_ELEM_I_I(intermediate)
#define IRIS_ZZ_PP_SEQ_ELEM_I_I(x, _) x

#define IRIS_PP_SEQ_HEAD(seq) IRIS_ZZ_PP_SEQ_ELEM_I(IRIS_ZZ_PP_SEQ_ELEM_I_0 seq)

#define IRIS_PP_SEQ_TAIL(seq) IRIS_ZZ_PP_SEQ_TAIL_I seq
#define IRIS_ZZ_PP_SEQ_TAIL_I(head)

#define IRIS_PP_SEQ_SIZE(seq) IRIS_PP_CAT(IRIS_ZZ_PP_SEQ_SIZE_I_, IRIS_ZZ_PP_SEQ_SIZE_I_0 seq)

#define IRIS_PP_SEQ_FOR_EACH(seq, macro, data) IRIS_PP_CAT(IRIS_ZZ_PP_SEQ_FOR_EACH_I_, IRIS_PP_SEQ_SIZE(seq))(macro, data, seq)

#define IRIS_PP_SEQ_FOR_EACH_WITH_INDEX(seq, macro, data) \
    IRIS_PP_CAT(IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_, IRIS_PP_SEQ_SIZE(seq))(0, macro, data, seq)

#define IRIS_ZZ_PP_SEQ_ELEM_I_0(seq) seq, dummy
#define IRIS_ZZ_PP_SEQ_ELEM_I_1(seq) IRIS_ZZ_PP_SEQ_ELEM_I_0
#define IRIS_ZZ_PP_SEQ_ELEM_I_2(seq) IRIS_ZZ_PP_SEQ_ELEM_I_1
#define IRIS_ZZ_PP_SEQ_ELEM_I_3(seq) IRIS_ZZ_PP_SEQ_ELEM_I_2
#define IRIS_ZZ_PP_SEQ_ELEM_I_4(seq) IRIS_ZZ_PP_SEQ_ELEM_I_3
#define IRIS_ZZ_PP_SEQ_ELEM_I_5(seq) IRIS_ZZ_PP_SEQ_ELEM_I_4
#define IRIS_ZZ_PP_SEQ_ELEM_I_6(seq) IRIS_ZZ_PP_SEQ_ELEM_I_5
#define IRIS_ZZ_PP_SEQ_ELEM_I_7(seq) IRIS_ZZ_PP_SEQ_ELEM_I_6
#define IRIS_ZZ_PP_SEQ_ELEM_I_8(seq) IRIS_ZZ_PP_SEQ_ELEM_I_7
#define IRIS_ZZ_PP_SEQ_ELEM_I_9(seq) IRIS_ZZ_PP_SEQ_ELEM_I_8
#define IRIS_ZZ_PP_SEQ_ELEM_I_10(seq) IRIS_ZZ_PP_SEQ_ELEM_I_9
#define IRIS_ZZ_PP_SEQ_ELEM_I_11(seq) IRIS_ZZ_PP_SEQ_ELEM_I_10
#define IRIS_ZZ_PP_SEQ_ELEM_I_12(seq) IRIS_ZZ_PP_SEQ_ELEM_I_11
#define IRIS_ZZ_PP_SEQ_ELEM_I_13(seq) IRIS_ZZ_PP_SEQ_ELEM_I_12
#define IRIS_ZZ_PP_SEQ_ELEM_I_14(seq) IRIS_ZZ_PP_SEQ_ELEM_I_13
#define IRIS_ZZ_PP_SEQ_ELEM_I_15(seq) IRIS_ZZ_PP_SEQ_ELEM_I_14
#define IRIS_ZZ_PP_SEQ_ELEM_I_16(seq) IRIS_ZZ_PP_SEQ_ELEM_I_15
#define IRIS_ZZ_PP_SEQ_ELEM_I_17(seq) IRIS_ZZ_PP_SEQ_ELEM_I_16
#define IRIS_ZZ_PP_SEQ_ELEM_I_18(seq) IRIS_ZZ_PP_SEQ_ELEM_I_17
#define IRIS_ZZ_PP_SEQ_ELEM_I_19(seq) IRIS_ZZ_PP_SEQ_ELEM_I_18
#define IRIS_ZZ_PP_SEQ_ELEM_I_20(seq) IRIS_ZZ_PP_SEQ_ELEM_I_19
#define IRIS_ZZ_PP_SEQ_ELEM_I_21(seq) IRIS_ZZ_PP_SEQ_ELEM_I_20
#define IRIS_ZZ_PP_SEQ_ELEM_I_22(seq) IRIS_ZZ_PP_SEQ_ELEM_I_21
#define IRIS_ZZ_PP_SEQ_ELEM_I_23(seq) IRIS_ZZ_PP_SEQ_ELEM_I_22
#define IRIS_ZZ_PP_SEQ_ELEM_I_24(seq) IRIS_ZZ_PP_SEQ_ELEM_I_23
#define IRIS_ZZ_PP_SEQ_ELEM_I_25(seq) IRIS_ZZ_PP_SEQ_ELEM_I_24
#define IRIS_ZZ_PP_SEQ_ELEM_I_26(seq) IRIS_ZZ_PP_SEQ_ELEM_I_25
#define IRIS_ZZ_PP_SEQ_ELEM_I_27(seq) IRIS_ZZ_PP_SEQ_ELEM_I_26
#define IRIS_ZZ_PP_SEQ_ELEM_I_28(seq) IRIS_ZZ_PP_SEQ_ELEM_I_27
#define IRIS_ZZ_PP_SEQ_ELEM_I_29(seq) IRIS_ZZ_PP_SEQ_ELEM_I_28
#define IRIS_ZZ_PP_SEQ_ELEM_I_30(seq) IRIS_ZZ_PP_SEQ_ELEM_I_29
#define IRIS_ZZ_PP_SEQ_ELEM_I_31(seq) IRIS_ZZ_PP_SEQ_ELEM_I_30
#define IRIS_ZZ_PP_SEQ_ELEM_I_32(seq) IRIS_ZZ_PP_SEQ_ELEM_I_31

#define IRIS_ZZ_PP_SEQ_SIZE_I_0(_) IRIS_ZZ_PP_SEQ_SIZE_I_1
#define IRIS_ZZ_PP_SEQ_SIZE_I_1(_) IRIS_ZZ_PP_SEQ_SIZE_I_2
#define IRIS_ZZ_PP_SEQ_SIZE_I_2(_) IRIS_ZZ_PP_SEQ_SIZE_I_3
#define IRIS_ZZ_PP_SEQ_SIZE_I_3(_) IRIS_ZZ_PP_SEQ_SIZE_I_4
#define IRIS_ZZ_PP_SEQ_SIZE_I_4(_) IRIS_ZZ_PP_SEQ_SIZE_I_5
#define IRIS_ZZ_PP_SEQ_SIZE_I_5(_) IRIS_ZZ_PP_SEQ_SIZE_I_6
#define IRIS_ZZ_PP_SEQ_SIZE_I_6(_) IRIS_ZZ_PP_SEQ_SIZE_I_7
#define IRIS_ZZ_PP_SEQ_SIZE_I_7(_) IRIS_ZZ_PP_SEQ_SIZE_I_8
#define IRIS_ZZ_PP_SEQ_SIZE_I_8(_) IRIS_ZZ_PP_SEQ_SIZE_I_9
#define IRIS_ZZ_PP_SEQ_SIZE_I_9(_) IRIS_ZZ_PP_SEQ_SIZE_I_10
#define IRIS_ZZ_PP_SEQ_SIZE_I_10(_) IRIS_ZZ_PP_SEQ_SIZE_I_11
#define IRIS_ZZ_PP_SEQ_SIZE_I_11(_) IRIS_ZZ_PP_SEQ_SIZE_I_12
#define IRIS_ZZ_PP_SEQ_SIZE_I_12(_) IRIS_ZZ_PP_SEQ_SIZE_I_13
#define IRIS_ZZ_PP_SEQ_SIZE_I_13(_) IRIS_ZZ_PP_SEQ_SIZE_I_14
#define IRIS_ZZ_PP_SEQ_SIZE_I_14(_) IRIS_ZZ_PP_SEQ_SIZE_I_15
#define IRIS_ZZ_PP_SEQ_SIZE_I_15(_) IRIS_ZZ_PP_SEQ_SIZE_I_16
#define IRIS_ZZ_PP_SEQ_SIZE_I_16(_) IRIS_ZZ_PP_SEQ_SIZE_I_17
#define IRIS_ZZ_PP_SEQ_SIZE_I_17(_) IRIS_ZZ_PP_SEQ_SIZE_I_18
#define IRIS_ZZ_PP_SEQ_SIZE_I_18(_) IRIS_ZZ_PP_SEQ_SIZE_I_19
#define IRIS_ZZ_PP_SEQ_SIZE_I_19(_) IRIS_ZZ_PP_SEQ_SIZE_I_20
#define IRIS_ZZ_PP_SEQ_SIZE_I_20(_) IRIS_ZZ_PP_SEQ_SIZE_I_21
#define IRIS_ZZ_PP_SEQ_SIZE_I_21(_) IRIS_ZZ_PP_SEQ_SIZE_I_22
#define IRIS_ZZ_PP_SEQ_SIZE_I_22(_) IRIS_ZZ_PP_SEQ_SIZE_I_23
#define IRIS_ZZ_PP_SEQ_SIZE_I_23(_) IRIS_ZZ_PP_SEQ_SIZE_I_24
#define IRIS_ZZ_PP_SEQ_SIZE_I_24(_) IRIS_ZZ_PP_SEQ_SIZE_I_25
#define IRIS_ZZ_PP_SEQ_SIZE_I_25(_) IRIS_ZZ_PP_SEQ_SIZE_I_26
#define IRIS_ZZ_PP_SEQ_SIZE_I_26(_) IRIS_ZZ_PP_SEQ_SIZE_I_27
#define IRIS_ZZ_PP_SEQ_SIZE_I_27(_) IRIS_ZZ_PP_SEQ_SIZE_I_28
#define IRIS_ZZ_PP_SEQ_SIZE_I_28(_) IRIS_ZZ_PP_SEQ_SIZE_I_29
#define IRIS_ZZ_PP_SEQ_SIZE_I_29(_) IRIS_ZZ_PP_SEQ_SIZE_I_30
#define IRIS_ZZ_PP_SEQ_SIZE_I_30(_) IRIS_ZZ_PP_SEQ_SIZE_I_31
#define IRIS_ZZ_PP_SEQ_SIZE_I_31(_) IRIS_ZZ_PP_SEQ_SIZE_I_32
#define IRIS_ZZ_PP_SEQ_SIZE_I_32(_) IRIS_ZZ_PP_SEQ_SIZE_I_33

#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_0 0
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_1 1
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_2 2
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_3 3
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_4 4
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_5 5
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_6 6
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_7 7
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_8 8
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_9 9
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_10 10
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_11 11
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_12 12
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_13 13
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_14 14
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_15 15
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_16 16
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_17 17
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_18 18
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_19 19
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_20 20
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_21 21
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_22 22
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_23 23
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_24 24
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_25 25
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_26 26
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_27 27
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_28 28
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_29 29
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_30 30
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_31 31
#define IRIS_ZZ_PP_SEQ_SIZE_I_IRIS_ZZ_PP_SEQ_SIZE_I_32 32

#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_0(macro, data, seq)
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_1(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data)
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_2(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_1(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_3(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_2(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_4(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_3(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_5(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_4(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_6(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_5(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_7(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_6(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_8(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_7(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_9(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_8(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_10(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_9(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_11(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_10(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_12(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_11(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_13(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_12(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_14(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_13(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_15(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_14(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_16(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_15(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_17(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_16(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_18(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_17(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_19(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_18(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_20(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_19(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_21(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_20(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_22(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_21(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_23(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_22(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_24(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_23(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_25(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_24(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_26(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_25(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_27(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_26(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_28(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_27(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_29(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_28(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_30(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_29(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_31(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_30(macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_I_32(macro, data, seq) macro(IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_I_31(macro, data, IRIS_PP_SEQ_TAIL(seq))

#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_0(index, macro, data, seq)
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_1(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data)
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_2(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_1(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_3(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_2(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_4(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_3(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_5(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_4(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_6(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_5(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_7(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_6(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_8(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_7(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_9(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_8(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_10(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_9(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_11(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_10(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_12(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_11(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_13(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_12(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_14(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_13(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_15(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_14(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_16(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_15(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_17(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_16(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_18(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_17(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_19(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_18(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_20(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_19(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_21(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_20(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_22(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_21(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_23(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_22(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_24(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_23(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_25(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_24(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_26(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_25(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_27(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_26(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_28(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_27(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_29(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_28(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_30(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_29(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_31(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_30(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))
#define IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_32(index, macro, data, seq) macro(index, IRIS_PP_SEQ_HEAD(seq), data) IRIS_ZZ_PP_SEQ_FOR_EACH_WITH_INDEX_I_31(IRIS_PP_INCREMENT(index), macro, data, IRIS_PP_SEQ_TAIL(seq))

#endif

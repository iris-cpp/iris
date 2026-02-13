#ifndef IRIS_PP_REC_HPP
#define IRIS_PP_REC_HPP

#include <iris/pp/if.hpp>

#define IRIS_PP_REC(pred, n) IRIS_PP_CAT(IRIS_PP_REC_I_NODE_ENTRY_, n)(pred)

#define IRIS_PP_REC_I_NODE_ENTRY_8(pred) IRIS_PP_REC_I_NODE_4(pred)(pred)(pred)
#define IRIS_PP_REC_I_NODE_ENTRY_4(pred) IRIS_PP_REC_I_NODE_2(pred)(pred)
#define IRIS_PP_REC_I_NODE_ENTRY_2(pred) IRIS_PP_REC_I_NODE_1(pred)

#define IRIS_PP_REC_I_NODE_4(pred) IRIS_PP_IF(pred(4), IRIS_PP_REC_I_NODE_2, IRIS_PP_REC_I_NODE_6)

#define IRIS_PP_REC_I_NODE_2(pred) IRIS_PP_IF(pred(2), IRIS_PP_REC_I_NODE_1, IRIS_PP_REC_I_NODE_3)

#define IRIS_PP_REC_I_NODE_1(pred) IRIS_PP_IF(pred(1), 1, 2)
#define IRIS_PP_REC_I_NODE_3(pred) IRIS_PP_IF(pred(3), 3, 4)

#define IRIS_PP_REC_I_NODE_6(pred) IRIS_PP_IF(pred(6), IRIS_PP_REC_I_NODE_5, IRIS_PP_REC_I_NODE_7)

#define IRIS_PP_REC_I_NODE_5(pred) IRIS_PP_IF(pred(5), 5, 6)
#define IRIS_PP_REC_I_NODE_7(pred) IRIS_PP_IF(pred(5), 7, 8)

#endif

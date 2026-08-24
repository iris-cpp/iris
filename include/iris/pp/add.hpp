#ifndef IRIS_ZZ_PREPROCESS_ADD_HPP
#define IRIS_ZZ_PREPROCESS_ADD_HPP

// SPDX-License-Identifier: MIT

#include <iris/pp/cat.hpp>
#include <iris/pp/decrement.hpp>
#include <iris/pp/increment.hpp>
#include <iris/pp/while.hpp>

#define IRIS_PP_ADD(a, b) IRIS_ZZ_PP_ADD_I_FIRST(IRIS_PP_WHILE((a, b), IRIS_ZZ_PP_ADD_I_PRED, IRIS_ZZ_PP_ADD_I_UPDATE))

#define IRIS_ZZ_PP_ADD_I_FIRST(state) IRIS_ZZ_PP_ADD_I_FIRST_I state
#define IRIS_ZZ_PP_ADD_I_FIRST_I(a, b) a

#define IRIS_ZZ_PP_ADD_I_PRED(state) IRIS_ZZ_PP_ADD_I_PRED_I state
#define IRIS_ZZ_PP_ADD_I_PRED_I(a, b) b

#define IRIS_ZZ_PP_ADD_I_UPDATE(state) IRIS_ZZ_PP_ADD_I_UPDATE_I state
#define IRIS_ZZ_PP_ADD_I_UPDATE_I(a, b) (IRIS_PP_INCREMENT(a), IRIS_PP_DECREMENT(b))

#endif

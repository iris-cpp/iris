#ifndef IRIS_PP_IF_HPP
#define IRIS_PP_IF_HPP

#include <iris/preprocess/bool.hpp>
#include <iris/preprocess/cat.hpp>

#define IRIS_PP_IF(expr, true_expr, false_expr) IRIS_PP_IIF(IRIS_PP_BOOL(expr), true_expr, false_expr)
#define IRIS_PP_IIF(zero_or_one, one_expr, zero_expr) IRIS_PP_CAT(IRIS_PP_IIF_I_, zero_or_one)(one_expr, zero_expr)

#define IRIS_PP_IIF_I_0(one_expr, zero_expr) zero_expr
#define IRIS_PP_IIF_I_1(one_expr, zero_expr) one_expr

#endif

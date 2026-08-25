#ifndef IRIS_ZZ_PREPROCESS_CAT_HPP
#define IRIS_ZZ_PREPROCESS_CAT_HPP

// SPDX-License-Identifier: MIT

#define IRIS_ZZ_PP_CAT_I(a, b) a##b
#define IRIS_PP_CAT(a, b) IRIS_ZZ_PP_CAT_I(a, b)

#define IRIS_ZZ_PP_CAT_ONLY_TWO_I(a, b, ...) a##b
#define IRIS_PP_CAT_ONLY_TWO(a, b, ...) IRIS_ZZ_PP_CAT_ONLY_TWO_I(a, b)

#endif

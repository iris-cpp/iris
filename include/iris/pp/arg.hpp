#ifndef IRIS_ZZ_PP_ARG_HPP
#define IRIS_ZZ_PP_ARG_HPP

// SPDX-License-Identifier: MIT

#include <iris/pp/if.hpp>

#define IRIS_PP_IDENTITY(...) __VA_ARGS__

#define IRIS_PP_EMPTY(...)

#define IRIS_ZZ_PP_EXPAND_I(...) __VA_ARGS__
#define IRIS_PP_EXPAND(...) IRIS_ZZ_PP_EXPAND_I(__VA_ARGS__)

#define IRIS_ZZ_PP_PROBE() ~, 1

#define IRIS_ZZ_PP_SECOND(a, b, ...) b
#define IRIS_ZZ_PP_IS_PROBE(...) IRIS_ZZ_PP_SECOND(__VA_ARGS__, 0)
#define IRIS_ZZ_PP_IS_PAREN_PROBE(...) IRIS_ZZ_PP_PROBE()

#define IRIS_PP_IS_PAREN(x) IRIS_ZZ_PP_IS_PROBE(IRIS_ZZ_PP_IS_PAREN_PROBE x)

#define IRIS_PP_UNPAREN(x) IRIS_PP_EXPAND x
#define IRIS_PP_UNPAREN_IF_PAREN(x) \
    IRIS_PP_IF(IRIS_PP_IS_PAREN(x), IRIS_PP_UNPAREN, IRIS_PP_IDENTITY) (x)

#endif

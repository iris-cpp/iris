#ifndef IRIS_ZZ_PP_OVERLOAD_HPP
#define IRIS_ZZ_PP_OVERLOAD_HPP

// SPDX-License-Identifier: MIT

#define IRIS_PP_OVERLOAD(prefix, ...) IRIS_ZZ_PP_OVERLOAD_I(prefix, __VA_ARGS__ __VA_OPT__(, ) 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define IRIS_ZZ_PP_OVERLOAD_I(prefix, a32, a31, a30, a29, a28, a27, a26, a25, a24, a23, a22, a21, a20, a19, a18, a17, a16, a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, size, ...) prefix##size

#endif

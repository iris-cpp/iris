#ifndef IRIS_CONFIG_HPP
#define IRIS_CONFIG_HPP

// SPDX-License-Identifier: MIT

#include <version>

#define IRIS_STRINGIZE_I(x) #x
#define IRIS_STRINGIZE(x) IRIS_STRINGIZE_I(x)

#define IRIS_CONCAT_I_I(a, b) a##b
#define IRIS_CONCAT_I(a, b) IRIS_CONCAT_I_I(a, b)
#define IRIS_CONCAT(a, b) IRIS_CONCAT_I(a, b)

#if _MSC_VER
# include <CppCoreCheck/warnings.h>
# pragma warning(default: CPPCORECHECK_LIFETIME_WARNINGS)
#endif

// <https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#c++20-[[no_unique_address]]>

#if _MSC_VER && _MSC_VER < 1929 // VS 2019 v16.9 or before
# error "Too old MSVC version; we don't support this because it leads to ODR violation regarding the existence of [[(msvc::)no_unique_address]]"
#endif

#if _MSC_VER && __INTELLISENSE__ // Memory Layout view shows wrong layout without this workaround
# define IRIS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address, no_unique_address]]

#elif _MSC_VER // normal MSVC
# define IRIS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]

#else // other compilers
# define IRIS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ifndef IRIS_LIFETIMEBOUND
# ifdef __clang__
#  define IRIS_LIFETIMEBOUND [[clang::lifetimebound]]
# elifdef _MSC_VER
#  define IRIS_LIFETIMEBOUND [[msvc::lifetimebound]]
# else
#  define IRIS_LIFETIMEBOUND
# endif
#endif

#endif

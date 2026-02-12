#ifndef IRIS_CONFIG_HPP
#define IRIS_CONFIG_HPP

// SPDX-License-Identifier: MIT

#include <version>

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

#if __cpp_consteval >= 202211L
# define IRIS_CONSTEXPR_UP constexpr
#else
# define IRIS_CONSTEXPR_UP consteval
#endif


// TODO:
// [[msvc::forceinline]] https://developercommunity.visualstudio.com/t/support-forceinline-on-c-lambda-expressions/351580#T-N1092115
// [[clang::always_inline]] https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline
// [[gnu::always_inline]] https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-always_005finline-function-attribute

#ifndef IRIS_FORCEINLINE
# ifdef _MSC_VER
#  define IRIS_FORCEINLINE __forceinline
# elifdef __GNUC__
#  define IRIS_FORCEINLINE __attribute__((always_inline)) inline
# endif
#endif

#endif

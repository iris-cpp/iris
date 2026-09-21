#ifndef IRIS_ZZ_CONFIG_HPP
#define IRIS_ZZ_CONFIG_HPP

// SPDX-License-Identifier: MIT

// IWYU pragma: always_keep

#include <version>

#if defined(_MSC_VER) && \
    !defined(__clang__) && \
    !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
# define IRIS_COMPILER_STRICTLY_MSVC 1
#else
# define IRIS_COMPILER_STRICTLY_MSVC 0
#endif

#if defined(_MSC_VER) && defined(__clang__)
# define IRIS_COMPILER_CLANG_CL 1
#else
# define IRIS_COMPILER_CLANG_CL 0
#endif

#if defined(_MSC_VER) && defined(__INTELLISENSE__)
# define IRIS_MSVC_INTELLISENSE 1
#else
# define IRIS_MSVC_INTELLISENSE 0
#endif


#if _MSC_VER
# include <CppCoreCheck/warnings.h>
# pragma warning(default: CPPCORECHECK_LIFETIME_WARNINGS)
#endif

// <https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#c++20-[[no_unique_address]]>

#if _MSC_VER && _MSC_VER < 1929 // VS 2019 v16.9 or before
# error "Too old MSVC version; we don't support this because it leads to ODR violation regarding the existence of [[(msvc::)no_unique_address]]"
#endif

#if IRIS_MSVC_INTELLISENSE // Memory Layout view shows wrong layout without this workaround
# define IRIS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address, no_unique_address]]

#elif IRIS_COMPILER_STRICTLY_MSVC
# define IRIS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]

#elif defined(_MSC_VER)
# if __has_cpp_attribute(msvc::no_unique_address)
#  define IRIS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
# else
#  define IRIS_NO_UNIQUE_ADDRESS [[no_unique_address]]
# endif

#else
# define IRIS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif


#if IRIS_COMPILER_STRICTLY_MSVC
# define IRIS_EBO __declspec(empty_bases)

#elif IRIS_COMPILER_CLANG_CL
# if __has_declspec_attribute(empty_bases)
#  define IRIS_EBO __declspec(empty_bases)
# else
#  define IRIS_EBO
# endif

#else
# define IRIS_EBO
#endif


#if IRIS_MSVC_INTELLISENSE
# define IRIS_LIFETIMEBOUND [[msvc::lifetimebound]]

#elif __has_cpp_attribute(clang::lifetimebound)
# define IRIS_LIFETIMEBOUND [[clang::lifetimebound]]

#elif __has_cpp_attribute(msvc::lifetimebound)
# define IRIS_LIFETIMEBOUND [[msvc::lifetimebound]]

#else
# define IRIS_LIFETIMEBOUND
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

// ------------------------------------------------------

#if defined(__RESHARPER__) && !defined(__cpp_lib_reference_from_temporary)
#define __cpp_lib_reference_from_temporary 202202L
#endif

#endif

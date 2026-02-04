#ifndef IRIS_RVARIANT_BENCHMARK_SUPPORT_HPP
#define IRIS_RVARIANT_BENCHMARK_SUPPORT_HPP

// SPDX-License-Identifier: MIT

#include <memory>
#include <cstdint>

namespace benchmark {

#ifdef _WIN32
# define IRIS_BENCHMARK_API __declspec(dllexport)
#else
# define IRIS_BENCHMARK_API __attribute__((visibility("default")))
#endif

namespace detail {

IRIS_BENCHMARK_API void disable_optimization_impl(void const*) noexcept;
IRIS_BENCHMARK_API void asm_trace(std::uint_least32_t line) noexcept;

} // detail

#define IRIS_ASM_TRACE ::benchmark::detail::asm_trace(__LINE__);

template<class T>
void disable_optimization(T const& v) noexcept
{
    detail::disable_optimization_impl(std::addressof(v));
}

}

#endif

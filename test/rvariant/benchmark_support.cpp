// SPDX-License-Identifier: MIT

#include "benchmark_support.hpp"

namespace benchmark {

namespace detail {

YK_BENCHMARK_API void disable_optimization_impl(void const*) noexcept
{
    // do nothing
}

YK_BENCHMARK_API void asm_trace(std::uint_least32_t) noexcept
{
    // do nothing
}

} // detail

} // benchmark

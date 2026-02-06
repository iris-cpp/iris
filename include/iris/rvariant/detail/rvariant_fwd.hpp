#ifndef IRIS_RVARIANT_DETAIL_RVARIANT_FWD_HPP
#define IRIS_RVARIANT_DETAIL_RVARIANT_FWD_HPP

// SPDX-License-Identifier: MIT

#include <cstddef>

#if !defined(IRIS_RVARIANT_VISIT_STRENGTHEN)
# define IRIS_RVARIANT_VISIT_STRENGTHEN 1
#endif

#if IRIS_RVARIANT_VISIT_STRENGTHEN
# define IRIS_RVARIANT_VISIT_NOEXCEPT(...) noexcept(__VA_ARGS__)
#else
# define IRIS_RVARIANT_VISIT_NOEXCEPT(...)
#endif

namespace iris {

template<class... Ts>
class rvariant;

template<class T, class Allocator>
class recursive_wrapper;


namespace detail {

struct valueless_t
{
    constexpr explicit valueless_t() = default;
};

inline constexpr valueless_t valueless{};

} // detail

} // iris

#endif

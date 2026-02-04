#ifndef YK_RVARIANT_RECURSIVE_WRAPPER_PMR_HPP
#define YK_RVARIANT_RECURSIVE_WRAPPER_PMR_HPP

// SPDX-License-Identifier: MIT

#include <iris/rvariant/recursive_wrapper.hpp>

#include <memory_resource>

namespace iris::pmr {

template<class T>
using recursive_wrapper = iris::recursive_wrapper<T, std::pmr::polymorphic_allocator<T>>;

} // iris::pmr

#endif

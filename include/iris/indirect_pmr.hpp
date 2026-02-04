#ifndef IRIS_INDIRECT_PMR_HPP
#define IRIS_INDIRECT_PMR_HPP

// SPDX-License-Identifier: MIT

#include <iris/indirect.hpp>

#include <memory_resource>


namespace iris::pmr {

template<class T>
using indirect = iris::indirect<T, std::pmr::polymorphic_allocator<T>>;

} // iris::pmr

#endif

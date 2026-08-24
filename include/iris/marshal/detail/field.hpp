#ifndef IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP
#define IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <string_view>

namespace iris::marshal::detail {

template<class T, auto GetMem, auto SetMem>
struct field_definition
{
    using value_type = T;
    std::string_view name;
};

} // iris::marshal::detail

#endif

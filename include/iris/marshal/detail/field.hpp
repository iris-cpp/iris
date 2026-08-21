#ifndef IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP
#define IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <string_view>

namespace iris::marshal::detail {

template<auto Mem>
struct field_definition
{
    std::string_view name;
};

} // iris::marshal::detail

#endif

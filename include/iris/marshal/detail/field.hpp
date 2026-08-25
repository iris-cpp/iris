#ifndef IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP
#define IRIS_ZZ_MARSHAL_DETAIL_FIELD_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <string_view>
#include <functional>
#include <utility>
#include <type_traits>

namespace iris::marshal::detail {

template<class FieldT, auto GetMem, auto SetMem>
struct field_definition
{
    using value_type = FieldT;
    std::string_view name;

    template<class ClassT>
    [[nodiscard]] static constexpr decltype(auto) get(ClassT const& klass) noexcept
    {
        return std::invoke(GetMem, klass);
    }

    template<class ClassT, class T>
    static constexpr void set(ClassT& klass, T&& value)
    {
        if constexpr (std::is_member_object_pointer_v<decltype(SetMem)>) {
            (klass.*SetMem) = std::forward<T>(value);
        } else {
            std::invoke(SetMem, klass, std::forward<T>(value));
        }
    }
};

} // iris::marshal::detail

#endif

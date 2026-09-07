#ifndef IRIS_ZZ_ENUM_IO_HPP
#define IRIS_ZZ_ENUM_IO_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/enum/enum.hpp>
#include <iris/string.hpp>

#include <ostream> // IWYU pragma: keep
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

namespace iris {

inline namespace enum_operators {

template<Enum T>
std::ostream& operator<<(std::ostream& os, T const& val)
{
    using std::to_string;

    if constexpr (requires {
        { os << to_string(val) } -> std::convertible_to<std::ostream&>;
    }) {
        return os << to_string(val);
    } else {
        return os << std::to_underlying(val);
    }
}

} // enum_operators

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T parse_flag(StringLike auto const& str) noexcept
{
    return enum_traits<T>::parse(std::basic_string_view{str});
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T parse_flags(StringLike auto const& str, StringLike auto const& delim) noexcept
{
    using namespace iris::enum_bitops_operators;

    std::basic_string_view str_sv{str};
    std::basic_string_view delim_sv{delim};

    T res{};

    for (auto const& r : str_sv | std::views::split(delim_sv)) {
        std::basic_string_view const part_str{r.begin(), r.end()};

        auto const part = iris::parse_flag<T>(part_str);
        if (part == T{}) {
            return T{};
        }
        res |= part;
    }

    return res;
}

} // iris

#endif

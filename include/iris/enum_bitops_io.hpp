#ifndef IRIS_ENUM_BITOPS_IO_HPP
#define IRIS_ENUM_BITOPS_IO_HPP

// SPDX-License-Identifier: MIT

#include <iris/string.hpp>
#include <iris/enum_bitops.hpp>

#include <ostream>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

namespace iris {

inline namespace bitops_operators {

template<BitopsEnabledEnum T>
std::ostream& operator<<(std::ostream& os, T const& val)
{
    using std::to_string;

    if constexpr (requires {
        { to_string(val) } -> std::convertible_to<std::string const&>;
    }) {
        return os << to_string(val);
    } else {
        return os << std::to_underlying(val);
    }
}

} // bitops_operators

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T parse_flag(StringLike auto const& str) noexcept
{
    return bitops_enabled<T>::parse(std::basic_string_view{str});
}

template<BitopsEnabledEnum T>
[[nodiscard]] constexpr T parse_flags(StringLike auto const& str, StringLike auto const& delim) noexcept
{
    using namespace iris::bitops_operators;

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

namespace std {

template<::iris::BitopsEnabledEnum T, class CharT>
struct formatter<T, CharT> : formatter<std::underlying_type_t<T>, CharT>
{
    using base_formatter = formatter<std::underlying_type_t<T>, CharT>;

    template<class Context>
    constexpr auto parse(Context& ctx)
    {
        if (ctx.begin() == ctx.end()) return ctx.end();
        has_format_spec_ = true;
        return base_formatter::parse(ctx);
    }

    template<class Context>
    auto format(T const& val, Context& ctx) const
    {
        if (has_format_spec_) {
            return base_formatter::format(std::to_underlying(val), ctx);
        }
        return format_to(ctx.out(), "{}", to_string(val));
    }

private:
    bool has_format_spec_ = false;
};

} // std

#endif

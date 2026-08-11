#ifndef IRIS_ZZ_FORMAT_HPP
#define IRIS_ZZ_FORMAT_HPP

// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT

#include <iris/format_traits.hpp>

#include <format>

namespace iris {

template<class CharT>
struct no_spec_formatter
{
    static constexpr std::basic_format_parse_context<CharT>::const_iterator
    parse(std::basic_format_parse_context<CharT>& ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (*it == format_traits<CharT>::brace_close) return it;
        throw std::format_error("unknown format specifier");
    }
};

} // iris

#endif

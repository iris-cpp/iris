#ifndef IRIS_ZZ_IO_FWD_HPP
#define IRIS_ZZ_IO_FWD_HPP

// SPDX-License-Identifier: MIT

#include <concepts>
#include <iosfwd>
#include <string>
#include <type_traits>

namespace iris::req {

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
struct is_ADL_ostreamable : std::false_type {};

template<class T, class CharT, class CharTraits>
    requires requires(std::basic_ostream<CharT, CharTraits>& os, T const& val) {
        { os << val } -> std::same_as<std::basic_ostream<CharT, CharTraits>&>;
    }
struct is_ADL_ostreamable<T, CharT, CharTraits> : std::true_type {};

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
constexpr bool is_ADL_ostreamable_v = is_ADL_ostreamable<T, CharT, CharTraits>::value;

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
concept ADL_ostreamable = is_ADL_ostreamable_v<T, CharT, CharTraits>;

}  // iris::req

#endif

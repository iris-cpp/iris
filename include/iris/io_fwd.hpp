#ifndef IRIS_ZZ_IO_FWD_HPP
#define IRIS_ZZ_IO_FWD_HPP

// SPDX-License-Identifier: MIT

#include <concepts>
#include <iosfwd>
#include <type_traits>

namespace iris::req {

namespace detail {

namespace ADL_ostreamable_poison_pill {

template<class CharT, class CharTraits, class T>
void operator<<(std::basic_ostream<CharT, CharTraits>&, T const&) = delete;

template<class T, class CharT, class CharTraits>
struct is_ADL_ostreamable_impl : std::false_type {};

template<class T, class CharT, class CharTraits>
    requires requires(std::basic_ostream<CharT, CharTraits>& os, T const& val) {
        { os << val } -> std::same_as<std::basic_ostream<CharT, CharTraits>&>;
    }
struct is_ADL_ostreamable_impl<T, CharT, CharTraits> : std::true_type {};

} // ADL_ostreamable_poison_pill

} // detail

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
struct is_ADL_ostreamable : detail::ADL_ostreamable_poison_pill::is_ADL_ostreamable_impl<T, CharT, CharTraits> {};

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
constexpr bool is_ADL_ostreamable_v = is_ADL_ostreamable<T, CharT, CharTraits>::value;

template<class T, class CharT = char, class CharTraits = std::char_traits<CharT>>
concept ADL_ostreamable = is_ADL_ostreamable_v<T, CharT, CharTraits>;

}  // iris::req

#endif

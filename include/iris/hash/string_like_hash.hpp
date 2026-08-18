#ifndef IRIS_ZZ_HASH_STRING_LIKE_HASH_HPP
#define IRIS_ZZ_HASH_STRING_LIKE_HASH_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/hash.hpp>

#include <string>
#include <string_view>
#include <functional>

namespace iris {

template<class CharT, class TraitsT = std::char_traits<CharT>>
struct basic_string_like_hash
{
    using is_transparent = int;

    [[nodiscard]] static std::size_t operator()(std::basic_string_view<CharT, TraitsT> sv)
        noexcept(is_nothrow_hashable_v<std::basic_string_view<CharT, TraitsT>>)
    {
        return std::hash<std::basic_string_view<CharT, TraitsT>>{}(sv);
    }
};

using string_like_hash = basic_string_like_hash<char>;
using wstring_like_hash = basic_string_like_hash<wchar_t>;
using u8string_like_hash = basic_string_like_hash<char8_t>;
using u16string_like_hash = basic_string_like_hash<char16_t>;
using u32string_like_hash = basic_string_like_hash<char32_t>;

} // iris

#endif

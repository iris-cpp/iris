#ifndef IRIS_ZZ_TEST_CATCH2_HPP
#define IRIS_ZZ_TEST_CATCH2_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/unicode/string.hpp>
#include <iris/string.hpp>

#include <catch2/catch_test_macros.hpp>  // IWYU pragma: export
#include <catch2/catch_tostring.hpp>

#include <concepts>
#include <type_traits>
#include <string>
#include <string_view>
#include <ranges>
#include <format>

#include <cstddef> // IWYU pragma: keep

template<std::formattable<char> T>
    requires (!std::ranges::range<T>)
struct Catch::StringMaker<T>
{
    static std::string convert(T const& value)
    {
        return std::format("{}", value);
    }
};

template<iris::CharLike CharT, std::size_t N>
    requires (!std::same_as<CharT, char> && !std::same_as<CharT, wchar_t>)
struct Catch::StringMaker<CharT[N]>
{
    static std::string convert(CharT const (&str)[N])
    {
        return iris::to_string_ref(str);
    }
};

template<iris::CharLike CharT>
    requires (!std::same_as<CharT, char> && !std::same_as<CharT, wchar_t>)
struct Catch::StringMaker<CharT const*>
{
    static std::string convert(CharT const* str)
    {
        return iris::to_string_ref(str);
    }
};

template<class CharT, class TraitsT, class AllocT>
    requires (!std::same_as<CharT, char> && !std::same_as<CharT, wchar_t>)
struct Catch::StringMaker<std::basic_string<CharT, TraitsT, AllocT>, std::enable_if_t<Catch::is_range<std::basic_string<CharT, TraitsT, AllocT>>::value && !::Catch::Detail::IsStreamInsertable_v<std::basic_string<CharT, TraitsT, AllocT>>>>
{
    static std::string convert(std::basic_string<CharT, TraitsT, AllocT> const& str)
    {
        return iris::to_string_ref(str);
    }
};

template<class CharT, class TraitsT>
    requires (!std::same_as<CharT, char> && !std::same_as<CharT, wchar_t>)
struct Catch::StringMaker<std::basic_string_view<CharT, TraitsT>, std::enable_if_t<Catch::is_range<std::basic_string_view<CharT, TraitsT>>::value && !::Catch::Detail::IsStreamInsertable_v<std::basic_string_view<CharT, TraitsT>>>>
{
    static std::string convert(std::basic_string_view<CharT, TraitsT> const& str)
    {
        return iris::to_string_ref(str);
    }
};

#endif

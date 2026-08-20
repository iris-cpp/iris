#ifndef IRIS_ZZ_TEST_IRIS_TEST_HPP
#define IRIS_ZZ_TEST_IRIS_TEST_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>  // IWYU pragma: export
#include <catch2/catch_tostring.hpp>

#include <ranges>
#include <format>

template<std::formattable<char> T>
    requires (!std::ranges::range<T>)
struct Catch::StringMaker<T>
{
    static std::string convert(T const& value)
    {
        return std::format("{}", value);
    }
};

#endif

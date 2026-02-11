// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/enum_bitops.hpp>
#include <iris/enum_bitops_algorithm.hpp>
#include <iris/enum_bitops_io.hpp>

#include <algorithm>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <cstdint>

enum class MyFlags : std::uint8_t
{
    FOO = 1 << 0,
    BAR = 1 << 1,
    BAZ = 1 << 2,
};

enum class SpellType : std::uint8_t
{
    TYPE_ATTACK  = 1 << 0,
    TYPE_DEFENSE = 1 << 1,

    ATTR_FIRE    = 1 << 2,
    ATTR_WATER   = 1 << 3,
    ATTR_THUNDER = 1 << 4,
};

template <>
struct iris::bitops_enabled<MyFlags> : std::true_type
{
    static MyFlags parse(std::string_view sv) noexcept
    {
        using enum MyFlags;
        if (sv == "foo") return FOO;
        if (sv == "bar") return BAR;
        if (sv == "baz") return BAZ;
        return {};
    }
};

template <>
struct iris::bitops_enabled<SpellType> : std::true_type
{
    static constexpr int min_bit = 2;
    static constexpr int max_bit = 4;
};

TEST_CASE("enum")
{
    using namespace iris::bitops_operators;

    using enum MyFlags;

    CHECK((~FOO == static_cast<MyFlags>(~std::to_underlying(FOO))));

    CHECK(((FOO & BAR) == static_cast<MyFlags>(std::to_underlying(FOO) & std::to_underlying(BAR))));
    CHECK(((FOO ^ BAR) == static_cast<MyFlags>(std::to_underlying(FOO) ^ std::to_underlying(BAR))));
    CHECK(((FOO | BAR) == static_cast<MyFlags>(std::to_underlying(FOO) | std::to_underlying(BAR))));

    CHECK( iris::contains(FOO, FOO));
    CHECK(!iris::contains(FOO, BAR));
    CHECK( iris::contains(FOO | BAR, FOO));
    CHECK(!iris::contains(FOO | BAR, BAZ));
    CHECK(!iris::contains(FOO |       BAZ, FOO | BAR));
    CHECK(!iris::contains(      BAR | BAZ, FOO | BAR));
    CHECK( iris::contains(FOO | BAR | BAZ, FOO | BAR));

    CHECK( iris::contains_any_bit(FOO, FOO));
    CHECK(!iris::contains_any_bit(FOO, BAR));
    CHECK( iris::contains_any_bit(FOO | BAR, FOO));
    CHECK(!iris::contains_any_bit(FOO | BAR, BAZ));
    CHECK( iris::contains_any_bit(FOO |       BAZ, FOO | BAR));
    CHECK( iris::contains_any_bit(      BAR | BAZ, FOO | BAR));
    CHECK( iris::contains_any_bit(FOO | BAR | BAZ, FOO | BAR));

    CHECK( iris::contains_single_bit(FOO, FOO));
    CHECK(!iris::contains_single_bit(FOO, BAR));
    CHECK( iris::contains_single_bit(FOO | BAR, FOO));
    CHECK(!iris::contains_single_bit(FOO | BAR, BAZ));

    CHECK((iris::parse_flag<MyFlags>("foo") == FOO));
    CHECK((iris::parse_flag<MyFlags>("bar") == BAR));
    CHECK((iris::parse_flag<MyFlags>("baz") == BAZ));
    CHECK((iris::parse_flag<MyFlags>("yay") == MyFlags{}));

    CHECK((iris::parse_flags<MyFlags>("foo", "|") == FOO));
    CHECK((iris::parse_flags<MyFlags>("yay", "|") == MyFlags{}));

    CHECK((iris::parse_flags<MyFlags>("foo|bar", "|") == (FOO | BAR)));
    CHECK((iris::parse_flags<MyFlags>("foo|yay", "|") == MyFlags{}));
    CHECK((iris::parse_flags<MyFlags>("foo,bar", "|") == MyFlags{}));

    CHECK(std::ranges::equal(
        iris::each_bit(SpellType::TYPE_ATTACK | SpellType::ATTR_FIRE | SpellType::ATTR_THUNDER),
        std::vector{SpellType::ATTR_FIRE, SpellType::ATTR_THUNDER}
    ));

    CHECK(std::ranges::equal(
        iris::each_bit(static_cast<SpellType>(static_cast<std::underlying_type_t<SpellType>>(-1))),
        std::vector{SpellType::ATTR_FIRE, SpellType::ATTR_WATER, SpellType::ATTR_THUNDER}
    ));
}

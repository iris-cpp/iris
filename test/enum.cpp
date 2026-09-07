// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/enum/enum.hpp>
#include <iris/enum/io.hpp>

#include <algorithm>
#include <string_view>
#include <utility>
#include <vector>

#include <cstdint>

enum PlainEnum {};
enum struct PlainScopedEnum {};

enum struct Enum_0_4 : unsigned {};
enum struct Enum_2_4 : unsigned {};

enum struct BitEnum_0_4 : unsigned {};
enum struct BitEnum_2_4 : unsigned {};

template<>
struct iris::enum_traits<Enum_0_4>
{
    static constexpr Enum_0_4 max_element = Enum_0_4{0};
};
template<>
struct iris::enum_traits<Enum_2_4>
{
    static constexpr Enum_2_4 min_element = Enum_2_4{2};
    static constexpr Enum_2_4 max_element = Enum_2_4{4};
};

template<>
struct iris::enum_traits<BitEnum_0_4>
{
    static constexpr int max_bit = 4;
};
template<>
struct iris::enum_traits<BitEnum_2_4>
{
    static constexpr int min_bit = 2;
    static constexpr int max_bit = 4;
};

TEST_CASE("enum")
{
    STATIC_CHECK(!iris::Enum<PlainEnum>);
    STATIC_CHECK(!iris::Enum<PlainScopedEnum>);

    STATIC_CHECK(iris::BitopsEnabledEnum<Enum_0_4>);
    STATIC_CHECK(!iris::FiniteEnum<Enum_0_4>);

    STATIC_CHECK(iris::BitopsEnabledEnum<Enum_2_4>);
    STATIC_CHECK(iris::FiniteEnum<Enum_2_4>);
    STATIC_CHECK(iris::enum_count_v<Enum_2_4> == 3);
    STATIC_CHECK(iris::enum_values<Enum_2_4>() == std::array{Enum_2_4{2}, Enum_2_4{3}, Enum_2_4{4}});

    STATIC_CHECK(iris::BitopsEnabledEnum<BitEnum_0_4>);
    STATIC_CHECK(iris::FiniteEnum<BitEnum_0_4>);
    STATIC_CHECK(iris::enum_count_v<BitEnum_0_4> == 5);
    STATIC_CHECK(iris::enum_values<BitEnum_0_4>() == std::array{BitEnum_0_4{1u << 0}, BitEnum_0_4{1u << 1}, BitEnum_0_4{1u << 2}, BitEnum_0_4{1u << 3}, BitEnum_0_4{1u << 4}});

    STATIC_CHECK(iris::BitopsEnabledEnum<BitEnum_2_4>);
    STATIC_CHECK(iris::FiniteEnum<BitEnum_2_4>);
    STATIC_CHECK(iris::enum_count_v<BitEnum_2_4> == 3);
    STATIC_CHECK(iris::enum_values<BitEnum_2_4>() == std::array{BitEnum_2_4{1u << 2}, BitEnum_2_4{1u << 3}, BitEnum_2_4{1u << 4}});
}

enum class MyFlags : std::uint8_t
{
    NO_FLAGS = 0u,
    FOO = 1u << 0,
    BAR = 1u << 1,
    BAZ = 1u << 2,
};

enum class SpellType : std::uint8_t
{
    NO_SPELL_TYPE = 0u,

    TYPE_ATTACK  = 1u << 0,
    TYPE_DEFENSE = 1u << 1,

    ATTR_FIRE    = 1u << 2,
    ATTR_WATER   = 1u << 3,
    ATTR_THUNDER = 1u << 4,
};

template<>
struct iris::enum_traits<MyFlags>
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

template<>
struct iris::enum_traits<SpellType>
{
    static constexpr int min_bit = 2;
    static constexpr int max_bit = 4;
};

TEST_CASE("enum: flags")
{
    using namespace iris::enum_bitops_operators;

    using enum MyFlags;

    static_assert(iris::Enum<MyFlags>);

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

    static_assert(iris::detail::enum_has_finite_minmax_bit<SpellType>);

    CHECK(std::ranges::equal(
        iris::each_bit(SpellType::TYPE_ATTACK | SpellType::ATTR_FIRE | SpellType::ATTR_THUNDER),
        std::vector{SpellType::ATTR_FIRE, SpellType::ATTR_THUNDER}
    ));

    CHECK(std::ranges::equal(
        iris::each_bit(static_cast<SpellType>(static_cast<std::underlying_type_t<SpellType>>(-1))),
        std::vector{SpellType::ATTR_FIRE, SpellType::ATTR_WATER, SpellType::ATTR_THUNDER}
    ));
}

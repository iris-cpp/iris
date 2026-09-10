#include "iris_test.hpp"

#include <iris/bitset.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <bitset>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <format>
#include <concepts>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

#include <cstddef> // IWYU pragma: keep
#include <cstdint>

namespace {

template<std::size_t N, class T>
struct bitset_test_cfg
{
    static constexpr std::size_t n = N;
    using word = T;
};

#define IRIS_TEST_BITSET_CFGS \
    (bitset_test_cfg<1, std::uint8_t>), (bitset_test_cfg<7, std::uint8_t>), (bitset_test_cfg<8, std::uint8_t>), \
    (bitset_test_cfg<9, std::uint8_t>), (bitset_test_cfg<16, std::uint8_t>), (bitset_test_cfg<37, std::uint8_t>), \
    (bitset_test_cfg<37, std::uint16_t>), (bitset_test_cfg<64, std::uint8_t>), (bitset_test_cfg<64, std::uint64_t>), \
    (bitset_test_cfg<65, std::uint64_t>), (bitset_test_cfg<100, std::uint8_t>), (bitset_test_cfg<130, std::uint64_t>)

#define IRIS_TEST_BITSET_POSITIONS_CFGS \
    (bitset_test_cfg<0, std::uint8_t>), (bitset_test_cfg<1, std::uint8_t>), (bitset_test_cfg<8, std::uint8_t>), (bitset_test_cfg<37, std::uint16_t>), \
    (bitset_test_cfg<64, std::uint8_t>), (bitset_test_cfg<65, std::uint64_t>), (bitset_test_cfg<130, std::uint8_t>)


constexpr std::array<unsigned long long, 9> patterns{
    0ULL,
    1ULL,
    0x80ULL,
    0xFFULL,
    0x0100ULL,
    0x1234'5678'9ABC'DEF0ULL,
    0x8000'0000'0000'0000ULL,
    0xAAAA'AAAA'AAAA'AAAAULL,
    0xFFFF'FFFF'FFFF'FFFFULL,
};

template<std::size_t N, class T>
std::string as_std_string(iris::bitset<N, T> const& b)
{
    return b.to_string();
}

template<std::size_t N>
std::string as_std_string(std::bitset<N> const& b)
{
    return b.to_string();
}

enum class Flag : unsigned { a = 0, b = 3, c = 7 };

} // anonymous

// ----------------------------------------------------
// Static properties

TEST_CASE("type traits", "[bitset]")
{
    STATIC_REQUIRE(std::same_as<iris::bitset<0>::word_type, std::uint_least8_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<1>::word_type, std::uint_least8_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<8>::word_type, std::uint_least8_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<9>::word_type, std::uint_least16_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<16>::word_type, std::uint_least16_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<17>::word_type, std::uint_least32_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<32>::word_type, std::uint_least32_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<33>::word_type, std::uint_least64_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<64>::word_type, std::uint_least64_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<65>::word_type, std::uint_least64_t>);
    STATIC_REQUIRE(std::same_as<iris::bitset<1000>::word_type, std::uint_least64_t>);

    STATIC_REQUIRE(sizeof(iris::bitset<0>) == 1);
    STATIC_REQUIRE(sizeof(iris::bitset<8>) == 1);
    STATIC_REQUIRE(sizeof(iris::bitset<9>) == 2);
    STATIC_REQUIRE(sizeof(iris::bitset<64>) == 8);
    STATIC_REQUIRE(sizeof(iris::bitset<65>) == 16);
    STATIC_REQUIRE(sizeof(iris::bitset<64, std::uint8_t>) == 8);
    STATIC_REQUIRE(alignof(iris::bitset<64, std::uint8_t>) == 1);
    STATIC_REQUIRE(sizeof(iris::bitset<100, std::uint8_t>) == 13);

    STATIC_REQUIRE(std::is_convertible_v<unsigned long long, iris::bitset<8>>);
    STATIC_REQUIRE(std::is_convertible_v<int, iris::bitset<8>>);
    STATIC_REQUIRE(std::is_constructible_v<iris::bitset<8>, char const*>);
    STATIC_REQUIRE(!std::is_convertible_v<char const*, iris::bitset<8>>);
    STATIC_REQUIRE(std::is_constructible_v<iris::bitset<8>, std::string>);
    STATIC_REQUIRE(!std::is_convertible_v<std::string, iris::bitset<8>>);
    STATIC_REQUIRE(std::is_constructible_v<iris::bitset<8>, std::string_view>);
    STATIC_REQUIRE(!std::is_convertible_v<std::string_view, iris::bitset<8>>);

    STATIC_REQUIRE(std::is_trivially_copyable_v<iris::bitset<8>>);
    STATIC_REQUIRE(std::is_trivially_copyable_v<iris::bitset<100, std::uint8_t>>);
    STATIC_REQUIRE(std::is_standard_layout_v<iris::bitset<100, std::uint8_t>>);
}

// ----------------------------------------------------
// Construction

TEMPLATE_TEST_CASE("construction", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    {
        B const b;
        CHECK(b.none());
        CHECK(!b.any());
        CHECK(b.count() == 0);
        CHECK(b.size() == N);
        CHECK(B::bits == N);
        CHECK(as_std_string(b) == as_std_string(std::bitset<N>{}));
        CHECK(b == B{});
    }

    for (auto const v : patterns) {
        B const b(v);
        std::bitset<N> const s(v);
        INFO("value = " << v);
        CHECK(as_std_string(b) == as_std_string(s));
        CHECK(b.count() == s.count());
        CHECK(b.to_ullong() == s.to_ullong());
    }

    SECTION("string_view")
    {
        CHECK(iris::bitset<8>(std::string_view{"1010"}).to_ulong() == 0b1010);
        CHECK(iris::bitset<8>(std::string_view{"0110"}).to_string() == "00000110");
        CHECK(iris::bitset<4>(std::string_view{"11110000"}).to_ulong() == 0b1111);
        CHECK(iris::bitset<4>(std::string_view{"11110000"}).to_string() == std::bitset<4>("11110000").to_string());
        CHECK(iris::bitset<8>(std::string_view{"xx1010"}, 2).to_ulong() == 0b1010);
        CHECK(iris::bitset<8>(std::string_view{"xx1010yy"}, 2, 4).to_ulong() == 0b1010);
        CHECK(iris::bitset<8>(std::string_view{"abab"}, 0, std::string_view::npos, 'a', 'b').to_ulong() == 0b0101);
        CHECK(iris::bitset<8>(std::string_view{"1010"}, 4).none());
    }
    SECTION("basic_string")
    {
        std::string const str = "xx1010yy";
        CHECK(iris::bitset<8>(str, 2, 4).to_ulong() == std::bitset<8>(str, 2, 4).to_ulong());
        CHECK_THROWS_AS(iris::bitset<8>(str, 2), std::invalid_argument);
        CHECK(iris::bitset<8>(std::string{"xx1010"}, 2).to_string() == std::bitset<8>(std::string{"xx1010"}, 2).to_string());
        CHECK(iris::bitset<8>(std::string{"ab"}, 0, 2, 'a', 'b').to_ulong() == 0b01);
    }
    SECTION("CharT const*")
    {
        CHECK(iris::bitset<8>("1010").to_ulong() == std::bitset<8>("1010").to_ulong());
        CHECK(iris::bitset<8>("101011", 4).to_ulong() == std::bitset<8>("101011", 4).to_ulong());
        CHECK(iris::bitset<8>("ab", 2, 'a', 'b').to_ulong() == 0b01);
        CHECK(iris::bitset<8>("ab", std::string_view::npos, 'a', 'b').to_ulong() == 0b01);
    }
    SECTION("wide characters")
    {
        CHECK(iris::bitset<8>(L"1010").to_ulong() == 0b1010);
        CHECK(iris::bitset<8>(std::wstring_view{L"1010"}).to_string<wchar_t>() == L"00001010");
    }
    SECTION("exceptions")
    {
        CHECK_THROWS_AS(iris::bitset<8>("012"), std::invalid_argument);
        CHECK_THROWS_AS(iris::bitset<8>("1010", 3, 'a', 'b'), std::invalid_argument);
        CHECK_THROWS_AS(iris::bitset<8>("01", 3), std::invalid_argument);
        CHECK_THROWS_AS(iris::bitset<8>(std::string{"01"}, 3), std::out_of_range);
        CHECK_THROWS_AS(iris::bitset<8>(std::string_view{"01"}, 3), std::out_of_range);
        CHECK_THROWS_AS(iris::bitset<2>("11x"), std::invalid_argument);
    }
}

TEST_CASE("from_positions", "[bitset]")
{
    using B = iris::bitset<16, std::uint8_t>;

    SECTION("initializer_list")
    {
        CHECK(B::from_positions({0, 9, 15}).to_string() == "1000001000000001");
        CHECK(B::from_positions({3, 3, 3}).count() == 1);
    }
    SECTION("range")
    {
        std::vector<int> const v{1, 2, 4};
        CHECK(B::from_positions(v).to_ulong() == 0b10110);
        CHECK(B::from_positions(std::array{15}).to_ulong() == 0x8000);
    }
    SECTION("scoped enum elements")
    {
        CHECK(B::from_positions({Flag::a, Flag::c}).to_ulong() == 0b1000'0001);
        CHECK(B::from_positions(std::array{Flag::b}).to_ulong() == 0b1000);
    }
    SECTION("out of range")
    {
        CHECK_THROWS_AS(B::from_positions({16}), std::out_of_range);
        CHECK_THROWS_AS(B::from_positions(std::vector<std::size_t>{0, 100}), std::out_of_range);
        CHECK_THROWS_AS(iris::bitset<0>::from_positions({0}), std::out_of_range);
    }
}

// ----------------------------------------------------
// Element access and single-bit modifiers

TEMPLATE_TEST_CASE("single-bit operations", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    {
        B b;
        std::bitset<N> s;

        // Walk every position with a deterministic mix of set / reset / flip.
        for (std::size_t pos = 0; pos < N; ++pos) {
            switch (pos % 4) {
            case 0:
                b.set(pos);
                s.set(pos);
                break;
            case 1:
                b.set(pos, true);
                s.set(pos, true);
                break;
            case 2:
                b.flip(pos);
                s.flip(pos);
                break;
            default:
                b.set(pos, false);
                s.set(pos, false);
                break;
            }
            REQUIRE(as_std_string(b) == as_std_string(s));
        }

        for (std::size_t pos = 0; pos < N; ++pos) {
            REQUIRE(b[pos] == s[pos]);
            REQUIRE(b.test(pos) == s.test(pos));
        }

        for (std::size_t pos = 0; pos < N; pos += 3) {
            b.reset(pos);
            s.reset(pos);
        }
        CHECK(as_std_string(b) == as_std_string(s));
        CHECK(b.count() == s.count());
    }
    {
        B b;
        CHECK_THROWS_AS(b.set(N), std::out_of_range);
        CHECK_THROWS_AS(b.set(N, false), std::out_of_range);
        CHECK_THROWS_AS(b.reset(N), std::out_of_range);
        CHECK_THROWS_AS(b.flip(N), std::out_of_range);
        CHECK_THROWS_AS((void)b.test(N), std::out_of_range);
        CHECK_THROWS_AS(b.set(static_cast<std::size_t>(-1)), std::out_of_range);
        CHECK(b.none());
    }
}

TEST_CASE("modifier chaining", "[bitset]")
{
    iris::bitset<8> b;
    CHECK(b.set().reset(0).flip(7).to_ulong() == 0b0111'1110);
    CHECK(b.reset().set(1).to_ulong() == 0b10);
    CHECK(b.flip().to_ulong() == 0b1111'1101);
    STATIC_REQUIRE(std::same_as<decltype(b.reset()), iris::bitset<8>&>);
    STATIC_REQUIRE(std::same_as<decltype(b.set()), iris::bitset<8>&>);
    STATIC_REQUIRE(std::same_as<decltype(b.flip()), iris::bitset<8>&>);
}

// ----------------------------------------------------
// Modifiers and acccessors

TEMPLATE_TEST_CASE("set/reset/flip on all bits", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    for (auto const v : patterns) {
        B b(v);
        std::bitset<N> s(v);
        INFO("value = " << v);

        b.flip();
        s.flip();
        CHECK(as_std_string(b) == as_std_string(s));
        CHECK(as_std_string(~b) == as_std_string(~s));

        b.set();
        s.set();
        CHECK(as_std_string(b) == as_std_string(s));
        CHECK(b.all());
        CHECK(b.count() == N);

        b.reset();
        s.reset();
        CHECK(as_std_string(b) == as_std_string(s));
        CHECK(b.none());
    }
}

TEMPLATE_TEST_CASE("all/any/none/count", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    for (auto const v : patterns) {
        B const b(v);
        std::bitset<N> const s(v);
        INFO("value = " << v);
        CHECK(b.all() == s.all());
        CHECK(b.any() == s.any());
        CHECK(b.none() == s.none());
        CHECK(b.count() == s.count());
    }

    // Partially used last word: all() must look only at the valid bits.
    B full;
    full.set();
    CHECK(full.all());
    if constexpr (N > 0) {
        full.reset(N - 1);
        CHECK(!full.all());
        CHECK(full.any() == (N > 1));
    }
}

TEST_CASE("N == 0", "[bitset]")
{
    using B = iris::bitset<0>;
    B b;
    CHECK(b.size() == 0);
    CHECK(b.count() == 0);
    CHECK(b.all());
    CHECK(b.none());
    CHECK(!b.any());
    CHECK(b.to_ullong() == 0);
    CHECK(b.to_string().empty());
    CHECK(b.flip().count() == 0); // padding must not become visible
    CHECK(b.set().count() == 0);
    CHECK(b.all());
    CHECK((b << 5) == B{});
    CHECK((b >> 5) == B{});
    CHECK(b == B{});
    CHECK((b <=> B{}) == std::strong_ordering::equal);
    CHECK(B(0xFF).none());
    CHECK_THROWS_AS(b.set(0), std::out_of_range);
    CHECK_THROWS_AS((void)b.test(0), std::out_of_range);
    CHECK(std::format("[{}]", b) == "[]");
    CHECK(std::hash<B>{}(b) == std::hash<B>{}(B{}));
}

// ----------------------------------------------------
// Bitwise operators

TEMPLATE_TEST_CASE("bitwise operators", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    for (auto const x : patterns) {
        for (auto const y : patterns) {
            B const bx(x);
            B const by(y);
            std::bitset<N> const sx(x);
            std::bitset<N> const sy(y);
            INFO("x = " << x << ", y = " << y);

            CHECK(as_std_string(bx & by) == as_std_string(sx & sy));
            CHECK(as_std_string(bx | by) == as_std_string(sx | sy));
            CHECK(as_std_string(bx ^ by) == as_std_string(sx ^ sy));

            B b = bx;
            std::bitset<N> s = sx;
            b &= by;
            s &= sy;
            CHECK(as_std_string(b) == as_std_string(s));

            b = bx;
            s = sx;
            b |= by;
            s |= sy;
            CHECK(as_std_string(b) == as_std_string(s));

            b = bx;
            s = sx;
            b ^= by;
            s ^= sy;
            CHECK(as_std_string(b) == as_std_string(s));
        }
    }
}

TEMPLATE_TEST_CASE("shifts", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    std::vector<std::size_t> shifts;
    for (std::size_t i = 0; i <= N + 2; ++i) {
        shifts.push_back(i);
    }
    shifts.push_back(200);
    shifts.push_back(1000);

    for (auto const v : patterns) {
        B const b(v);
        std::bitset<N> const s(v);
        for (auto const sh : shifts) {
            INFO("value = " << v << ", shift = " << sh);
            REQUIRE(as_std_string(b << sh) == as_std_string(s << sh));
            REQUIRE(as_std_string(b >> sh) == as_std_string(s >> sh));

            B bl = b;
            std::bitset<N> sl = s;
            bl <<= sh;
            sl <<= sh;
            REQUIRE(as_std_string(bl) == as_std_string(sl));

            B br = b;
            std::bitset<N> sr = s;
            br >>= sh;
            sr >>= sh;
            REQUIRE(as_std_string(br) == as_std_string(sr));
        }
    }
}

TEST_CASE("shifted-out bits never leak into padding", "[bitset]")
{
    iris::bitset<12, std::uint8_t> b(0xFFF);
    b <<= 4;
    CHECK(b.to_ulong() == 0xFF0);
    CHECK(b.count() == 8);
    CHECK((~b).to_ulong() == 0x00F);
    CHECK(b == iris::bitset<12, std::uint8_t>(0xFF0));
    CHECK(std::hash<iris::bitset<12, std::uint8_t>>{}(b) == std::hash<iris::bitset<12, std::uint8_t>>{}(iris::bitset<12, std::uint8_t>(0xFF0)));
}

// ----------------------------------------------------
// Comparison

TEMPLATE_TEST_CASE("equality", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    for (auto const x : patterns) {
        for (auto const y : patterns) {
            INFO("x = " << x << ", y = " << y);
            CHECK((B(x) == B(y)) == (std::bitset<N>(x) == std::bitset<N>(y)));
            CHECK((B(x) != B(y)) == (std::bitset<N>(x) != std::bitset<N>(y)));
        }
    }
}

TEST_CASE("ordering", "[bitset]")
{
    using B8 = iris::bitset<16, std::uint8_t>;
    using B16 = iris::bitset<16, std::uint16_t>;
    using B64 = iris::bitset<16, std::uint64_t>;

    CHECK(B8(0x0100) > B8(0x00FF));
    CHECK(B16(0x0100) > B16(0x00FF));
    CHECK(B64(0x0100) > B64(0x00FF));

    std::vector<unsigned> const values{0, 0xFF, 0x0100, 0x0101, 0x8000, 0xFFFF, 0x00FE, 0x7FFF};
    std::vector<unsigned> sorted_by_value = values;
    std::ranges::sort(sorted_by_value);

    auto const check = [&]<class B>(std::type_identity<B>) {
        std::vector<B> sorted;
        for (auto const v : values) {
            sorted.emplace_back(v);  // NOLINT(performance-inefficient-vector-operation)
        }
        std::ranges::sort(sorted);
        for (std::size_t i = 0; i < sorted.size(); ++i) {
            REQUIRE(sorted[i].to_ulong() == sorted_by_value[i]);
        }
    };
    check(std::type_identity<B8>{});
    check(std::type_identity<B16>{});
    check(std::type_identity<B64>{});

    using W = iris::bitset<100, std::uint8_t>;
    W hi;
    hi.set(99);
    W lo;
    lo.set();
    lo.reset(99);
    CHECK(hi > lo);
    CHECK((hi <=> lo) == std::strong_ordering::greater);
    CHECK((lo <=> hi) == std::strong_ordering::less);
    CHECK((hi <=> hi) == std::strong_ordering::equal);
    CHECK(std::set<W>{lo, hi, W{}}.size() == 3);
}

// ----------------------------------------------------
// Conversions

TEMPLATE_TEST_CASE("to_ulong/to_ullong", "[bitset]", IRIS_TEST_BITSET_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    for (auto const v : patterns) {
        B const b(v);
        std::bitset<N> const s(v);
        INFO("value = " << v);
        CHECK(b.to_ullong() == s.to_ullong());

        bool const fits_ulong = s.to_ullong() <= static_cast<unsigned long long>(~0UL);  // NOLINT(bugprone-misplaced-widening-cast)
        if (fits_ulong) {
            CHECK(b.to_ulong() == s.to_ulong());
        } else {
            CHECK_THROWS_AS((void)b.to_ulong(), std::overflow_error);
            CHECK_THROWS_AS((void)s.to_ulong(), std::overflow_error);
        }
    }

    {
        iris::bitset<70> b;
        b.set(64);
        CHECK_THROWS_AS((void)b.to_ullong(), std::overflow_error);
        b.reset(64);
        b.set(63);
        CHECK(b.to_ullong() == 0x8000'0000'0000'0000ULL);

        iris::bitset<130, std::uint8_t> w;
        w.set(129);
        CHECK_THROWS_AS((void)w.to_ullong(), std::overflow_error);
        w.reset(129);
        w.set(0);
        CHECK(w.to_ullong() == 1);

        if constexpr (sizeof(unsigned long) == 4) {
            iris::bitset<64, std::uint64_t> x;
            x.set(32);
            CHECK_THROWS_AS((void)x.to_ulong(), std::overflow_error);
            x.reset(32);
            x.set(31);
            CHECK(x.to_ulong() == 0x8000'0000UL);
        }
    }
}

TEST_CASE("to_string", "[bitset]")
{
    iris::bitset<8> const b(0b0010'1101);
    CHECK(b.to_string() == "00101101");
    CHECK(b.to_string() == std::bitset<8>(0b0010'1101).to_string());
    CHECK(b.to_string('.', '#') == "..#.##.#");
    CHECK(b.to_string<wchar_t>() == L"00101101");
    CHECK(b.to_string<wchar_t>(L'_', L'X') == L"__X_XX_X");
    CHECK(iris::bitset<130, std::uint8_t>{}.set(129).to_string() == std::bitset<130>{}.set(129).to_string());
}

TEST_CASE("data()", "[bitset]")
{
    iris::bitset<20, std::uint8_t> const b(0x0A0B0C);
    auto const d = b.data();
    STATIC_REQUIRE(std::same_as<decltype(d), std::span<std::uint8_t const, 3> const>);
    CHECK(d[0] == 0x0C);
    CHECK(d[1] == 0x0B);
    CHECK(d[2] == 0x0A);

    iris::bitset<0> const z;
    CHECK(z.data().size() == 1);
    CHECK(z.data()[0] == 0);
}

// ----------------------------------------------------
// Hashing and formatting

TEST_CASE("std::hash", "[bitset]")
{
    using B = iris::bitset<100, std::uint8_t>;
    std::hash<B> const h;

    CHECK(h(B(5)) == h(B(5)));
    CHECK(h(B{}.set(99)) == h(B{}.set(99)));

    std::unordered_set<B> set;
    set.insert(B(1));
    set.insert(B(1));
    set.insert(B(2));
    CHECK(set.size() == 2);
    CHECK(set.contains(B(1)));
    CHECK(!set.contains(B(3)));
}

TEST_CASE("std::formatter", "[bitset]")
{
    iris::bitset<8> const b(5);
    CHECK(std::format("{}", b) == "00000101");
    CHECK(std::format("{}", b) == std::bitset<8>(5).to_string());
    CHECK(std::format("{:>12}", b) == "    00000101");
    CHECK(std::format("{:<12}|", b) == "00000101    |");
    CHECK(std::format("{:*^12}", b) == "**00000101**");
    CHECK(std::format("{:.4}", b) == "0000");
    CHECK(std::format(L"{}", iris::bitset<4>(0b1010)) == L"1010");
    CHECK(std::format("{}", iris::bitset<130, std::uint8_t>{}.set(129)) == std::bitset<130>{}.set(129).to_string());
}

// ----------------------------------------------------
// Constant evaluation

TEST_CASE("usable in constant expressions", "[bitset]")
{
    using B = iris::bitset<12, std::uint8_t>;

    STATIC_REQUIRE(B(0xABC).to_ulong() == 0xABC);
    STATIC_REQUIRE(B("101").to_ulong() == 0b101);
    STATIC_REQUIRE(B(std::string_view{"xx101"}, 2).to_ulong() == 0b101);
    STATIC_REQUIRE(B::from_positions({0, 11}).to_ulong() == 0x801);
    STATIC_REQUIRE(B{}.set().count() == 12);
    STATIC_REQUIRE(B{}.set(3).flip(3).none());
    STATIC_REQUIRE(B{}.set().reset(11).to_ulong() == 0x7FF);
    STATIC_REQUIRE((B(0xF0F) & B(0x0FF)) == B(0x00F));
    STATIC_REQUIRE((B(0xF0F) | B(0x0F0)) == B(0xFFF));
    STATIC_REQUIRE((B(0xF0F) ^ B(0xFFF)) == B(0x0F0));
    STATIC_REQUIRE((~B(0xF0F)) == B(0x0F0));
    STATIC_REQUIRE((B(1) << 11) == B(0x800));
    STATIC_REQUIRE((B(1) << 12).none());
    STATIC_REQUIRE((B(0x800) >> 11) == B(1));
    STATIC_REQUIRE(B(0x100) > B(0x0FF));
    STATIC_REQUIRE(B(0x0FF).all() == false);
    STATIC_REQUIRE(B(0xFFF).all());
    STATIC_REQUIRE(B(0xFFF).any());
    STATIC_REQUIRE(B(0).none());
    STATIC_REQUIRE(B(0xABC).to_string() == "101010111100");
    STATIC_REQUIRE(B(0xABC)[2] && !B(0xABC)[0]);
    STATIC_REQUIRE(B(0xABC).test(2));
    STATIC_REQUIRE(B(0xABC).data()[1] == 0x0A);

    using W = iris::bitset<130, std::uint64_t>;
    STATIC_REQUIRE(W{}.set(129).count() == 1);
    STATIC_REQUIRE((W{}.set(129) >> 129) == W(1));
    STATIC_REQUIRE((W(1) << 129).test(129));
    STATIC_REQUIRE(W{}.set().count() == 130);
    STATIC_REQUIRE(W{}.set(129) > W{}.set(128));
}

#undef IRIS_TEST_BITSET_CFGS

// -----------------------------------------------------

TEST_CASE("positions(): range properties", "[bitset]")
{
    using V = decltype(std::declval<iris::bitset<130, std::uint8_t> const&>().positions());
    STATIC_REQUIRE(std::ranges::view<V>);
    STATIC_REQUIRE(std::ranges::bidirectional_range<V>);
    STATIC_REQUIRE(std::ranges::common_range<V>);
    STATIC_REQUIRE(std::ranges::sized_range<V>);
    STATIC_REQUIRE(!std::ranges::random_access_range<V>);

    using B = iris::bitset<100, std::uint8_t>;
    constexpr auto b = B::from_positions({0, 7, 8, 63, 64, 99});
    STATIC_REQUIRE(std::ranges::equal(b.positions(), std::array<std::size_t, 6>{0, 7, 8, 63, 64, 99}));
    STATIC_REQUIRE(std::ranges::equal(b.positions() | std::views::reverse, std::array<std::size_t, 6>{99, 64, 63, 8, 7, 0}));
    STATIC_REQUIRE(b.positions().size() == 6);
    STATIC_REQUIRE(B{}.positions().empty());
}

TEMPLATE_TEST_CASE("positions(): matches a naive scan in both directions", "[bitset]", IRIS_TEST_BITSET_POSITIONS_CFGS)
{
    constexpr std::size_t N = TestType::n;
    using B = iris::bitset<N, typename TestType::word>;

    std::vector<B> inputs{B{}, B{}.set()};
    for (std::size_t offset = 0; offset < 3; ++offset) {
        B b;
        if constexpr (N > 0) {
            for (std::size_t pos = offset; pos < N; pos += 3) {
                b.set(pos);
            }
        }
        inputs.push_back(b);
    }

    for (auto const& b : inputs) {
        std::vector<std::size_t> expected;
        if constexpr (N > 0) {
            for (std::size_t pos = 0; pos < N; ++pos) {
                if (b[pos]) expected.push_back(pos);
            }
        }

        auto const pv = b.positions();
        CHECK(std::ranges::equal(pv, expected));
        CHECK(std::ranges::equal(pv | std::views::reverse, expected | std::views::reverse));
        CHECK(pv.size() == expected.size());
        CHECK(B::from_positions(pv) == b);

        for (auto it = pv.begin(); it != pv.end(); ++it) {
            auto j = std::next(it);
            REQUIRE(std::prev(j) == it);
        }
    }
}

#undef IRIS_TEST_BITSET_POSITIONS_CFGS

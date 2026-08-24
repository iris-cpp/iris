#include "iris_test.hpp"

#include <iris/string.hpp>
#include <iris/string_algo.hpp>

#include <format>
#include <string>
#include <string_view>

#ifdef _MSC_VER
# include <Windows.h>
#endif

using namespace std::string_view_literals;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("string: type traits")
{
    STATIC_CHECK(iris::CharLike<char>);
    STATIC_CHECK(iris::CharLike<wchar_t>);
    STATIC_CHECK(iris::CharLike<char8_t>);
    STATIC_CHECK(iris::CharLike<char16_t>);
    STATIC_CHECK(iris::CharLike<char32_t>);

    STATIC_CHECK(iris::CharLike<char const>);
    STATIC_CHECK(iris::CharLike<char volatile>);
    STATIC_CHECK(iris::CharLike<char const volatile>);
    STATIC_CHECK(!iris::CharLike<char const&>);
    STATIC_CHECK(!iris::CharLike<char&>);

    STATIC_CHECK(std::same_as<iris::char_type_for<char>, char>);
    STATIC_CHECK(std::same_as<iris::char_type_for<char const>, char>);
    STATIC_CHECK(std::same_as<iris::char_type_for<char volatile>, char>);
    STATIC_CHECK(std::same_as<iris::char_type_for<char const volatile>, char>);

    STATIC_CHECK(std::same_as<iris::char_type_for<char*>, char>);
    STATIC_CHECK(std::same_as<iris::char_type_for<char const*>, char>);

#define IRIS_CHECK_STRING_LIKE(CharT, type) \
    STATIC_CHECK(iris::StringLike<type>); \
    STATIC_CHECK(std::same_as<iris::char_type_for<type>, CharT>);

    IRIS_CHECK_STRING_LIKE(char, char const*);
    IRIS_CHECK_STRING_LIKE(char, char const* const);
    IRIS_CHECK_STRING_LIKE(char, char const*&);
    IRIS_CHECK_STRING_LIKE(char, char const* const&);
    IRIS_CHECK_STRING_LIKE(char, char const*&&);
    IRIS_CHECK_STRING_LIKE(char, char const* const&&);

    IRIS_CHECK_STRING_LIKE(char, std::string_view);
    IRIS_CHECK_STRING_LIKE(char, std::string_view const);
    IRIS_CHECK_STRING_LIKE(char, std::string_view&);
    IRIS_CHECK_STRING_LIKE(char, std::string_view const&);
    IRIS_CHECK_STRING_LIKE(char, std::string_view&&);
    IRIS_CHECK_STRING_LIKE(char, std::string_view const&&);

    IRIS_CHECK_STRING_LIKE(char, std::string);
    IRIS_CHECK_STRING_LIKE(char, std::string const);
    IRIS_CHECK_STRING_LIKE(char, std::string&);
    IRIS_CHECK_STRING_LIKE(char, std::string const&);
    IRIS_CHECK_STRING_LIKE(char, std::string&&);
    IRIS_CHECK_STRING_LIKE(char, std::string const&&);

    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const*);
    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const* const);
    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const*&);
    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const* const&);
    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const*&&);
    IRIS_CHECK_STRING_LIKE(char32_t, char32_t const* const&&);

    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view const);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view const&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view&&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string_view const&&);

    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string const);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string const&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string&&);
    IRIS_CHECK_STRING_LIKE(char32_t, std::u32string const&&);

#undef IRIS_CHECK_STRING_LIKE
}

TEST_CASE("string_algo: trim")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::string str;
        iris::trim_edges(str, "");
        CHECK(str == ""sv);
    }

    CHECK(iris::trim_edges_copy("")   == ""sv);
    CHECK(iris::trim_edges_copy(" ")  == ""sv);
    CHECK(iris::trim_edges_copy("\t") == ""sv);
    CHECK(iris::trim_edges_copy("\r") == ""sv);
    CHECK(iris::trim_edges_copy("\n") == ""sv);

    CHECK(iris::trim_edges_copy(U"")   == U""sv);
    CHECK(iris::trim_edges_copy(U" ")  == U""sv);
    CHECK(iris::trim_edges_copy(U"　") == U""sv); // Japanese space
    CHECK(iris::trim_edges_copy(U"\t") == U""sv);
    CHECK(iris::trim_edges_copy(U"\r") == U""sv);
    CHECK(iris::trim_edges_copy(U"\n") == U""sv);

    CHECK(iris::trim_edges_copy(" a")  == "a"sv);
    CHECK(iris::trim_edges_copy("a ")  == "a"sv);
    CHECK(iris::trim_edges_copy(" a ") == "a"sv);

    CHECK(iris::trim_edges_copy(U" a")  == U"a"sv);
    CHECK(iris::trim_edges_copy(U"a ")  == U"a"sv);
    CHECK(iris::trim_edges_copy(U" a ") == U"a"sv);

    CHECK(iris::trim_edges_copy(" a b")  == "a b"sv);
    CHECK(iris::trim_edges_copy("a b ")  == "a b"sv);
    CHECK(iris::trim_edges_copy(" a b ") == "a b"sv);

    CHECK(iris::trim_edges_copy(U" a b")  == U"a b"sv);
    CHECK(iris::trim_edges_copy(U"a b ")  == U"a b"sv);
    CHECK(iris::trim_edges_copy(U" a b ") == U"a b"sv);
}

TEST_CASE("string_algo: normalize")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::string str;
        iris::normalize_spaces(str, "");
        CHECK(str == ""sv);
    }

    CHECK(iris::normalize_spaces_copy("")   == ""sv);
    CHECK(iris::normalize_spaces_copy(" ")  == " "sv);
    CHECK(iris::normalize_spaces_copy("\t") == " "sv);
    CHECK(iris::normalize_spaces_copy("\r") == " "sv);
    CHECK(iris::normalize_spaces_copy("\n") == " "sv);

    CHECK(iris::normalize_spaces_copy(U"")   == U""sv);
    CHECK(iris::normalize_spaces_copy(U" ")  == U" "sv);
    CHECK(iris::normalize_spaces_copy(U"　")  == U" "sv); // Japanese space
    CHECK(iris::normalize_spaces_copy(U"\t") == U" "sv);
    CHECK(iris::normalize_spaces_copy(U"\r") == U" "sv);
    CHECK(iris::normalize_spaces_copy(U"\n") == U" "sv);
}

TEST_CASE("string_algo: compact")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::string str;
        iris::compact_spaces(str, "");
        CHECK(str == ""sv);
    }

    CHECK(iris::compact_spaces_copy("")   == ""sv);
    CHECK(iris::compact_spaces_copy(" ")  == ""sv);
    CHECK(iris::compact_spaces_copy("\t") == ""sv);
    CHECK(iris::compact_spaces_copy("\r") == ""sv);
    CHECK(iris::compact_spaces_copy("\n") == ""sv);

    CHECK(iris::compact_spaces_copy(U"")   == U""sv);
    CHECK(iris::compact_spaces_copy(U" ")  == U""sv);
    CHECK(iris::compact_spaces_copy(U"　")  == U""sv); // Japanese space
    CHECK(iris::compact_spaces_copy(U"\t") == U""sv);
    CHECK(iris::compact_spaces_copy(U"\r") == U""sv);
    CHECK(iris::compact_spaces_copy(U"\n") == U""sv);

    // ------------------------------------------
    // single spaces

    CHECK(iris::compact_spaces_copy(" a")  == "a"sv);
    CHECK(iris::compact_spaces_copy("a ")  == "a"sv);
    CHECK(iris::compact_spaces_copy(" a ") == "a"sv);

    CHECK(iris::compact_spaces_copy(U" a")  == U"a"sv);
    CHECK(iris::compact_spaces_copy(U"a ")  == U"a"sv);
    CHECK(iris::compact_spaces_copy(U" a ") == U"a"sv);

    CHECK(iris::compact_spaces_copy(" a b")  == "a b"sv);
    CHECK(iris::compact_spaces_copy("a b ")  == "a b"sv);
    CHECK(iris::compact_spaces_copy(" a b ") == "a b"sv);

    CHECK(iris::compact_spaces_copy(U" a b")  == U"a b"sv);
    CHECK(iris::compact_spaces_copy(U"a b ")  == U"a b"sv);
    CHECK(iris::compact_spaces_copy(U" a b ") == U"a b"sv);

    // ------------------------------------------
    // multiple spaces

    CHECK(iris::compact_spaces_copy("  a")  == "a"sv);
    CHECK(iris::compact_spaces_copy("a  ")  == "a"sv);
    CHECK(iris::compact_spaces_copy("  a  ") == "a"sv);

    CHECK(iris::compact_spaces_copy(U"  a")  == U"a"sv);
    CHECK(iris::compact_spaces_copy(U"a  ")  == U"a"sv);
    CHECK(iris::compact_spaces_copy(U"  a  ") == U"a"sv);

    CHECK(iris::compact_spaces_copy("  a  b")  == "a b"sv);
    CHECK(iris::compact_spaces_copy("a  b  ")  == "a b"sv);
    CHECK(iris::compact_spaces_copy("  a  b  ") == "a b"sv);

    CHECK(iris::compact_spaces_copy(U"  a  b")  == U"a b"sv);
    CHECK(iris::compact_spaces_copy(U"a  b  ")  == U"a b"sv);
    CHECK(iris::compact_spaces_copy(U"  a  b  ") == U"a b"sv);
}

TEST_CASE("string_algo: escape")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::string str;
        iris::escape(str, '!', "");
        CHECK(str == ""sv);
    }

#define IRIS_TEST_ESCAPE(str, leader, escape_targets, expected) \
    do { \
        auto const escaped_str = iris::escape_copy(str, leader, escape_targets); \
        CHECK(escaped_str == expected ## sv); \
        if (escaped_str == expected ## sv) { \
            CHECK(iris::unescape_copy(escaped_str, leader) == str ## sv); \
        } \
    } while (false)

    IRIS_TEST_ESCAPE("", '!', {}, "");
    IRIS_TEST_ESCAPE("a", '!', {}, "a");
    IRIS_TEST_ESCAPE("!", '!', {}, "!!");
    IRIS_TEST_ESCAPE("!a", '!', {}, "!!a");
    IRIS_TEST_ESCAPE("a!", '!', {}, "a!!");
    IRIS_TEST_ESCAPE("a!a", '!', {}, "a!!a");

    IRIS_TEST_ESCAPE("", '!', "/", "");
    IRIS_TEST_ESCAPE("a", '!', "/", "a");
    IRIS_TEST_ESCAPE("!", '!', "/", "!!");
    IRIS_TEST_ESCAPE("!a", '!', "/", "!!a");
    IRIS_TEST_ESCAPE("a!", '!', "/", "a!!");
    IRIS_TEST_ESCAPE("a!a", '!', "/", "a!!a");

    IRIS_TEST_ESCAPE("/", '!', "/", "!/");

    IRIS_TEST_ESCAPE("/a", '!', "/", "!/a");
    IRIS_TEST_ESCAPE("a/", '!', "/", "a!/");

    IRIS_TEST_ESCAPE("/!", '!', "/", "!/!!");
    IRIS_TEST_ESCAPE("!/", '!', "/", "!!!/");

    IRIS_TEST_ESCAPE("/!a", '!', "/", "!/!!a");
    IRIS_TEST_ESCAPE("!/a", '!', "/", "!!!/a");
    IRIS_TEST_ESCAPE("!a/", '!', "/", "!!a!/");

    IRIS_TEST_ESCAPE("/a!", '!', "/", "!/a!!");
    IRIS_TEST_ESCAPE("a/!", '!', "/", "a!/!!");
    IRIS_TEST_ESCAPE("a!/", '!', "/", "a!!!/");

    IRIS_TEST_ESCAPE("/a!a", '!', "/", "!/a!!a");
    IRIS_TEST_ESCAPE("a/!a", '!', "/", "a!/!!a");
    IRIS_TEST_ESCAPE("a!/a", '!', "/", "a!!!/a");
    IRIS_TEST_ESCAPE("a!a/", '!', "/", "a!!a!/");

#undef IRIS_TEST_ESCAPE
}


TEST_CASE("string_algo: abbreviate")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::string str;
        iris::abbreviate(str, 3, "...");
        CHECK(str == ""sv);
    }

    CHECK_THROWS_AS(iris::abbreviate_copy("", 2, "..."), std::length_error);
    CHECK_THROWS_AS(iris::abbreviate_copy("aaaaaa", 2, "..."), std::length_error);

    CHECK(iris::abbreviate_copy("fo", 3, "...") == "fo"sv);
    CHECK(iris::abbreviate_copy("foo", 3, "...") == "foo"sv);
    CHECK(iris::abbreviate_copy("foob", 3, "...") == "..."sv);

    CHECK(iris::abbreviate_copy("foo", 4, "...") == "foo"sv);
    CHECK(iris::abbreviate_copy("foob", 4, "...") == "foob"sv);
    CHECK(iris::abbreviate_copy("fooba", 4, "...") == "f..."sv);
}

TEST_CASE("string_algo: ordinary_normalize")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        std::u32string str;
        iris::ordinary_normalize(str);
        CHECK(str == U""sv);
    }

    CHECK(iris::ordinary_normalize_copy(U"") == U""sv);

#if 0
let edge_chars = [['ａ', 'ｚ'], ['Ａ', 'Ｚ'], ['０', '９']];
for (let char_range of edge_chars) {
    console.log(`[${char_range[0].codePointAt(0).toString(16)}, ${char_range[1].codePointAt(0).toString(16)}]`);
}
// [ff41, ff5a]
// [ff21, ff3a]
// [ff10, ff19]
#endif

    CHECK(iris::ordinary_normalize_copy(U" ") == U" "sv);
    CHECK(iris::ordinary_normalize_copy(U"　") == U" "sv);

    CHECK(iris::ordinary_normalize_copy(U"a") == U"a"sv);
    CHECK(iris::ordinary_normalize_copy(U"z") == U"z"sv);
    CHECK(iris::ordinary_normalize_copy(U"A") == U"A"sv);
    CHECK(iris::ordinary_normalize_copy(U"Z") == U"Z"sv);
    CHECK(iris::ordinary_normalize_copy(U"0") == U"0"sv);
    CHECK(iris::ordinary_normalize_copy(U"9") == U"9"sv);

    CHECK(iris::ordinary_normalize_copy(U"ａ") == U"a"sv);
    CHECK(iris::ordinary_normalize_copy(U"ｚ") == U"z"sv);
    CHECK(iris::ordinary_normalize_copy(U"\uff5b") == U"\uff5b"sv);

    CHECK(iris::ordinary_normalize_copy(U"Ａ") == U"A"sv);
    CHECK(iris::ordinary_normalize_copy(U"Ｚ") == U"Z"sv);
    CHECK(iris::ordinary_normalize_copy(U"\uff3b") == U"\uff3b"sv);

    CHECK(iris::ordinary_normalize_copy(U"０") == U"0"sv);
    CHECK(iris::ordinary_normalize_copy(U"９") == U"9"sv);
    CHECK(iris::ordinary_normalize_copy(U"\uff20") == U"\uff20"sv);

    {
        std::u32string_view const input =
            U"ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ／"
            U"ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ／"
            U"０１２３４５６７８９";

        std::u32string_view const expected_normalized =
            U"abcdefghijklmnopqrstuvwxyz／"
            U"ABCDEFGHIJKLMNOPQRSTUVWXYZ／"
            U"0123456789";

        CHECK(iris::ordinary_normalize_copy(input) == expected_normalized);
        CHECK(iris::ordinary_normalize_copy(input).size() == input.size());
    }
}

// NOLINTEND(readability-container-size-empty)

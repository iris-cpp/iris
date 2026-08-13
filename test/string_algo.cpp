#include "iris_test.hpp"

#include <iris/string_algo.hpp>

#include <format>
#include <string>
#include <string_view>

#ifdef _MSC_VER
# include <Windows.h>
#endif

using namespace std::string_view_literals;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("string_algo: trim")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

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

// NOLINTEND(readability-container-size-empty)

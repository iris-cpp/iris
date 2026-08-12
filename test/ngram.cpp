// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/ngram.hpp>

#include <format>
#include <iostream>
#include <ranges>

#ifdef _MSC_VER
# include <Windows.h>
#endif

using namespace iris::ngram_literals;
using iris::ngram_occurrence;
using iris::interval;

[[nodiscard]]
constexpr auto make_occurrences(std::initializer_list<ngram_occurrence> occs)
{
    return std::vector<ngram_occurrence>{occs};
}

#define IRIS_CHECK_NO_OCCURRENCE(ng_str) do { \
        std::vector<ngram_occurrence> occs; \
        ngram_db.find_occurrences(iris::to_ngram(U ## ng_str), occs); \
        CHECK(occs.empty()); \
    } while (false)

#define IRIS_CHECK_OCCURRENCE(ng_str, ...) do { \
        std::vector<ngram_occurrence> occs; \
        ngram_db.find_occurrences(iris::to_ngram(U ## ng_str), occs); \
        CHECK(occs == make_occurrences({__VA_ARGS__})); \
    } while (false)

TEST_CASE("ngram (minimal input)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"");
        IRIS_CHECK_NO_OCCURRENCE("a");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"a");
        IRIS_CHECK_OCCURRENCE("a", {0_doc_id, 0});
        IRIS_CHECK_NO_OCCURRENCE("X");
        IRIS_CHECK_NO_OCCURRENCE("XX");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab");
        IRIS_CHECK_OCCURRENCE("a", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("b", {0_doc_id, 1});
        IRIS_CHECK_NO_OCCURRENCE("X");
        IRIS_CHECK_OCCURRENCE("ab", {0_doc_id, 0});
        IRIS_CHECK_NO_OCCURRENCE("XX");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abc");
        IRIS_CHECK_OCCURRENCE("a", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("b", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("c", {0_doc_id, 2});
        IRIS_CHECK_NO_OCCURRENCE("X");
        IRIS_CHECK_OCCURRENCE("ab", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("bc", {0_doc_id, 1});
        IRIS_CHECK_NO_OCCURRENCE("XX");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abcd");
        IRIS_CHECK_OCCURRENCE("a", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("b", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("c", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("d", {0_doc_id, 3});
        IRIS_CHECK_NO_OCCURRENCE("X");
        IRIS_CHECK_OCCURRENCE("ab", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("bc", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("cd", {0_doc_id, 2});
        IRIS_CHECK_NO_OCCURRENCE("XX");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abcde");
        IRIS_CHECK_OCCURRENCE("a", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("b", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("c", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("d", {0_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("e", {0_doc_id, 4});
        IRIS_CHECK_NO_OCCURRENCE("X");
        IRIS_CHECK_OCCURRENCE("ab", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("bc", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("cd", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("de", {0_doc_id, 3});
        IRIS_CHECK_NO_OCCURRENCE("XX");
    }
}

TEST_CASE("ngram (realistic input)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    // https://gihyo.jp/dev/serial/01/make-findspot/0005

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今日は良い天気です。");

        IRIS_CHECK_OCCURRENCE("今日", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("日は", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("は良", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("良い", {0_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("い天", {0_doc_id, 4});
        IRIS_CHECK_OCCURRENCE("天気", {0_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("気で", {0_doc_id, 6});
        IRIS_CHECK_OCCURRENCE("です", {0_doc_id, 7});
        IRIS_CHECK_OCCURRENCE("す。", {0_doc_id, 8});
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今日は大雨です。");

        IRIS_CHECK_OCCURRENCE("今日", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("日は", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("は大", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("大雨", {0_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("雨で", {0_doc_id, 4});
        IRIS_CHECK_OCCURRENCE("です", {0_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("す。", {0_doc_id, 6});
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今日の東海地方は大雨でしょう。");

        IRIS_CHECK_OCCURRENCE("今日", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("日の", {0_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("の東", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("東海", {0_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("海地", {0_doc_id, 4});
        IRIS_CHECK_OCCURRENCE("地方", {0_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("方は", {0_doc_id, 6});
        IRIS_CHECK_OCCURRENCE("は大", {0_doc_id, 7});
        IRIS_CHECK_OCCURRENCE("大雨", {0_doc_id, 8});
        IRIS_CHECK_OCCURRENCE("雨で", {0_doc_id, 9});
        IRIS_CHECK_OCCURRENCE("でし", {0_doc_id, 10});
        IRIS_CHECK_OCCURRENCE("しょ", {0_doc_id, 11});
        IRIS_CHECK_OCCURRENCE("ょう", {0_doc_id, 12});
        IRIS_CHECK_OCCURRENCE("う。", {0_doc_id, 13});
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今日は良い天気です。");
        (void)ngram_db.add_document(U"今日は大雨です。");
        (void)ngram_db.add_document(U"今日の東海地方は大雨でしょう。");

        IRIS_CHECK_OCCURRENCE("今日", {0_doc_id, 0}, {1_doc_id, 0}, {2_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("日は", {0_doc_id, 1}, {1_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("は良", {0_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("良い", {0_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("い天", {0_doc_id, 4});
        IRIS_CHECK_OCCURRENCE("天気", {0_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("気で", {0_doc_id, 6});
        IRIS_CHECK_OCCURRENCE("です", {0_doc_id, 7}, {1_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("す。", {0_doc_id, 8}, {1_doc_id, 6});
        IRIS_CHECK_OCCURRENCE("は大", {1_doc_id, 2}, {2_doc_id, 7});
        IRIS_CHECK_OCCURRENCE("大雨", {1_doc_id, 3}, {2_doc_id, 8});
        IRIS_CHECK_OCCURRENCE("雨で", {1_doc_id, 4}, {2_doc_id, 9});
        IRIS_CHECK_OCCURRENCE("日の", {2_doc_id, 1});
        IRIS_CHECK_OCCURRENCE("の東", {2_doc_id, 2});
        IRIS_CHECK_OCCURRENCE("東海", {2_doc_id, 3});
        IRIS_CHECK_OCCURRENCE("海地", {2_doc_id, 4});
        IRIS_CHECK_OCCURRENCE("地方", {2_doc_id, 5});
        IRIS_CHECK_OCCURRENCE("方は", {2_doc_id, 6});
        IRIS_CHECK_OCCURRENCE("でし", {2_doc_id, 10});
        IRIS_CHECK_OCCURRENCE("しょ", {2_doc_id, 11});
        IRIS_CHECK_OCCURRENCE("ょう", {2_doc_id, 12});
        IRIS_CHECK_OCCURRENCE("う。", {2_doc_id, 13});
    }
}

struct DocumentMatch
{
    iris::ngram_document_id doc_id;
    std::vector<iris::ngram_search_word_match> word_matches;

    DocumentMatch(iris::ngram_document_id doc_id, std::initializer_list<iris::ngram_search_word_match> word_matches)
        : doc_id(doc_id)
        , word_matches(word_matches)
    {}

    DocumentMatch(iris::ngram_document_id doc_id, std::vector<iris::ngram_search_word_match> word_matches)
        : doc_id(doc_id)
        , word_matches(std::move(word_matches))
    {}

    [[nodiscard]]
    bool operator==(DocumentMatch const&) const noexcept = default;
};

template<class CharT>
struct std::formatter<DocumentMatch, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(DocumentMatch const& doc_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "(doc: #{}, word_matches: {})", doc_match.doc_id, doc_match.word_matches);
    }
};

#define IRIS_CHECK_SEARCH(query_input, ...) do { \
        iris::ngram_search_query const query{U ## query_input}; \
        auto const search_res = ngram_db.search(query); \
        auto const& doc_matches = search_res.doc_matches(); \
        \
        std::vector<DocumentMatch> const expected_doc_matches{ \
            std::initializer_list<DocumentMatch>{__VA_ARGS__} \
        }; \
        \
        auto const actual_doc_matches = doc_matches | std::views::transform([](auto const& kv) { \
            return DocumentMatch{kv.first, kv.second}; \
        }) | std::ranges::to<std::vector>(); \
        CHECK(actual_doc_matches == expected_doc_matches); \
    } while (false)

TEST_CASE("ngram search (document chars = 0)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        IRIS_CHECK_SEARCH("");
        IRIS_CHECK_SEARCH("X");
        IRIS_CHECK_SEARCH("XX");
    }
}

// 1-gram document
TEST_CASE("ngram search (document chars = 1)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"a");
        IRIS_CHECK_SEARCH("");
        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");
        IRIS_CHECK_SEARCH("XX");
    }
}

// 2-gram document
TEST_CASE("ngram search (document chars = 2)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"aa");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{1, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "aa",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH("aaX");
        IRIS_CHECK_SEARCH("Xaa");
        IRIS_CHECK_SEARCH("XXX");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "b",
            {0_doc_id, {
                {0, {interval{1, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xab");
        IRIS_CHECK_SEARCH("XXX");
    }
}

// 2-gram + 1-gram document
TEST_CASE("ngram search (document chars = 3, aaa/baa)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"aaa");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{1, 2}, interval{2, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "aa",
            {0_doc_id, {
                {0, {interval{0, 2}, interval{1, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "aaa",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("aaX");
        IRIS_CHECK_SEARCH("Xaa");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH("aaaX");
        IRIS_CHECK_SEARCH("Xaaa");
        IRIS_CHECK_SEARCH("XXaa");
        IRIS_CHECK_SEARCH("aaXX");
        IRIS_CHECK_SEARCH("XXXX");
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"baa");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{1, 2}, interval{2, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "b",
            {0_doc_id, {
                {0, {interval{0, 1}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "aa",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ba",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "baa",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("aaX");
        IRIS_CHECK_SEARCH("Xaa");
        IRIS_CHECK_SEARCH("baX");
        IRIS_CHECK_SEARCH("Xba");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH("baaX");
        IRIS_CHECK_SEARCH("Xbaa");
        IRIS_CHECK_SEARCH("baXX");
        IRIS_CHECK_SEARCH("XXba");
        IRIS_CHECK_SEARCH("aaXX");
        IRIS_CHECK_SEARCH("XXaa");
        IRIS_CHECK_SEARCH("XXXX");
    }
}

// 2-gram + 1-gram document
TEST_CASE("ngram search (document chars = 3, aba/aab)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"aba");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{2, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "b",
            {0_doc_id, {
                {0, {interval{1, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ba",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "aba",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xab");
        IRIS_CHECK_SEARCH("baX");
        IRIS_CHECK_SEARCH("Xba");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH("abaX");
        IRIS_CHECK_SEARCH("Xaba");
        IRIS_CHECK_SEARCH("XXab");
        IRIS_CHECK_SEARCH("abXX");
        IRIS_CHECK_SEARCH("XXba");
        IRIS_CHECK_SEARCH("baXX");
        IRIS_CHECK_SEARCH("XXXX");
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"aab");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{1, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "b",
            {0_doc_id, {
                {0, {interval{2, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "aa",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "aab",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("aaX");
        IRIS_CHECK_SEARCH("Xaa");
        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xab");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH("aabX");
        IRIS_CHECK_SEARCH("Xaab");
        IRIS_CHECK_SEARCH("XXaa");
        IRIS_CHECK_SEARCH("aaXX");
        IRIS_CHECK_SEARCH("XXab");
        IRIS_CHECK_SEARCH("abXX");
        IRIS_CHECK_SEARCH("XXXX");
    }
}

// 2-gram + 1-gram document
TEST_CASE("ngram search (document chars = 3, abc)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abc");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "b",
            {0_doc_id, {
                {0, {interval{1, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "c",
            {0_doc_id, {
                {0, {interval{2, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "bc",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xab");
        IRIS_CHECK_SEARCH("bcX");
        IRIS_CHECK_SEARCH("Xbc");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH("abcX");
        IRIS_CHECK_SEARCH("Xabc");
        IRIS_CHECK_SEARCH("XXab");
        IRIS_CHECK_SEARCH("abXX");
        IRIS_CHECK_SEARCH("XXbc");
        IRIS_CHECK_SEARCH("bcXX");
        IRIS_CHECK_SEARCH("XXXX");
    }
}

TEST_CASE("ngram search (document chars = 4)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    // TODO

    // 2x 2-gram document
    //{
    //    iris::ngram_database<> ngram_db;
    //    (void)ngram_db.add_document(U"aaaa");
    //    IRIS_CHECK_SEARCH("");

    //    IRIS_CHECK_SEARCH(
    //        "a",
    //        {0_doc_id, {
    //            {0, {interval{0, 1}, interval{1, 2}, interval{2, 3}, interval{3, 4}}},
    //        }},
    //    );
    //    IRIS_CHECK_SEARCH("X");

    //    IRIS_CHECK_SEARCH(
    //        "aa",
    //        {0_doc_id, {
    //            {0, {interval{0, 2}, interval{2, 4}}},
    //        }},
    //    );
    //    IRIS_CHECK_SEARCH("XX");
    //}

}

TEST_CASE("ngram search (minimal input, dependency on previous match)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXXef");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXXefef");
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abef");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abefef");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abXXef");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abXXefef");
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXef");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXefef");
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"abxcd"); // trap document
        (void)ngram_db.add_document(U"abcd");
        IRIS_CHECK_SEARCH(
            "abcd",
            {1_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
    }

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"ab");
        (void)ngram_db.add_document(U"abcd");
        IRIS_CHECK_SEARCH(
            "ab cd",
            {1_doc_id, {
                {0, {interval{0, 2}}},
                {1, {interval{2, 4}}},
            }},
        );
    }
}

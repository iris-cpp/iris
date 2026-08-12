// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

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

TEST_CASE("ngram (type traits)")
{
    STATIC_CHECK(std::same_as<decltype(iris::ngram<1, char>::data), char>);
    STATIC_CHECK(std::same_as<decltype(iris::ngram<2, char>::data), std::uint16_t>);

    STATIC_CHECK(std::same_as<decltype(iris::ngram<1, char32_t>::data), char32_t>);
    STATIC_CHECK(std::same_as<decltype(iris::ngram<2, char32_t>::data), std::uint64_t>);
}

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

#include "iris_test.hpp"

#include <iris/ngram.hpp>

#include <format>
#include <iostream>
#include <ranges>

#ifdef _MSC_VER
# include <Windows.h>
#endif

namespace iris {

inline std::ostream& operator<<(std::ostream& os, iris::ngram_occurrence const& occ)
{
    return os << std::format("{}:{}", occ.doc_id, occ.pos);
}

} // iris

// --------------------------------------------------

using namespace iris::ngram_literals;
using iris::ngram_occurrence;

[[nodiscard]]
constexpr auto make_occurrences(std::initializer_list<ngram_occurrence> occs)
{
    return std::vector<ngram_occurrence>{occs};
}

#define IRIS_CHECK_NO_OCCURRENCE(ng_str) do { \
        CHECK(!ngram_db.get_occurrences(iris::to_ngram(U ## ng_str))); \
    } while (false);

#define IRIS_CHECK_OCCURRENCE(ng_str, ...) do { \
        std::vector<ngram_occurrence> const* occs = nullptr; \
        CHECK((occs = ngram_db.get_occurrences(iris::to_ngram(U ## ng_str)))); \
        if (occs) { \
            CHECK(*occs == make_occurrences({__VA_ARGS__})); \
        } \
    } while (false);

TEST_CASE("ngram (minimal input)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"");
        IRIS_CHECK_NO_OCCURRENCE("今");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今");
        IRIS_CHECK_OCCURRENCE("今", {0_doc_id, 0});
        IRIS_CHECK_NO_OCCURRENCE("無");
        IRIS_CHECK_NO_OCCURRENCE("今日");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今");
        IRIS_CHECK_OCCURRENCE("今", {0_doc_id, 0});
        IRIS_CHECK_NO_OCCURRENCE("無");
        IRIS_CHECK_NO_OCCURRENCE("今日");
    }
    {
        iris::ngram_database<> ngram_db;
        (void)ngram_db.add_document(U"今日");
        IRIS_CHECK_OCCURRENCE("今", {0_doc_id, 0});
        IRIS_CHECK_OCCURRENCE("日", {0_doc_id, 1});
        IRIS_CHECK_NO_OCCURRENCE("無");
        IRIS_CHECK_OCCURRENCE("今日", {0_doc_id, 0});
        IRIS_CHECK_NO_OCCURRENCE("今無");
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

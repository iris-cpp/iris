// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

#include <iris/ngram/keyed_database.hpp>

#include <iris/hash/string_like_hash.hpp>

#include <string>
#include <string_view>

TEST_CASE("ngram: type traits")
{
    STATIC_CHECK(sizeof(iris::ngram::gram<1, char>) == 1);
    STATIC_CHECK(sizeof(iris::ngram::gram<2, char>) == 2);
    STATIC_CHECK(sizeof(iris::ngram::gram<3, char>) == 3);
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<1, char>>);
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<2, char>>);
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<3, char>>);

    STATIC_CHECK(sizeof(iris::ngram::gram<1, char32_t>) == 4);
    STATIC_CHECK(sizeof(iris::ngram::gram<2, char32_t>) == 8);
    STATIC_CHECK(sizeof(iris::ngram::gram<3, char32_t>) == 12); // TODO: optimize
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<1, char32_t>>);
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<2, char32_t>>);
    STATIC_CHECK(std::is_trivially_copyable_v<iris::ngram::gram<3, char32_t>>);
}

TEST_CASE("ngram: update document")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;

        auto const doc_id = ngram_db.add_document(U"abc");
        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
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

        ngram_db.set_visible(doc_id, false);
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH("ab");
        IRIS_CHECK_SEARCH("bc");

        ngram_db.set_visible(doc_id, true);
        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
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

        ngram_db.remove_document(doc_id);
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH("ab");
        IRIS_CHECK_SEARCH("bc");
    }

    {
        iris::ngram::database<> ngram_db;

        auto const doc_id = ngram_db.add_document(U"abc");
        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
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

        ngram_db.update_document(doc_id, U"abd");
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH(
            "abd",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH("bc");
        IRIS_CHECK_SEARCH(
            "bd",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );

        ngram_db.remove_document(doc_id);
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH("ab");
        IRIS_CHECK_SEARCH("bc");
        IRIS_CHECK_SEARCH("abd");
        IRIS_CHECK_SEARCH("bd");
    }
}

TEST_CASE("ngram: keyed_database")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::keyed_database<std::string, iris::string_like_hash>
        ngram_db;

        ngram_db.add_document("doc0", U"abc");
        IRIS_CHECK_SEARCH(
            "abc",
            {"doc0", {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ab",
            {"doc0", {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "bc",
            {"doc0", {
                {0, {interval{1, 3}}},
            }},
        );

        ngram_db.set_visible("doc0", false);
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH("ab");
        IRIS_CHECK_SEARCH("bc");

        ngram_db.set_visible("doc0", true);
        IRIS_CHECK_SEARCH(
            "abc",
            {"doc0", {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "ab",
            {"doc0", {
                {0, {interval{0, 2}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "bc",
            {"doc0", {
                {0, {interval{1, 3}}},
            }},
        );

        ngram_db.remove_document("doc0");
        IRIS_CHECK_SEARCH("abc");
        IRIS_CHECK_SEARCH("ab");
        IRIS_CHECK_SEARCH("bc");
    }
}

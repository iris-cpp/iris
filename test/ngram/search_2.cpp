// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

TEST_CASE("gram search (document chars = 0)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        IRIS_CHECK_SEARCH("");
        IRIS_CHECK_SEARCH("X");
        IRIS_CHECK_SEARCH("XX");
    }
}

// 1-gram document
TEST_CASE("gram search (document chars = 1)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
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
TEST_CASE("gram search (document chars = 2)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
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
        iris::ngram::database<> ngram_db;
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

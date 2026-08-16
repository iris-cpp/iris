// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

// 2-gram + 1-gram document
TEST_CASE("gram search (document chars = 3, aaa/baa)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
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
        iris::ngram::database<> ngram_db;
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
TEST_CASE("gram search (document chars = 3, aba/aab)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
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
        iris::ngram::database<> ngram_db;
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
TEST_CASE("gram search (document chars = 3, abc)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
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

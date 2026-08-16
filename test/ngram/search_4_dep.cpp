// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

// 2x 2-gram document
TEST_CASE("gram search (document chars = 4, aaaa)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"aaaa");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{1, 2}, interval{2, 3}, interval{3, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "aa",
            {0_doc_id, {
                {0, {interval{0, 2}, interval{1, 3}, interval{2, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "aaa",
            {0_doc_id, {
                {0, {interval{0, 3}, interval{1, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH(
            "aaaa",
            {0_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("aaaX");
        IRIS_CHECK_SEARCH("Xaaa");
        IRIS_CHECK_SEARCH("XXXX");

        IRIS_CHECK_SEARCH("aaaaX");
        IRIS_CHECK_SEARCH("Xaaaa");
        IRIS_CHECK_SEARCH("XXaaa");
        IRIS_CHECK_SEARCH("aaaXX");
        IRIS_CHECK_SEARCH("XXXXX");
    }
}

// 2x 2-gram document
TEST_CASE("gram search (document chars = 4, abab)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abab");
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
                {0, {interval{1, 2}, interval{3, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("X");

        IRIS_CHECK_SEARCH(
            "ab",
            {0_doc_id, {
                {0, {interval{0, 2}, interval{2, 4}}},
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
        IRIS_CHECK_SEARCH(
            "bab",
            {0_doc_id, {
                {0, {interval{1, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xab");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH(
            "abab",
            {0_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("ababX");
        IRIS_CHECK_SEARCH("Xabab");
        IRIS_CHECK_SEARCH("XXXX");
    }
}

// 2x 2-gram document
TEST_CASE("gram search (document chars = 4, abca)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abca");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}, interval{3, 4}}},
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
        IRIS_CHECK_SEARCH(
            "ca",
            {0_doc_id, {
                {0, {interval{2, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "bca",
            {0_doc_id, {
                {0, {interval{1, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("abX");
        IRIS_CHECK_SEARCH("Xbc");
        IRIS_CHECK_SEARCH("caX");
        IRIS_CHECK_SEARCH("Xca");
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH(
            "abca",
            {0_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("abcaX");
        IRIS_CHECK_SEARCH("Xabca");
        IRIS_CHECK_SEARCH("caab"); // "ca" and "ab" both exist but not contiguous as "caab"
        IRIS_CHECK_SEARCH("XXXX");
    }
}

// 2x 2-gram document
TEST_CASE("gram search (document chars = 4, abcd)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abcd");
        IRIS_CHECK_SEARCH("");

        IRIS_CHECK_SEARCH(
            "a",
            {0_doc_id, {
                {0, {interval{0, 1}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "d",
            {0_doc_id, {
                {0, {interval{3, 4}}},
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
        IRIS_CHECK_SEARCH(
            "cd",
            {0_doc_id, {
                {0, {interval{2, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("XX");

        IRIS_CHECK_SEARCH(
            "abc",
            {0_doc_id, {
                {0, {interval{0, 3}}},
            }},
        );
        IRIS_CHECK_SEARCH(
            "bcd",
            {0_doc_id, {
                {0, {interval{1, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("XXX");

        IRIS_CHECK_SEARCH(
            "abcd",
            {0_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
        IRIS_CHECK_SEARCH("abcX");
        IRIS_CHECK_SEARCH("Xbcd");
        IRIS_CHECK_SEARCH("abcdX");
        IRIS_CHECK_SEARCH("Xabcd");
        IRIS_CHECK_SEARCH("abXcd"); // both halves exist; broken by X in the middle... but see note below!
        IRIS_CHECK_SEARCH("acbd"); // all chars exist; order scrambled
        IRIS_CHECK_SEARCH("XXXX");
    }
}

TEST_CASE("gram search (dependency on previous match)")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abcd");
        IRIS_CHECK_SEARCH("abXX");
    }

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXXef");
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXXefef");
    }

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abef");
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abefef");
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abXXef");
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ab..ef");
        IRIS_CHECK_SEARCH("abXXefef");
    }

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXef");
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abef");
        IRIS_CHECK_SEARCH("abXefef");
    }

    {
        iris::ngram::database<> ngram_db;
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
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abcd");
        (void)ngram_db.add_document(U"abxcd"); // trap document
        IRIS_CHECK_SEARCH(
            "abcd",
            {0_doc_id, {
                {0, {interval{0, 4}}},
            }},
        );
    }

    {
        iris::ngram::database<> ngram_db;
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

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abXcd");
        (void)ngram_db.add_document(U"abcdX");
        IRIS_CHECK_SEARCH("abcdc");
    }

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"abXXabcd");
        IRIS_CHECK_SEARCH(
            "abcd",
            {0_doc_id, {
                {0, {interval{4, 8}}},
            }},
        );
    }

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ab cdXf");
        (void)ngram_db.add_document(U"ab cdef");
        IRIS_CHECK_SEARCH(
            "ab cdef",
            {1_doc_id, {
                {0, {interval{0, 2}}},
                {1, {interval{3, 7}}},
            }},
        );
    }
}

// SPDX-License-Identifier: MIT

#include "ngram_test.hpp"

#include <iris/ngram/keyed_database.hpp>

#include <iris/hash/string_like_hash.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>

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

    STATIC_CHECK(std::ranges::random_access_range<iris::ngram::search_result<>>);
    STATIC_CHECK(std::ranges::sized_range<iris::ngram::search_result<>>);
    STATIC_CHECK(std::ranges::random_access_range<iris::ngram::search_result<>::word_matches>);
    STATIC_CHECK(std::ranges::sized_range<iris::ngram::search_result<>::word_matches>);
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

TEST_CASE("ngram: overlapping occurrences")
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
#endif

    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"そんな機会はなかなかない");
        IRIS_CHECK_SEARCH(
            "なかな",
            {0_doc_id, {
                {0, {interval{6, 9}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"すももも買った");
        IRIS_CHECK_SEARCH(
            "もも",
            {0_doc_id, {
                {0, {interval{1, 3}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"こここそ探していた場所だ");
        IRIS_CHECK_SEARCH(
            "ここ",
            {0_doc_id, {
                {0, {interval{0, 2}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ボボボーボ・ボーボボ");
        IRIS_CHECK_SEARCH(
            "ボボ",
            {0_doc_id, {
                {0, {interval{0, 2}, interval{8, 10}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"banana");
        IRIS_CHECK_SEARCH(
            "ana",
            {0_doc_id, {
                {0, {interval{1, 4}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"Mississippi");
        IRIS_CHECK_SEARCH(
            "issi",
            {0_doc_id, {
                {0, {interval{1, 5}}},
            }},
        );
    }
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"assesses");
        IRIS_CHECK_SEARCH(
            "sses",
            {0_doc_id, {
                {0, {interval{1, 5}}},
            }},
        );
    }

    // The occurrence after a skipped one may start right where the previous match ends
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"ははははと笑った");
        IRIS_CHECK_SEARCH(
            "はは",
            {0_doc_id, {
                {0, {interval{0, 2}, interval{2, 4}}},
            }},
        );
    }

    // Matches of different words may overlap each other
    {
        iris::ngram::database<> ngram_db;
        (void)ngram_db.add_document(U"すももも買った");
        IRIS_CHECK_SEARCH(
            "すもも もも",
            {0_doc_id, {
                {0, {interval{0, 3}}},
                {1, {interval{1, 3}}},
            }},
        );
    }
}

TEST_CASE("ngram: search word order")
{
    // "xyz" is examined first, but the result keeps the query order
    iris::ngram::database<> ngram_db;
    (void)ngram_db.add_document(U"abcd xyz abcd");
    (void)ngram_db.add_document(U"abcd");
    IRIS_CHECK_SEARCH(
        "abcd xyz",
        {0_doc_id, {
            {0, {interval{0, 4}, interval{9, 13}}},
            {1, {interval{5, 8}}},
        }},
    );

}

TEST_CASE("ngram: search n-gram order")
{
    // "cd" is examined first, and its first occurrence is too early for the word
    iris::ngram::database<> ngram_db;
    (void)ngram_db.add_document(U"cd abcd");
    (void)ngram_db.add_document(U"ab");
    IRIS_CHECK_SEARCH(
        "abcd",
        {0_doc_id, {
            {0, {interval{3, 7}}},
        }},
    );
}

TEST_CASE("ngram: search words of temporary strings")
{
    iris::ngram::database<> ngram_db;
    (void)ngram_db.add_document(U"今日は良い天気です。明日も良い天気でしょう。");

    std::vector<std::u32string> const words{U"良い天気", U"明日も"};
    auto const temporaries = words | std::views::transform([](std::u32string const& word) { return word; });

    iris::ngram::search_result<> search_res;
    CHECK(ngram_db.search(temporaries, search_res));
    auto const doc_matches = search_res | std::views::transform([](auto const& kv) {
        return DocumentMatch<>{kv.first, kv.second};
    }) | std::ranges::to<std::vector>();
    CHECK(doc_matches == std::vector<DocumentMatch<>>{
        {0_doc_id, {
            {0, {interval{3, 7}, interval{13, 17}}},
            {1, {interval{10, 13}}},
        }},
    });
}

TEST_CASE("ngram: keyed_database copy")
{
    iris::ngram::keyed_database<std::string, iris::string_like_hash> source;
    source.add_document("doc0", U"abc");
    source.add_document("doc1", U"bcd");
    source.remove_document("doc0");
    source.add_document("doc2", U"abcd");

    auto ngram_db = source;
    source.clear();
    IRIS_CHECK_SEARCH(
        "bc",
        {"doc1", {
            {0, {interval{0, 2}}},
        }},
        {"doc2", {
            {0, {interval{1, 3}}},
        }},
    );

    source.add_document("doc3", U"bc");
    ngram_db = source;
    source.clear();
    IRIS_CHECK_SEARCH(
        "bc",
        {"doc3", {
            {0, {interval{0, 2}}},
        }},
    );
}

TEST_CASE("ngram: compaction")
{
    // Enough updates to compact several times
    iris::ngram::database<> ngram_db;
    auto const updated = ngram_db.add_document(U"initial");
    auto const removed = ngram_db.add_document(U"removed text");
    auto const hidden = ngram_db.add_document(U"hidden text");
    auto const kept = ngram_db.add_document(U"kept text");
    ngram_db.remove_document(removed);
    ngram_db.set_visible(hidden, false);

    for (int i = 0; i < 10000; ++i) {
        ngram_db.update_document(updated, i % 2 == 0 ? U"even version" : U"odd version");
    }
    auto const added = ngram_db.add_document(U"added text");

    CHECK(ngram_db.has_document(updated));
    CHECK_FALSE(ngram_db.has_document(removed));
    CHECK(ngram_db.has_document(hidden));
    CHECK_FALSE(ngram_db.is_visible(hidden));
    CHECK(ngram_db.has_document(kept));
    CHECK(added == 4_doc_id);

    IRIS_CHECK_SEARCH(
        "version",
        {updated, {
            {0, {interval{4, 11}}},
        }},
    );
    IRIS_CHECK_SEARCH("even");
    IRIS_CHECK_SEARCH("initial");
    IRIS_CHECK_SEARCH(
        "text",
        {kept, {
            {0, {interval{5, 9}}},
        }},
        {added, {
            {0, {interval{6, 10}}},
        }},
    );

    ngram_db.set_visible(hidden, true);
    IRIS_CHECK_SEARCH(
        "text",
        {hidden, {
            {0, {interval{7, 11}}},
        }},
        {kept, {
            {0, {interval{5, 9}}},
        }},
        {added, {
            {0, {interval{6, 10}}},
        }},
    );
}

namespace {

struct throwing_key
{
    int value = 0;

    static inline int copies_until_throw = -1; // never throws if negative

    throwing_key(int value) : value(value) {}

    throwing_key(throwing_key const& other) : value(other.value)
    {
        count_copy();
    }

    throwing_key& operator=(throwing_key const& other)
    {
        count_copy();
        value = other.value;
        return *this;
    }

    static void count_copy()
    {
        if (copies_until_throw < 0) return;
        if (copies_until_throw-- == 0) throw std::runtime_error{"key copy"};
    }

    [[nodiscard]] bool operator==(throwing_key const&) const noexcept = default;
};

struct throwing_key_hash
{
    [[nodiscard]] std::size_t operator()(throwing_key const& key) const noexcept
    {
        return std::hash<int>{}(key.value);
    }
};

} // anonymous

TEST_CASE("ngram: search_result copy")
{
    iris::ngram::keyed_database<throwing_key, throwing_key_hash> ngram_db;
    for (int i = 0; i < 100; ++i) {
        ngram_db.add_document(i, std::u32string(static_cast<std::size_t>(i % 7), U'.') + U"abc xyz");
    }

    iris::ngram::search_result<throwing_key> source;
    ngram_db.search(iris::ngram::search_query{U"abc xyz"}, source);
    iris::ngram::search_result<throwing_key> target;
    ngram_db.search(iris::ngram::search_query{U"b"}, target);

    auto const matches_of = [](iris::ngram::search_result<throwing_key> const& search_res) {
        std::vector<std::pair<int, std::vector<std::vector<interval<int>>>>> matches;
        for (auto const& [key, word_matches] : search_res) {
            auto& spans = matches.emplace_back(key.value, std::vector<std::vector<interval<int>>>{}).second;
            for (auto const& word : word_matches) {
                spans.emplace_back(word.begin(), word.end());
            }
        }
        return matches;
    };
    auto const target_matches = matches_of(target);
    REQUIRE(target_matches.size() == 100);

    throwing_key::copies_until_throw = 3;
    CHECK_THROWS_AS(target = source, std::runtime_error);
    throwing_key::copies_until_throw = -1;
    CHECK(matches_of(target) == target_matches);

    target = source;
    CHECK(matches_of(target) == matches_of(source));

    // A search failing in the middle leaves no partial result, with a single gram or multiple words
    throwing_key::copies_until_throw = 3;
    CHECK_THROWS_AS(ngram_db.search(iris::ngram::search_query{U"b"}, target), std::runtime_error);
    CHECK(matches_of(target).empty());
    throwing_key::copies_until_throw = 3;
    CHECK_THROWS_AS(ngram_db.search(iris::ngram::search_query{U"abc xyz"}, target), std::runtime_error);
    CHECK(matches_of(target).empty());
    throwing_key::copies_until_throw = -1;

    ngram_db.search(iris::ngram::search_query{U"abc xyz"}, target);
    CHECK(matches_of(target) == matches_of(source));
}

TEST_CASE("ngram: search missing n-gram")
{
    iris::ngram::database<> ngram_db;
    (void)ngram_db.add_document(U"abcdef");
    IRIS_CHECK_SEARCH("axcdef");
    IRIS_CHECK_SEARCH("abxdef");
    IRIS_CHECK_SEARCH("abcdxf");
    IRIS_CHECK_SEARCH("abcdefx");
    IRIS_CHECK_SEARCH("x abc");
    IRIS_CHECK_SEARCH("abc x");
}

TEST_CASE("ngram: search_result access")
{
    iris::ngram::database<> ngram_db;
    (void)ngram_db.add_document(U"abc xyz");
    (void)ngram_db.add_document(U"none");
    (void)ngram_db.add_document(U"xyz abc abc");

    iris::ngram::search_result<> search_res;
    REQUIRE(ngram_db.search(iris::ngram::search_query{U"abc xyz"}, search_res));
    REQUIRE(search_res.size() == 2);
    CHECK(search_res.end() - search_res.begin() == 2);

    auto const [doc_id, word_matches] = search_res[1];
    CHECK(doc_id == 2_doc_id);
    REQUIRE(word_matches.size() == 2);
    CHECK(std::ranges::equal(word_matches[0], std::vector{interval{4, 7}, interval{8, 11}}));
    CHECK(std::ranges::equal(word_matches[1], std::vector{interval{0, 3}}));

    CHECK(std::get<0>(search_res.begin()[0]) == 0_doc_id);
    CHECK(std::ranges::equal(std::get<1>(*(search_res.end() - 1)).end()[-1], std::vector{interval{0, 3}}));

    CHECK(search_res.rend() - search_res.rbegin() == 2);
    CHECK(std::get<0>(*search_res.rbegin()) == 2_doc_id);
    CHECK(std::ranges::equal(*word_matches.rbegin(), std::vector{interval{0, 3}}));
    CHECK(std::ranges::equal(word_matches.rend()[-1], std::vector{interval{4, 7}, interval{8, 11}}));

    CHECK(search_res.cbegin() == search_res.begin());
    CHECK(search_res.cend() == search_res.end());
    CHECK(search_res.crend() - search_res.crbegin() == 2);
    CHECK(word_matches.cend() - word_matches.cbegin() == 2);
    CHECK(word_matches.crbegin() == word_matches.rbegin());
}

#ifndef IRIS_ZZ_TEST_NGRAM_TEST_HPP
#define IRIS_ZZ_TEST_NGRAM_TEST_HPP

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/ngram_search_query.hpp>
#include <iris/ngram_database.hpp>

#include <format>
#include <iostream>
#include <ranges>

#ifdef _MSC_VER
# include <Windows.h>
#endif

using namespace iris::ngram_literals;
using iris::ngram_occurrence;
using iris::interval;

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

#endif

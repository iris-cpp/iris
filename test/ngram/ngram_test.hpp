#ifndef IRIS_ZZ_TEST_NGRAM_TEST_HPP
#define IRIS_ZZ_TEST_NGRAM_TEST_HPP

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/ngram/search_query.hpp>
#include <iris/ngram/database.hpp>

#include <format>      // IWYU pragma: export
#include <iostream>    // IWYU pragma: export
#include <ranges>      // IWYU pragma: export
#include <concepts>    // IWYU pragma: export
#include <type_traits> // IWYU pragma: export

#ifdef _MSC_VER
# include <Windows.h> // IWYU pragma: export
#endif

using namespace iris::ngram_literals;
using iris::interval;

// The spans of a search word, identified by its index among the non-empty words
struct WordMatch
{
    int word_id;
    std::vector<interval<int>> spans;

    [[nodiscard]] bool operator==(WordMatch const&) const noexcept = default;
};

template<class CharT>
struct std::formatter<WordMatch, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(WordMatch const& word_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{{word: #{}, spans: {}}}", word_match.word_id, word_match.spans);
    }
};

template<class DocumentID = iris::ngram::document_id>
struct DocumentMatch
{
    DocumentID doc_id;
    std::vector<WordMatch> word_matches;

    template<class ID>
    DocumentMatch(ID&& doc_id, std::initializer_list<WordMatch> word_matches)
        : doc_id(std::forward<ID>(doc_id))
        , word_matches(word_matches)
    {}

    template<class ID, std::ranges::input_range WordSpans>
    DocumentMatch(ID&& doc_id, WordSpans&& word_spans)
        : doc_id(std::forward<ID>(doc_id))
    {
        int word_id = 0;
        for (auto const& spans : word_spans) {
            word_matches.emplace_back(word_id++, spans | std::ranges::to<std::vector>());
        }
    }

    [[nodiscard]]
    bool operator==(DocumentMatch const&) const noexcept = default;
};

template<class DocumentID, class CharT>
struct std::formatter<DocumentMatch<DocumentID>, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(DocumentMatch<DocumentID> const& doc_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "(doc: `{}`, word_matches: {})", doc_match.doc_id, doc_match.word_matches);
    }
};

#define IRIS_CHECK_SEARCH(query_input, ...) do { \
        iris::ngram::search_query const query{U ## query_input}; \
        iris::ngram::search_result<typename std::remove_cvref_t<decltype(ngram_db)>::document_id_type> search_res; \
        ngram_db.search(query, search_res); \
        std::vector<DocumentMatch<typename std::remove_cvref_t<decltype(ngram_db)>::document_id_type>> const expected_doc_matches{ \
            std::initializer_list<DocumentMatch<typename std::remove_cvref_t<decltype(ngram_db)>::document_id_type>>{__VA_ARGS__} \
        }; \
        \
        auto const actual_doc_matches = search_res | std::views::transform([](auto const& kv) { \
            return DocumentMatch<typename std::remove_cvref_t<decltype(ngram_db)>::document_id_type>{kv.first, kv.second}; \
        }) | std::ranges::to<std::vector>(); \
        CHECK(actual_doc_matches == expected_doc_matches); \
    } while (false)

#endif

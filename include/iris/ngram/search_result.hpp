#ifndef IRIS_ZZ_NGRAM_SEARCH_RESULT_HPP
#define IRIS_ZZ_NGRAM_SEARCH_RESULT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/id.hpp>

#include <iris/interval.hpp>
#include <iris/format.hpp>

#include <format>
#include <concepts>
#include <type_traits>
#include <ranges>
#include <vector>
#include <unordered_map>
#include <utility>

namespace iris::ngram {

namespace detail {
class search_result_cache;
} // detail

template<class CharT>
class database;

struct [[nodiscard]] search_word_match
{
    search_word_match() = default;

    explicit search_word_match(int word_id)
        : word_id_(word_id)
    {}

    search_word_match(int word_id, std::initializer_list<interval<int>> spans)
        : word_id_(word_id)
        , spans_(spans)
    {}

    [[nodiscard]] int word_id() const noexcept { return word_id_; }
    [[nodiscard]] auto const& spans() const noexcept { return spans_; }

    [[nodiscard]] bool operator==(search_word_match const& other) const noexcept
    {
        return word_id_ == other.word_id_ && spans_ == other.spans_;
    }

private:
    friend class detail::search_result_cache;

    template<class CharT>
    friend class database;

    int word_id_ = 0;
    unsigned successful_ngrams_ = 1; // due to the class layout, this must be placed here
    std::vector<interval<int>> spans_;
};

template<class DocumentID = document_id>
struct [[nodiscard]] search_result
{
    using document_id_type = DocumentID;
    using map_type = std::unordered_map<DocumentID, std::vector<search_word_match>>;

    search_result() = default;

    template<std::ranges::input_range DocumentMatchMap>
        requires requires(map_type& doc_matches) {
            doc_matches.insert_range(std::declval<DocumentMatchMap>());
        }
    void assign(DocumentMatchMap&& doc_match_map)
    {
        doc_matches_.clear();
        if constexpr (std::ranges::sized_range<DocumentMatchMap>) {
            doc_matches_.reserve(std::ranges::size(doc_match_map));
        }
        doc_matches_.insert_range(std::forward<DocumentMatchMap>(doc_match_map));
    }

    [[nodiscard]] map_type const& doc_matches() const noexcept
    {
        return doc_matches_;
    }

    void clear() noexcept
    {
        doc_matches_.clear();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return doc_matches_.empty();
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return !this->empty();
    }

private:
    map_type doc_matches_;
};

} // iris::ngram

template<class CharT>
struct std::formatter<iris::ngram::search_word_match, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram::search_word_match const& word_match, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{{word: #{}, spans: {}}}", word_match.word_id(), word_match.spans());
    }
};

#endif

#ifndef IRIS_ZZ_NGRAM_SEARCH_QUERY_HPP
#define IRIS_ZZ_NGRAM_SEARCH_QUERY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/string_algo.hpp>
#include <iris/format.hpp>

#include <iris/unicode/string.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <format>
#include <algorithm>

namespace iris::ngram {

template<class CharT = char32_t>
struct search_query
{
    search_query() = default;

    explicit search_query(std::basic_string_view<CharT> input_sv)
    {
        std::basic_string<CharT> input{input_sv};
        iris::compact_spaces(input);
        if (input.empty()) return;

        words_ = input
            | std::views::split(iris::detail::string_algo_traits<CharT>::space)
            | std::views::transform([](auto const& r) {
                return std::basic_string<CharT>{std::from_range, r};
            })
            | std::ranges::to<std::vector>();

        std::ranges::sort(words_);
        {
            auto const [first, last] = std::ranges::unique(words_);
            words_.erase(first, last);
        }
    }

    // ------------------------------------------

    [[nodiscard]] auto const& words() const noexcept
    {
        return words_;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return words_.empty();
    }

    [[nodiscard]] bool operator==(search_query const& other) const noexcept
    {
        return words_ == other.words_;
    }

private:
    std::vector<std::basic_string<CharT>> words_;
};

template<class CharT, std::size_t N>
search_query(CharT const(&)[N]) -> search_query<CharT>;

} // iris::ngram

template<class NGCharT, class CharT>
struct std::formatter<iris::ngram::search_query<NGCharT>, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram::search_query<NGCharT> const& query, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}", query.words() | std::views::transform([](std::u32string_view ustr) {
            return iris::unicode::transcode<char>(ustr);
        }));
    }
};

#endif

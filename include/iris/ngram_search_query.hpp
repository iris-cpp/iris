#ifndef IRIS_ZZ_NGRAM_SEARCH_QUERY_HPP
#define IRIS_ZZ_NGRAM_SEARCH_QUERY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/string_algo.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>

namespace iris {

template<class CharT = char32_t>
struct ngram_search_query
{
    explicit ngram_search_query(std::basic_string_view<CharT> input_sv)
    {
        std::basic_string<CharT> input{input_sv};
        iris::compact_spaces(input);
        if (input.empty()) return;

        words_ = input
            | std::views::split(detail::string_algo_traits<CharT>::space)
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

    [[nodiscard]]
    auto const& words() const noexcept
    {
        return words_;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return words_.empty();
    }

    [[nodiscard]] bool operator==(ngram_search_query const& other) const noexcept
    {
        return words_ == other.words_;
    }

private:
    std::vector<std::basic_string<CharT>> words_;
};

template<class CharT, std::size_t N>
ngram_search_query(CharT const(&)[N]) -> ngram_search_query<CharT>;

} // iris

#endif

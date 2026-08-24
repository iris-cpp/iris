#ifndef IRIS_ZZ_SNIPPET_HPP
#define IRIS_ZZ_SNIPPET_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/exception.hpp>
#include <iris/interval.hpp>
#include <iris/interval_set.hpp>

#include <iris/unicode/string.hpp>

#include <concepts>
#include <type_traits>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <format>
#include <vector>
#include <string>
#include <string_view>
#include <utility>
#include <stdexcept>

#include <cstddef> // IWYU pragma: keep
#include <cassert>

namespace iris::snip {

namespace detail {

struct gap_traits
{
    template<class SinkT>
        requires requires(SinkT const& csink) {
            { csink.gap_first_chars() } -> std::convertible_to<std::size_t>;
        }
    [[nodiscard]] static constexpr std::size_t gap_first_chars(SinkT const& csink) noexcept
    {
        return csink.gap_first_chars();
    }

    template<class SinkT>
        requires (!requires(SinkT const& csink) {
            { csink.gap_first_chars() } -> std::convertible_to<std::size_t>;
        })
    [[nodiscard]] static constexpr std::size_t gap_first_chars(SinkT const& csink) noexcept
    {
        return csink.gap_chars();
    }

    template<class SinkT>
        requires requires(SinkT& sink) {
            sink.gap_first();
        }
    static constexpr void gap_first(SinkT& sink)
    {
        return sink.gap_first();
    }

    template<class SinkT>
        requires (!requires(SinkT& sink) {
            sink.gap_first();
        })
    static constexpr void gap_first(SinkT& sink)
    {
        return sink.gap();
    }

    // ----------------------------------------

    template<class SinkT>
        requires requires(SinkT const& csink) {
            { csink.gap_last_chars() } -> std::convertible_to<std::size_t>;
        }
    [[nodiscard]] static constexpr std::size_t gap_last_chars(SinkT const& csink) noexcept
    {
        return csink.gap_last_chars();
    }

    template<class SinkT>
        requires (!requires(SinkT const& csink) {
            { csink.gap_last_chars() } -> std::convertible_to<std::size_t>;
        })
    [[nodiscard]] static constexpr std::size_t gap_last_chars(SinkT const& csink) noexcept
    {
        return csink.gap_chars();
    }

    template<class SinkT>
        requires requires(SinkT& sink) {
            sink.gap_last();
        }
    static constexpr void gap_last(SinkT& sink)
    {
        return sink.gap_last();
    }

    template<class SinkT>
        requires (!requires(SinkT& sink) {
            sink.gap_last();
        })
    static constexpr void gap_last(SinkT& sink)
    {
        return sink.gap();
    }
};

} // detail

template<class SinkT, class CharT = SinkT::char_type>
concept SnippetSink = requires(SinkT& sink, SinkT const& csink, std::basic_string_view<CharT> sv) {
    sink.context(sv);
    sink.match(sv);

    sink.gap();
    { csink.gap_chars() } -> std::convertible_to<std::size_t>;

    { detail::gap_traits::gap_first_chars(csink) } -> std::convertible_to<std::size_t>;
    detail::gap_traits::gap_first(sink);

    { detail::gap_traits::gap_last_chars(csink) } -> std::convertible_to<std::size_t>;
    detail::gap_traits::gap_last(sink);

    { sink.clear() } noexcept;
};

template<class CharT = char32_t>
struct recording_sink
{
    using char_type = CharT;

    std::vector<std::pair<CharT, std::basic_string<CharT>>>
    events;

    // The adjacent text connected to the left or the right of `match`
    void context(std::basic_string_view<CharT> sv)
    {
        events.emplace_back(static_cast<CharT>('C'), sv);
    }

    void match(std::basic_string_view<CharT> sv)
    {
        events.emplace_back(static_cast<CharT>('M'), sv);
    }

    void gap()
    {
        events.emplace_back(std::piecewise_construct, std::forward_as_tuple(static_cast<CharT>('G')), std::forward_as_tuple());
    }

    [[nodiscard]] std::size_t gap_chars() const noexcept
    {
        return 0;
    }

    void clear() noexcept
    {
        events.clear();
    }

    [[nodiscard]] std::string to_string() const
    {
        std::string str;
        bool is_first = true;
        for (auto const& [ch, substr] : events) {
            if (is_first) {
                is_first = false;
            } else {
                str += ' ';
            }
            if (ch == static_cast<CharT>('G')) {
                str += 'G';
            } else {
                std::format_to(std::back_inserter(str), "{}\"{}\"", iris::to_string_ref(ch), iris::to_string_ref(substr));
            }
        }
        return str;
    }
};

template<class CharT>
struct bbcode_search_result_tokens;

template<>
struct bbcode_search_result_tokens<char>
{
    static constexpr std::string_view GAP         = " ... ";
    static constexpr std::string_view GAP_FIRST   = "... ";
    static constexpr std::string_view GAP_LAST    = " ...";
    static constexpr std::string_view MATCH_START = "[b]";
    static constexpr std::string_view MATCH_END   = "[/b]";
};

template<>
struct bbcode_search_result_tokens<char32_t>
{
    static constexpr std::u32string_view GAP         = U" ... ";
    static constexpr std::u32string_view GAP_FIRST   = U"... ";
    static constexpr std::u32string_view GAP_LAST    = U" ...";
    static constexpr std::u32string_view MATCH_START = U"[b]";
    static constexpr std::u32string_view MATCH_END   = U"[/b]";
};

template<class CharT = char32_t, class TokensT = bbcode_search_result_tokens<CharT>>
struct search_result_sink
{
    using char_type = CharT;

    std::basic_string<CharT> result_str;

    void context(std::basic_string_view<CharT> sv)
    {
        result_str += sv;
    }

    void match(std::basic_string_view<CharT> sv)
    {
        result_str += TokensT::MATCH_START;
        result_str += sv;
        result_str += TokensT::MATCH_END;
    }

    void gap() { result_str += TokensT::GAP; }
    [[nodiscard]] std::size_t gap_chars() const noexcept { return TokensT::GAP.size(); }

    void gap_first() { result_str += TokensT::GAP_FIRST; }
    [[nodiscard]] std::size_t gap_first_chars() const noexcept { return TokensT::GAP_FIRST.size(); }

    void gap_last() { result_str += TokensT::GAP_LAST; }
    [[nodiscard]] std::size_t gap_last_chars() const noexcept { return TokensT::GAP_LAST.size(); }

    void clear() noexcept
    {
        result_str.clear();
    }

    [[nodiscard]] decltype(auto) to_string() const
    {
        return iris::to_string_ref(result_str);
    }
};

template<class CharT = char32_t>
class snippet_generator
{
public:
    template<std::ranges::forward_range MatchesR>
        requires std::convertible_to<std::ranges::range_value_t<MatchesR>, interval<int>>
    void process(
        std::basic_string_view<CharT> const input_text,
        MatchesR&& matches_r,
        interval_set<interval<int>> const& frags,
        SnippetSink<CharT> auto& sink
    )
    {
        input_text_ = input_text;
        matches_.clear();
        frags_.clear();
        sink.clear();

        matches_.assign_range(std::forward<MatchesR>(matches_r));
        frags_ = frags;
        this->process_impl(sink);
    }

    template<std::ranges::forward_range MatchesR>
        requires std::convertible_to<std::ranges::range_value_t<MatchesR>, interval<int>>
    void process(
        std::basic_string<CharT> const&& input_text,
        MatchesR&& matches_r,
        interval_set<interval<int>> const& frags,
        SnippetSink<CharT> auto& sink
    ) = delete;

    template<std::ranges::forward_range MatchesR>
        requires std::convertible_to<std::ranges::range_value_t<MatchesR>, interval<int>>
    void process(
        std::basic_string_view<CharT> const input_text,
        MatchesR&& matches_r,
        int const result_max_chars,
        SnippetSink<CharT> auto& sink
    )
    {
        input_text_ = input_text;
        matches_.clear();
        frags_.clear();
        sink.clear();

        matches_.assign_range(std::forward<MatchesR>(matches_r));
        this->generate_frags(
            result_max_chars,
            static_cast<int>(detail::gap_traits::gap_first_chars(sink)),
            static_cast<int>(sink.gap_chars()),
            static_cast<int>(detail::gap_traits::gap_last_chars(sink))
        );
        this->process_impl(sink);
    }

    template<std::ranges::forward_range MatchesR>
        requires std::convertible_to<std::ranges::range_value_t<MatchesR>, interval<int>>
    void process(
        std::basic_string<CharT> const&& input_text,
        MatchesR&& matches_r,
        int const result_max_chars,
        SnippetSink<CharT> auto& sink
    ) = delete;

private:
    void process_impl(SnippetSink<CharT> auto& sink)
    {
        if (!frags_.extent().within(input_text_)) {
            throw std::out_of_range{"frags is outside input text"};
        }

        auto match_it = matches_.begin();
        int last_upper = 0;
        for (auto const& frag : frags_) {
            if (frag.lower != last_upper) {
                if (last_upper == 0) {
                    detail::gap_traits::gap_first(sink);
                } else {
                    sink.gap();
                }
            }

            int pos = frag.lower;
            for (; match_it != matches_.end() && frag.encloses(*match_it) && pos <= match_it->lower; ++match_it) {
                if (pos != match_it->lower) {
                    sink.context(interval{pos, match_it->lower}.as_subview_of(input_text_));
                }
                sink.match(match_it->as_subview_of(input_text_));
                pos = match_it->upper;
            }
            if (pos != frag.upper) {
                sink.context(interval{pos, frag.upper}.as_subview_of(input_text_));
            }
            last_upper = frag.upper;
        }
        if (match_it != matches_.end()) {
            sink.clear();
            throwf<std::invalid_argument>("match {} not covered by any fragment", *match_it);
        }

        if (!frags_.empty() && last_upper != static_cast<int>(input_text_.size())) {
            detail::gap_traits::gap_last(sink);
        }
    }

    void generate_frags(int const result_max_chars, int const gap_first_chars, int const gap_chars, int const gap_last_chars)
    {
        assert(frags_.empty());
        if (input_text_.empty()) {
            matches_.clear();
            return;
        }

        int sum = 0;
        for (auto const& match : matches_) {
            sum += match.length();
        }

        int pad = 0;
        while (!matches_.empty()) {
            int const n = static_cast<int>(matches_.size());
            // worst case: leading + trailing + (n - 1) inner gaps
            int const reserve = gap_first_chars + gap_last_chars + (n - 1) * gap_chars;
            int const budget = result_max_chars - reserve - sum;

            if (budget < 0) {
                if (n == 1) {
                    this->truncate_sole_match(result_max_chars, gap_first_chars, gap_last_chars);
                    return;
                }
                sum -= matches_.back().length();
                matches_.pop_back();
                continue;
            }
            pad = budget / (2 * n);
            if (pad == 0 && std::ranges::any_of(matches_, &interval<int>::empty)) {
                std::erase_if(matches_, [](auto const m) noexcept { return m.empty(); });
                continue;
            }
            break;
        }
        if (matches_.empty()) return;

        interval<int> const bounds{0, static_cast<int>(input_text_.size())};
        for (auto const& match : matches_) {
            frags_.insert(interval{match.lower - pad, match.upper + pad}.intersection(bounds));
        }
    }

    void truncate_sole_match(
        int const result_max_chars,
        int const gap_first_chars, int const gap_last_chars
    ) {
        assert(matches_.size() == 1);
        auto& match = matches_.front();

        // leading gap iff text precedes the match; trailing assumed
        int const allowed = result_max_chars - (match.lower > 0 ? gap_first_chars : 0) - gap_last_chars;
        if (allowed <= 0 || match.empty()) {
            matches_.clear();
            return;
        }
        match = interval{match.lower, match.lower + std::min(match.length(), allowed)};
        frags_.insert(match);
    }

    std::basic_string_view<CharT> input_text_;
    std::vector<interval<int>> matches_;
    interval_set<interval<int>> frags_;
};

} // iris::snip

#endif

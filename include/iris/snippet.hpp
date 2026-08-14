#ifndef IRIS_ZZ_SNIPPET_HPP
#define IRIS_ZZ_SNIPPET_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>

#include <iris/exception.hpp>
#include <iris/interval.hpp>
#include <iris/interval_set.hpp>

#include <iris/unicode/string.hpp>

#include <iterator>
#include <format>
#include <vector>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <stdexcept>

namespace iris::snip {

template<class SinkT, class CharT = SinkT::char_type>
concept snippet_sink = requires(SinkT& sink, SinkT const& csink, std::basic_string_view<CharT> sv) {
    sink.context(sv);
    sink.match(sv);
    sink.gap();
    sink.clear();
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

template<class CharT = char32_t>
class snippet_generator
{
public:
    using char_type = CharT;

    void process(
        std::basic_string_view<CharT> input,
        std::vector<interval<int>> const& matches,
        interval_set<interval<int>> const& frags,
        snippet_sink<CharT> auto& sink
    )
    {
        sink.clear();

        if (!frags.extent().within(input)) {
            throw std::out_of_range{"frags is outside input text"};
        }

        auto match_it = matches.begin();
        int last_upper = 0;
        for (auto const frag : frags) {
            if (frag.lower != last_upper) {
                sink.gap();
            }

            int pos = frag.lower;
            for (; match_it != matches.end() && frag.encloses(*match_it) && pos <= match_it->lower; ++match_it) {
                if (pos != match_it->lower) {
                    sink.context(interval{pos, match_it->lower}.as_subview_of(input));
                }
                sink.match(match_it->as_subview_of(input));
                pos = match_it->upper;
            }
            if (pos != frag.upper) {
                sink.context(interval{pos, frag.upper}.as_subview_of(input));
            }
            last_upper = frag.upper;
        }
        if (match_it != matches.end()) {
            sink.clear();
            throwf<std::invalid_argument>("match {} not covered by any fragment", *match_it);
        }
        if (!frags.empty() && last_upper != static_cast<int>(input.size())) {
            sink.gap();
        }
    }
};

} // iris::snip

#endif

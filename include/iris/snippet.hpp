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
    sink.marginal(sv);
    sink.highlight(sv);
    sink.gap();
};

template<class CharT = char32_t>
struct recording_sink
{
    using char_type = CharT;

    std::vector<std::pair<CharT, std::basic_string<CharT>>>
    events;

    void marginal(std::basic_string_view<CharT> sv)
    {
        events.emplace_back(static_cast<CharT>('M'), sv);
    }

    void highlight(std::basic_string_view<CharT> sv)
    {
        events.emplace_back(static_cast<CharT>('H'), sv);
    }

    void gap()
    {
        events.emplace_back(std::piecewise_construct, std::forward_as_tuple(static_cast<CharT>('G')), std::forward_as_tuple());
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
        std::vector<interval<int>> const& winners,
        interval_set<interval<int>> const& frags,
        snippet_sink<CharT> auto& sink
    )
    {
        if (frags.empty()) return;
        if (frags.size() != winners.size()) {
            throw std::invalid_argument{"frags.size() does not match winners.size()"};
        }
        if (!frags.extent().within(input)) {
            throw std::out_of_range{"frags is outside input text"};
        }

        auto w = winners.begin();
        int last_upper = 0;
        for (auto const frag : frags) {
            if (frag.lower != last_upper) {
                sink.gap();
            }

            int pos = frag.lower;
            for (; w != winners.end() && frag.covers(*w) && pos <= w->lower; ++w) {
                if (pos != w->lower) {
                    sink.marginal(interval{pos, w->lower}.as_subview_of(input));
                }
                sink.highlight(w->as_subview_of(input));
                pos = w->upper;
            }
            if (pos != frag.upper) {
                sink.marginal(interval{pos, frag.upper}.as_subview_of(input));
            }
            last_upper = frag.upper;
        }
        if (w != winners.end()) {
            throwf<std::invalid_argument>("winner {} not covered by any fragment", *w);
        }
        if (last_upper != static_cast<int>(input.size())) {
            sink.gap();
        }
    }
};

} // iris::snip

#endif

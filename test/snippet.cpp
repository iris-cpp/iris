// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/snippet.hpp>
#include <iris/interval.hpp>
#include <iris/interval_set.hpp>

#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

namespace snip = iris::snip;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("snippet: type traits")
{
    STATIC_CHECK(snip::SnippetSink<snip::recording_sink<>>);
}

TEST_CASE("snippet: process predefined frags")
{
   // Match flush at fragment start; full coverage, no gaps
    {
        auto const input = U"XXabc"sv;

        std::vector<iris::interval<int>> matches{
            {0, 2}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(M"XX" C"abc")");
    }

    // Match flush at fragment end
    {
        auto const input = U"abcXX"sv;

        std::vector<iris::interval<int>> matches{
            {3, 5}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(C"abc" M"XX")");
    }

    // Two matches merged into one fragment (close regime), no gaps
    {
        auto const input = U"aaXXbbYYcc"sv;

        std::vector<iris::interval<int>> matches{
            {2, 4}, // XX
            {6, 8}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 10});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(C"aa" M"XX" C"bb" M"YY" C"cc")");
    }

    // Merged fragment with both leading and trailing gap
    {
        auto const input = U"aaXXbbYYcc"sv;

        std::vector<iris::interval<int>> matches{
            {2, 4}, // XX
            {6, 8}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 9});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(G C"a" M"XX" C"bb" M"YY" C"c" G)");
    }

    // Adjacent matches: no empty context between matches
    {
        auto const input = U"abXXYYcd"sv;

        std::vector<iris::interval<int>> matches{
            {2, 4}, // XX
            {4, 6}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 8});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(C"ab" M"XX" M"YY" C"cd")");
    }

    // Match equals fragment: match only, gaps both sides
    {
        auto const input = U"aXXb"sv;

        std::vector<iris::interval<int>> matches{
            {1, 3}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 3});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(G M"XX" G)");
    }

    // Caret match mid-fragment: empty match
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> matches{
            {3, 3}, // caret
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(G C"bc" M"" C"de" G)");
    }

    // Caret match at fragment upper edge (gap domain membership)
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> matches{
            {5, 5}, // caret at edge
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(G C"bcde" M"" G)");
    }

    // Three fragments: mixed edge-flush matches, inner gaps, no outer gaps
    {
        auto const input = U"XXaaaaYYbbbbZZ"sv;

        std::vector<iris::interval<int>> matches{
            { 0,  2}, // XX
            { 6,  8}, // YY
            {12, 14}, // ZZ
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({ 0,  3});
        frags.insert({ 5,  9});
        frags.insert({11, 14});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == R"(M"XX" C"a" G C"a" M"YY" C"b" G C"b" M"ZZ")");
    }

    // Empty matches and empty frags: no events
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> matches;
        iris::interval_set<iris::interval<int>> frags;

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, frags, sink);

        CHECK(sink.to_string() == "");
        CHECK(sink.events.empty());
    }

    // Match not covered by any fragment
    {
        auto const input = U"abcdefghi"sv;

        std::vector<iris::interval<int>> matches{
            {3, 5},
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({6, 9});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, matches, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Caret match outside every fragment
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> matches{
            {8, 8}, // stray caret
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, matches, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Nonempty matches with empty frags
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> matches{
            {1, 3},
        };
        iris::interval_set<iris::interval<int>> frags;

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, matches, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Unsorted matches
    {
        auto const input = U"abcXXdefghiYYjkl"sv;

        std::vector<iris::interval<int>> matches{
            {11, 13}, // YY first
            { 3,  5}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1,  7});
        frags.insert({9, 15});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, matches, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Fragment extent outside input text
    {
        auto const input = U"abc"sv;

        std::vector<iris::interval<int>> matches{
            {1, 2},
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 100});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, matches, frags, sink), std::out_of_range);
        CHECK(sink.events.empty());
    }
}

TEST_CASE("snippet budget (recording_sink)")
{
    // Gap widths of recording_sink are all 0, so the budget is text chars only.

    // Match longer than cap: truncated
    {
        auto const input = U"abcdeMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMvwxyz"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {5, 35},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 20, sink);

        CHECK(sink.to_string() == R"(G M"MMMMMMMMMMMMMMMMMMMM" G)");
    }

    // Match at offset 0: no leading gap
    {
        auto const input = U"MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMabcdefghij"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {0, 30},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 25, sink);

        CHECK(sink.to_string() == R"(M"MMMMMMMMMMMMMMMMMMMMMMMMM" G)");
    }

    // Cap fits only the first two matches
    {
        auto const input =
            U"012345678901234567890123456789012345678901234567890123456789"sv; // 60 chars

        std::vector<iris::interval<int>> matches{
            { 5,  7},
            {20, 22},
            {40, 42}, // dropped
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 5, sink);

        CHECK(sink.to_string() == R"(G M"56" G M"01" G)");
    }

    // No room for context: caret dropped, freed budget pads the survivor
    {
        auto const input = U"0123456789012345678901234567890123456789"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            { 3,  5},
            {20, 20}, // caret, dropped
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 4, sink);

        CHECK(sink.to_string() == R"(G C"2" M"34" C"5" G)");
    }

    // Caret with enough budget for context
    {
        auto const input = U"0123456789012345678901234567890123456789"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {10, 10},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 10, sink);

        CHECK(sink.to_string() == R"(G C"56789" M"" C"01234" G)");
    }

    // Caret without any context budget: nothing to show
    {
        auto const input = U"0123456789012345678901234567890123456789"sv;

        std::vector<iris::interval<int>> matches{
            {10, 10},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 1, sink);

        CHECK(sink.events.empty());
    }

    // Empty text
    {
        auto const input = U""sv;

        std::vector<iris::interval<int>> matches{
            {0, 0},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 50, sink);

        CHECK(sink.events.empty());
    }

    // Cap zero
    {
        auto const input = U"01234567890123456789"sv;

        std::vector<iris::interval<int>> matches{
            {5, 7},
        };

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 0, sink);

        CHECK(sink.events.empty());
    }
}

TEST_CASE("snippet budget (search_result_sink)")
{
    // Gap widths: "... " = 4, " ... " = 5, " ..." = 4

    // Match longer than cap: truncated to 20 - 4 - 4 = 12 chars
    {
        auto const input = U"abcdeMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMvwxyz"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {5, 35},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 20, sink);

        CHECK(sink.result_str == U"... [b]MMMMMMMMMMMM[/b] ...");
    }

    // Match at offset 0: no leading gap, so 35 - 4 = 31 chars fit it untruncated
    {
        auto const input = U"MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMabcdefghij"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {0, 30},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 35, sink);

        CHECK(sink.result_str == U"[b]MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM[/b] ...");
    }

    // Cap fits only the first two matches, 1 char of context each side
    {
        auto const input =
            U"012345678901234567890123456789012345678901234567890123456789"sv; // 60 chars

        std::vector<iris::interval<int>> matches{
            { 5,  7},
            {20, 22},
            {40, 42}, // dropped
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 21, sink);

        CHECK(sink.result_str == U"... 4[b]56[/b]7 ... 9[b]01[/b]2 ...");
    }

    // No room for context: caret dropped, freed budget pads the survivor
    {
        auto const input = U"0123456789012345678901234567890123456789"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            { 3,  5},
            {20, 20}, // caret, dropped
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 15, sink);

        CHECK(sink.result_str == U"... 12[b]34[/b]56 ...");
    }

    // Caret with context budget renders as empty [b][/b]
    {
        auto const input = U"0123456789012345678901234567890123456789"sv; // 40 chars

        std::vector<iris::interval<int>> matches{
            {10, 10},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 20, sink);

        CHECK(sink.result_str == U"... 456789[b][/b]012345 ...");
    }

    // Caret without any context budget: nothing to show
    {
        auto const input = U"0123456789012345678901234567890123456789"sv;

        std::vector<iris::interval<int>> matches{
            {10, 10},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 5, sink);

        CHECK(sink.result_str.empty());
    }

    // Empty text
    {
        auto const input = U""sv;

        std::vector<iris::interval<int>> matches{
            {0, 0},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 50, sink);

        CHECK(sink.result_str.empty());
    }

    // Cap zero
    {
        auto const input = U"01234567890123456789"sv;

        std::vector<iris::interval<int>> matches{
            {5, 7},
        };

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, matches, 0, sink);

        CHECK(sink.result_str.empty());
    }

    // Reused generator and sink: second result replaces the first
    {
        auto const input = U"0123456789012345678901234567890123456789"sv;

        snip::search_result_sink<> sink;
        snip::snippet_generator<> sgen;

        std::vector<iris::interval<int>> first{{3, 5}};
        sgen.process(input, first, 15, sink);
        std::vector<iris::interval<int>> second{{10, 10}};
        sgen.process(input, second, 20, sink);

        CHECK(sink.result_str == U"... 456789[b][/b]012345 ...");
    }
}

// NOLINTEND(readability-container-size-empty)

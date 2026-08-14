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
    STATIC_CHECK(snip::snippet_sink<snip::recording_sink<>>);
}

TEST_CASE("snippet: process")
{
   // Winner flush at fragment start; full coverage, no gaps
    {
        auto const input = U"XXabc"sv;

        std::vector<iris::interval<int>> winners{
            {0, 2}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(M"XX" C"abc")");
    }

    // Winner flush at fragment end
    {
        auto const input = U"abcXX"sv;

        std::vector<iris::interval<int>> winners{
            {3, 5}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(C"abc" M"XX")");
    }

    // Two winners merged into one fragment (close regime), no gaps
    {
        auto const input = U"aaXXbbYYcc"sv;

        std::vector<iris::interval<int>> winners{
            {2, 4}, // XX
            {6, 8}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 10});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(C"aa" M"XX" C"bb" M"YY" C"cc")");
    }

    // Merged fragment with both leading and trailing gap
    {
        auto const input = U"aaXXbbYYcc"sv;

        std::vector<iris::interval<int>> winners{
            {2, 4}, // XX
            {6, 8}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 9});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(G C"a" M"XX" C"bb" M"YY" C"c" G)");
    }

    // Adjacent winners: no empty context between matches
    {
        auto const input = U"abXXYYcd"sv;

        std::vector<iris::interval<int>> winners{
            {2, 4}, // XX
            {4, 6}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 8});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(C"ab" M"XX" M"YY" C"cd")");
    }

    // Winner equals fragment: match only, gaps both sides
    {
        auto const input = U"aXXb"sv;

        std::vector<iris::interval<int>> winners{
            {1, 3}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 3});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(G M"XX" G)");
    }

    // Caret winner mid-fragment: empty match
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> winners{
            {3, 3}, // caret
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(G C"bc" M"" C"de" G)");
    }

    // Caret winner at fragment upper edge (gap domain membership)
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> winners{
            {5, 5}, // caret at edge
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(G C"bcde" M"" G)");
    }

    // Three fragments: mixed edge-flush winners, inner gaps, no outer gaps
    {
        auto const input = U"XXaaaaYYbbbbZZ"sv;

        std::vector<iris::interval<int>> winners{
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
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(M"XX" C"a" G C"a" M"YY" C"b" G C"b" M"ZZ")");
    }

    // Empty winners and empty frags: no events
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> winners;
        iris::interval_set<iris::interval<int>> frags;

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == "");
        CHECK(sink.events.empty());
    }

    // Winner not covered by any fragment
    {
        auto const input = U"abcdefghi"sv;

        std::vector<iris::interval<int>> winners{
            {3, 5},
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({6, 9});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, winners, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Caret winner outside every fragment
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> winners{
            {8, 8}, // stray caret
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1, 5});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, winners, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Nonempty winners with empty frags
    {
        auto const input = U"abcdef"sv;

        std::vector<iris::interval<int>> winners{
            {1, 3},
        };
        iris::interval_set<iris::interval<int>> frags;

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, winners, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Unsorted winners
    {
        auto const input = U"abcXXdefghiYYjkl"sv;

        std::vector<iris::interval<int>> winners{
            {11, 13}, // YY first
            { 3,  5}, // XX
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({1,  7});
        frags.insert({9, 15});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, winners, frags, sink), std::invalid_argument);
        CHECK(sink.events.empty());
    }

    // Fragment extent outside input text
    {
        auto const input = U"abc"sv;

        std::vector<iris::interval<int>> winners{
            {1, 2},
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({0, 100});

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        CHECK_THROWS_AS(sgen.process(input, winners, frags, sink), std::out_of_range);
        CHECK(sink.events.empty());
    }
}

// NOLINTEND(readability-container-size-empty)

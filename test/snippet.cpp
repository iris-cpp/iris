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

TEST_CASE("snippet: type traits")
{
    STATIC_CHECK(snip::snippet_sink<snip::recording_sink<>>);
}

TEST_CASE("snippet: process")
{
    {
        auto const input = U"abcXXdefghiYYjkl"sv;

        std::vector<iris::interval<int>> winners{
            { 3,  5}, // XX
            {11, 13}, // YY
        };
        iris::interval_set<iris::interval<int>> frags;
        frags.insert({ 1,  7}); // around XX
        frags.insert({ 9, 15}); // around YY until text end

        snip::recording_sink<> sink;
        snip::snippet_generator<> sgen;
        sgen.process(input, winners, frags, sink);

        CHECK(sink.to_string() == R"(G M"bc" H"XX" M"de" G M"hi" H"YY" M"jk" G)");
    }
}

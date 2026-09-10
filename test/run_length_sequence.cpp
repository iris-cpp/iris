// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/run_length_sequence.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <ranges>

using namespace std::string_view_literals;
using namespace std::string_literals;

TEST_CASE("run_length_sequence: traits")
{
    using RLS = iris::run_length_sequence<double>;
    STATIC_CHECK(std::same_as<RLS::item_type, double>);
    STATIC_CHECK(std::same_as<RLS::index_type, unsigned>);

    STATIC_CHECK(std::same_as<RLS::value_type, iris::indexed_value<unsigned, double>>);
    STATIC_CHECK(std::is_trivially_copyable_v<RLS::value_type>);

    STATIC_CHECK(std::same_as<RLS::value_type, std::iter_value_t<RLS::iterator>>);
    STATIC_CHECK(std::same_as<RLS::reference, std::iter_reference_t<RLS::iterator>>);
    STATIC_CHECK(std::same_as<RLS::const_reference, std::iter_reference_t<RLS::const_iterator>>);

    STATIC_CHECK(std::indirectly_readable<RLS::iterator>);
    STATIC_CHECK(std::bidirectional_iterator<RLS::iterator>);

    STATIC_CHECK(std::indirectly_readable<RLS::const_iterator>);
    STATIC_CHECK(std::bidirectional_iterator<RLS::const_iterator>);

    STATIC_CHECK(std::ranges::bidirectional_range<RLS>);
    STATIC_CHECK(std::ranges::bidirectional_range<RLS const>);

    STATIC_CHECK(std::sized_sentinel_for<RLS::const_iterator, RLS::iterator>);

    STATIC_CHECK(std::default_initializable<RLS>);
    STATIC_CHECK(std::is_copy_constructible_v<RLS>);
    STATIC_CHECK(std::is_copy_assignable_v<RLS>);
    STATIC_CHECK(std::is_move_constructible_v<RLS>);
    STATIC_CHECK(std::is_move_assignable_v<RLS>);

    {
        RLS::value_type iv{};
        auto&& [index, value] = iv;
        index = 42;
        value = 1.0;
        CHECK(iv.index == 42);
        CHECK(iv.value == 1.0);
    }
    {
        RLS::value_type iv{};
        auto [index, value] = iv;
        index = 42;
        value = 1.0;
        CHECK(iv.index == 0);
        CHECK(iv.value == 0.0);  // NOLINT(readability-container-size-empty)
    }
}

TEST_CASE("run_length_sequence: construction")
{
    constexpr auto expected_input = std::array{2, 3, 3, 4, 2, 2, 3, 4, 4};
    auto const expected_runs = std::vector{2, 3, 4, 2, 3, 4};

    // std::from_range
    {
        iris::run_length_sequence<int, unsigned, std::tuple> seq{std::from_range, expected_input};
        auto const expected_elems = std::views::zip(std::views::iota(0u), expected_input) | std::ranges::to<std::vector>();
        auto const actual_elems = seq | std::ranges::to<std::vector>();
        CHECK(expected_elems == actual_elems);
        CHECK(seq.run_count() == expected_runs.size());
        CHECK(seq.runs() == expected_runs);

        auto const run_view = seq.run_view();
        auto it = run_view.begin();
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 2);
            CHECK(run_ref.span.equals({0, 1}));
        }
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 3);
            CHECK(run_ref.span.equals({1, 3}));
        }
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 4);
            CHECK(run_ref.span.equals({3, 4}));
        }
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 2);
            CHECK(run_ref.span.equals({4, 6}));
        }
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 3);
            CHECK(run_ref.span.equals({6, 7}));
        }
        {
            REQUIRE(it != run_view.end());
            auto const run_ref = *it++;
            CHECK(run_ref.value == 4);
            CHECK(run_ref.span.equals({7, 9}));
        }
        REQUIRE(it == run_view.end());
    }

    // it, se
    {
        iris::run_length_sequence<int, unsigned, std::tuple> seq{expected_input.begin(), expected_input.end()};
        auto const expected_elems = std::views::zip(std::views::iota(0u), expected_input) | std::ranges::to<std::vector>();
        auto const actual_elems = seq | std::ranges::to<std::vector>();
        CHECK(expected_elems == actual_elems);
        CHECK(seq.run_count() == expected_runs.size());
        CHECK(seq.runs() == expected_runs);
    }

    // initializer list
    {
        iris::run_length_sequence<int, unsigned, std::tuple> seq{2, 3, 3, 4, 2, 2, 3, 4, 4};
        auto const expected_elems = std::views::zip(std::views::iota(0u), expected_input) | std::ranges::to<std::vector>();
        auto const actual_elems = seq | std::ranges::to<std::vector>();
        CHECK(expected_elems == actual_elems);
        CHECK(seq.run_count() == expected_runs.size());
        CHECK(seq.runs() == expected_runs);
    }

    // initializer list + append_range
    {
        iris::run_length_sequence<int, unsigned, std::tuple> seq{2, 3, 3, 4, 2};
        seq.append_range(std::array{2, 3, 4, 4});
        auto const expected_elems = std::views::zip(std::views::iota(0u), expected_input) | std::ranges::to<std::vector>();
        auto const actual_elems = seq | std::ranges::to<std::vector>();
        CHECK(expected_elems == actual_elems);
        CHECK(seq.run_count() == expected_runs.size());
        CHECK(seq.runs() == expected_runs);
    }
}

TEST_CASE("run_length_sequence: emplace_back / push_back / pop_back")
{
    using RLS = iris::run_length_sequence<std::string>;

    // emplace_back(char array)
    {
        RLS seq;
        auto&& elem = seq.emplace_back("foo");
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }
    // emplace_back(string_view)
    {
        RLS seq;
        auto&& elem = seq.emplace_back("foo"sv);
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }
    // emplace_back(string)
    {
        RLS seq;
        auto&& elem = seq.emplace_back("foo"s);
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }

    // push_back(char array)
    {
        RLS seq;
        auto&& elem = seq.push_back("foo");
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }
    // push_back(string_view)
    {
        RLS seq;
        auto&& elem = seq.push_back("foo"sv);
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }
    // push_back(string)
    {
        RLS seq;
        auto&& elem = seq.push_back("foo"s);
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "foo");

        seq.pop_back();
        CHECK(seq.empty());
    }
    // push_back(initializer_list)
    {
        constexpr auto str_arr = std::array{'a', 'b', 'c'};

        RLS seq;
        auto&& elem = seq.push_back({str_arr.begin(), str_arr.end()}); // this cannot be resolved by `emplace_back`
        CHECK(seq.size() == 1);
        CHECK(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == "abc");

        seq.pop_back();
        CHECK(seq.empty());
    }
}

TEST_CASE("run_length_sequence: sequential insertion")
{
    using RLS = iris::run_length_sequence<double>;
    RLS seq;
    CHECK(seq == seq);
    CHECK(seq.empty());
    CHECK(seq.size() == 0);  // NOLINT(readability-container-size-empty)
    CHECK(seq.begin() == seq.end());
    CHECK(seq.nth(0) == seq.end());

    {
        using std::swap;
        swap(seq, seq);
    }

    CHECK_NOTHROW((void)(seq.cbegin() = seq.begin()));

    {
        auto&& elem = seq.emplace_back(1.0);
        REQUIRE(seq.size() == 1);
        REQUIRE(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == 1.0);
        CHECK(seq.front() == elem);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        CHECK(seq.nth(1) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);
        CHECK(seq.front() == elem);
        CHECK(seq.back() == e0);

        REQUIRE(it == seq.end());
    }
    {
        auto&& elem = seq.emplace_back(1.0);
        REQUIRE(seq.size() == 2);
        REQUIRE(seq.run_count() == 1);
        CHECK(elem.index == 1);
        CHECK(elem.value == 1.0);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        {
            auto nth1 = *seq.nth(1);
            CHECK(nth1.index == 1);
            CHECK(nth1.value == 1.0);
        }
        CHECK(seq.nth(2) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it == seq.end());
    }
    {
        auto&& elem = seq.emplace_back(1.1);
        REQUIRE(seq.size() == 3);
        REQUIRE(seq.run_count() == 2);
        CHECK(elem.index == 2);
        CHECK(elem.value == 1.1);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        {
            auto nth1 = *seq.nth(1);
            CHECK(nth1.index == 1);
            CHECK(nth1.value == 1.0);
        }
        {
            auto nth2 = *seq.nth(2);
            CHECK(nth2.index == 2);
            CHECK(nth2.value == 1.1);
        }
        CHECK(seq.nth(3) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it == seq.end());
    }
    {
        auto&& elem = seq.emplace_back(1.1);
        REQUIRE(seq.size() == 4);
        REQUIRE(seq.run_count() == 2);
        CHECK(elem.index == 3);
        CHECK(elem.value == 1.1);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        {
            auto nth1 = *seq.nth(1);
            CHECK(nth1.index == 1);
            CHECK(nth1.value == 1.0);
        }
        {
            auto nth2 = *seq.nth(2);
            CHECK(nth2.index == 2);
            CHECK(nth2.value == 1.1);
        }
        {
            auto nth3 = *seq.nth(3);
            CHECK(nth3.index == 3);
            CHECK(nth3.value == 1.1);
        }
        CHECK(seq.nth(4) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it == seq.end());
    }
    {
        auto&& elem = seq.emplace_back(1.1);
        REQUIRE(seq.size() == 5);
        REQUIRE(seq.run_count() == 2);
        CHECK(elem.index == 4);
        CHECK(elem.value == 1.1);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        {
            auto nth1 = *seq.nth(1);
            CHECK(nth1.index == 1);
            CHECK(nth1.value == 1.0);
        }
        {
            auto nth2 = *seq.nth(2);
            CHECK(nth2.index == 2);
            CHECK(nth2.value == 1.1);
        }
        {
            auto nth3 = *seq.nth(3);
            CHECK(nth3.index == 3);
            CHECK(nth3.value == 1.1);
        }
        {
            auto nth4 = *seq.nth(4);
            CHECK(nth4.index == 4);
            CHECK(nth4.value == 1.1);
        }
        CHECK(seq.nth(5) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e4 = *it++;
        CHECK(e4.index == 4);
        CHECK(e4.value == 1.1);

        REQUIRE(it == seq.end());
    }
    {
        auto&& elem = seq.emplace_back(1.2);
        REQUIRE(seq.size() == 6);
        REQUIRE(seq.run_count() == 3);
        CHECK(elem.index == 5);
        CHECK(elem.value == 1.2);
        CHECK(seq.back() == elem);
        {
            auto nth0 = *seq.nth(0);
            CHECK(nth0.index == 0);
            CHECK(nth0.value == 1.0);
        }
        {
            auto nth1 = *seq.nth(1);
            CHECK(nth1.index == 1);
            CHECK(nth1.value == 1.0);
        }
        {
            auto nth2 = *seq.nth(2);
            CHECK(nth2.index == 2);
            CHECK(nth2.value == 1.1);
        }
        {
            auto nth3 = *seq.nth(3);
            CHECK(nth3.index == 3);
            CHECK(nth3.value == 1.1);
        }
        {
            auto nth4 = *seq.nth(4);
            CHECK(nth4.index == 4);
            CHECK(nth4.value == 1.1);
        }
        {
            auto nth5 = *seq.nth(5);
            CHECK(nth5.index == 5);
            CHECK(nth5.value == 1.2);
        }
        CHECK(seq.nth(6) == seq.end());

        auto it = seq.begin();

        REQUIRE(it != seq.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != seq.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e4 = *it++;
        CHECK(e4.index == 4);
        CHECK(e4.value == 1.1);

        REQUIRE(it != seq.end());
        auto&& e5 = *it++;
        CHECK(e5.index == 5);
        CHECK(e5.value == 1.2);

        REQUIRE(it == seq.end());
    }
}

TEST_CASE("run_length_sequence: append_run")
{
    using RLS = iris::run_length_sequence<std::string>;

    {
        RLS seq;
        seq.append_run(3, "foo");
        CHECK(seq.size() == 3);
        CHECK(seq.run_count() == 1);
        CHECK(seq.front().index == 0);
        CHECK(seq.front().value == "foo");
        CHECK(seq.back().index == 2);
    }
    {
        RLS seq;
        seq.append_run(3, "foo");
        seq.append_run(2, "foo");
        CHECK(seq.size() == 5);
        CHECK(seq.run_count() == 1);
        CHECK(seq.back().index == 4);
        CHECK(seq.back().value == "foo");
    }
    {
        RLS seq;
        seq.append_run(3, "foo");
        seq.append_run(2, "bar");
        CHECK(seq.size() == 5);
        CHECK(seq.run_count() == 2);
        CHECK(seq.nth(3).run_span() == iris::interval<unsigned>{3, 5});
        CHECK(seq.back().value == "bar");
        CHECK(seq.runs() == std::vector<std::string>{"foo", "bar"});
    }
    {
        RLS seq;
        seq.append_run(1, "foo");
        seq.append_run(4, "foo"sv);
        CHECK(seq.size() == 5);
        CHECK(seq.run_count() == 1);
    }
    {
        RLS seq;
        seq.append_run(0, "foo");
        CHECK(seq.empty());
        CHECK(seq.run_count() == 0);

        seq.append_run(2, "foo");
        seq.append_run(0, "bar");
        CHECK(seq.size() == 2);
        CHECK(seq.run_count() == 1);
        CHECK(seq.runs() == std::vector<std::string>{"foo"});
    }
    {
        constexpr auto values = std::array{"a"sv, "a"sv, "b"sv, "c"sv, "c"sv, "c"sv, "a"sv};
        constexpr auto counts = std::array{2u, 3u, 1u, 4u, 1u, 2u, 5u};

        RLS by_run;
        RLS by_elem;
        for (auto const [value, count] : std::views::zip(values, counts)) {
            by_run.append_run(count, value);
            for (auto i = 0u; i < count; ++i) {
                by_elem.emplace_back(value);
            }
        }
        CHECK(by_run == by_elem);
        CHECK(by_run.size() == 18);
        CHECK(by_run.run_count() == 4);
        CHECK(by_run.runs() == std::vector<std::string>{"a", "b", "c", "a"});

        auto const spans = by_run.run_view()
            | std::views::transform([](auto const& r) { return r.span; })
            | std::ranges::to<std::vector>();
        using iv = iris::interval<unsigned>;
        CHECK(spans == std::vector{iv{0, 5}, iv{5, 6}, iv{6, 13}, iv{13, 18}});
    }
    {
        using RLS_int = iris::run_length_sequence<int>;
        RLS_int seq;
        seq.append_run(1, 7);
        seq.append_run(RLS_int::max_size() - 1, 7);
        CHECK(seq.size() == RLS_int::max_size());
        CHECK(seq.run_count() == 1);

        CHECK_THROWS_AS(seq.append_run(1, 7), std::length_error);
        CHECK_THROWS_AS(seq.append_run(1, 8), std::length_error);
        CHECK_THROWS_AS(seq.emplace_back(8), std::length_error);
        CHECK(seq.size() == RLS_int::max_size());
        CHECK(seq.run_count() == 1);
    }
    {
        using RLS_int = iris::run_length_sequence<int>;
        RLS_int seq;
        CHECK_THROWS_AS(seq.append_run(RLS_int::max_size() + 1, 7), std::length_error);
        CHECK(seq.empty());
    }
}

TEST_CASE("run_length_sequence: run_span / run")
{
    using RLS = iris::run_length_sequence<int>;
    using iv = iris::interval<unsigned>;

    RLS seq;
    seq.append_run(3, 10); // [0, 3)
    seq.append_run(1, 20); // [3, 4)
    seq.append_run(4, 30); // [4, 8)

    for (auto pos = 0u; pos < 3; ++pos) {
        auto const it = seq.nth(pos);
        CHECK(it.run_span() == iv{0, 3});
        CHECK(it.run().span == iv{0, 3});
        CHECK(it.run().value == 10);
    }
    {
        auto const it = seq.nth(3);
        CHECK(it.run_span() == iv{3, 4});
        CHECK(it.run().value == 20);
    }
    for (auto pos = 4u; pos < 8; ++pos) {
        auto const it = seq.nth(pos);
        CHECK(it.run_span() == iv{4, 8});
        CHECK(it.run().value == 30);
    }

    {
        auto it = seq.begin();
        CHECK(it.run_span() == iv{0, 3});
        it.advance_run();
        CHECK(it.run_span() == iv{3, 4});
        it.advance_run();
        CHECK(it.run_span() == iv{4, 8});
        it.advance_run();
        CHECK(it == seq.end());
    }
}

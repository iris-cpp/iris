// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/run_length_sequence.hpp>

#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <ranges>

TEST_CASE("run_length_sequence")
{
    using namespace std::string_view_literals;

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

    RLS seq;
    CHECK(seq.empty());
    CHECK(seq.size() == 0);  // NOLINT(readability-container-size-empty)
    CHECK(seq.begin() == seq.end());
    CHECK(seq == seq);

    CHECK_NOTHROW((void)(seq.cbegin() = seq.begin()));

    {
        auto&& elem = seq.emplace_back(1.0);
        REQUIRE(seq.size() == 1);
        REQUIRE(seq.run_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == 1.0);
        CHECK(seq.front() == elem);
        CHECK(seq.back() == elem);

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

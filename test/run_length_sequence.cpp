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

    using List = iris::run_length_sequence<double>;
    STATIC_CHECK(std::same_as<List::item_type, double>);
    STATIC_CHECK(std::same_as<List::index_type, unsigned>);

    STATIC_CHECK(std::same_as<List::value_type, iris::indexed_value<unsigned, double>>);
    STATIC_CHECK(std::is_trivially_copyable_v<List::value_type>);

    STATIC_CHECK(std::same_as<List::value_type, std::iter_value_t<List::iterator>>);
    STATIC_CHECK(std::same_as<List::reference, std::iter_reference_t<List::iterator>>);
    STATIC_CHECK(std::same_as<List::const_reference, std::iter_reference_t<List::const_iterator>>);

    STATIC_CHECK(std::indirectly_readable<List::iterator>);
    STATIC_CHECK(std::bidirectional_iterator<List::iterator>);

    STATIC_CHECK(std::indirectly_readable<List::const_iterator>);
    STATIC_CHECK(std::bidirectional_iterator<List::const_iterator>);

    STATIC_CHECK(std::ranges::bidirectional_range<List>);
    STATIC_CHECK(std::ranges::bidirectional_range<List const>);

    STATIC_CHECK(std::sized_sentinel_for<List::const_iterator, List::iterator>);

    STATIC_CHECK(std::default_initializable<List>);
    STATIC_CHECK(std::is_copy_constructible_v<List>);
    STATIC_CHECK(std::is_copy_assignable_v<List>);
    STATIC_CHECK(std::is_move_constructible_v<List>);
    STATIC_CHECK(std::is_move_assignable_v<List>);

    {
        List::value_type iv{};
        auto&& [index, value] = iv;
        index = 42;
        value = 1.0;
        CHECK(iv.index == 42);
        CHECK(iv.value == 1.0);
    }
    {
        List::value_type iv{};
        auto [index, value] = iv;
        index = 42;
        value = 1.0;
        CHECK(iv.index == 0);
        CHECK(iv.value == 0.0);  // NOLINT(readability-container-size-empty)
    }

    List list;
    CHECK(list.empty());
    CHECK(list.size() == 0);  // NOLINT(readability-container-size-empty)
    CHECK(list.begin() == list.end());
    CHECK(list == list);
    CHECK((list <=> list) == std::strong_ordering::equal);

    CHECK_NOTHROW((void)(list.cbegin() = list.begin()));

    {
        auto&& elem = list.emplace_back(1.0);
        REQUIRE(list.size() == 1);
        REQUIRE(list.segment_count() == 1);
        CHECK(elem.index == 0);
        CHECK(elem.value == 1.0);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it == list.end());
    }
    {
        auto&& elem = list.emplace_back(1.0);
        REQUIRE(list.size() == 2);
        REQUIRE(list.segment_count() == 1);
        CHECK(elem.index == 1);
        CHECK(elem.value == 1.0);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it == list.end());
    }
    {
        auto&& elem = list.emplace_back(1.1);
        REQUIRE(list.size() == 3);
        REQUIRE(list.segment_count() == 2);
        CHECK(elem.index == 2);
        CHECK(elem.value == 1.1);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it == list.end());
    }
    {
        auto&& elem = list.emplace_back(1.1);
        REQUIRE(list.size() == 4);
        REQUIRE(list.segment_count() == 2);
        CHECK(elem.index == 3);
        CHECK(elem.value == 1.1);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it == list.end());
    }
    {
        auto&& elem = list.emplace_back(1.1);
        REQUIRE(list.size() == 5);
        REQUIRE(list.segment_count() == 2);
        CHECK(elem.index == 4);
        CHECK(elem.value == 1.1);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e4 = *it++;
        CHECK(e4.index == 4);
        CHECK(e4.value == 1.1);

        REQUIRE(it == list.end());
    }
    {
        auto&& elem = list.emplace_back(1.2);
        REQUIRE(list.size() == 6);
        REQUIRE(list.segment_count() == 3);
        CHECK(elem.index == 5);
        CHECK(elem.value == 1.2);

        auto it = list.begin();

        REQUIRE(it != list.end());
        auto&& e0 = *it++;
        CHECK(e0.index == 0);
        CHECK(e0.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e1 = *it++;
        CHECK(e1.index == 1);
        CHECK(e1.value == 1.0);

        REQUIRE(it != list.end());
        auto&& e2 = *it++;
        CHECK(e2.index == 2);
        CHECK(e2.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e3 = *it++;
        CHECK(e3.index == 3);
        CHECK(e3.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e4 = *it++;
        CHECK(e4.index == 4);
        CHECK(e4.value == 1.1);

        REQUIRE(it != list.end());
        auto&& e5 = *it++;
        CHECK(e5.index == 5);
        CHECK(e5.value == 1.2);

        REQUIRE(it == list.end());
    }
}

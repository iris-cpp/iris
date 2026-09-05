// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/iterator.hpp>
#include <iris/ranges.hpp>
#include <iris/container_traits.hpp>

#include <format>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <array>
#include <map>
#include <unordered_set> // TODO
#include <unordered_map>
#include <flat_map>
#include <span>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include <utility>

using namespace std::string_view_literals;
using namespace std::string_literals;

TEST_CASE("container: map")
{
    STATIC_CHECK(iris::ranges::key_value_range<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::map<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::map<int, int>>);
    STATIC_CHECK(iris::container::unique_mapping_container<std::map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::multimap<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::multimap<int, int>>);
    STATIC_CHECK(!iris::container::unique_mapping_container<std::multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::container::unique_mapping_container<std::unordered_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::unordered_multimap<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::unordered_multimap<int, int>>);
    STATIC_CHECK(!iris::container::unique_mapping_container<std::unordered_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::flat_map<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::flat_map<int, int>>);
    STATIC_CHECK(iris::container::unique_mapping_container<std::flat_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::flat_multimap<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<std::flat_multimap<int, int>>);
    STATIC_CHECK(!iris::container::unique_mapping_container<std::flat_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::pair<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_range<std::vector<std::pair<int, int>>>);
    STATIC_CHECK(!iris::container::mapping_container<std::vector<std::pair<int, int>>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::tuple<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_range<std::vector<std::tuple<int, int>>>);
    STATIC_CHECK(!iris::container::mapping_container<std::vector<std::tuple<int, int>>>);
}

TEST_CASE("container: compatible range/iterator")
{
    static_assert(!std::is_constructible_v<std::vector<std::string>, std::from_range_t, std::vector<std::string_view>>);
    STATIC_CHECK(iris::container::compatible_iterator<std::vector<std::string_view>::const_iterator, std::string>);

    static_assert(std::is_constructible_v<std::vector<std::string>, std::vector<std::string_view>::const_iterator, std::vector<std::string_view>::const_iterator>);
    STATIC_CHECK(!iris::container::compatible_range<std::vector<std::string_view>, std::string>);
}

TEST_CASE("container: member traits")
{
    STATIC_CHECK(iris::container::back_pushable<std::vector<int>>);
    STATIC_CHECK(iris::container::back_pushable<std::vector<int>&>);
    STATIC_CHECK(!iris::container::back_pushable<std::vector<int> const&>);
    STATIC_CHECK(iris::container::growable_array<std::vector<int>>);
    STATIC_CHECK(iris::container::growable_array<std::vector<int>&>);
    STATIC_CHECK(!iris::container::growable_array<std::vector<int> const&>);
    STATIC_CHECK(!iris::container::fixed_array<std::vector<int>>);

    STATIC_CHECK(iris::container::appendable<std::set<int>>);
    STATIC_CHECK(iris::container::appendable<std::set<int>&>);
    STATIC_CHECK(!iris::container::appendable<std::set<int> const&>);
    STATIC_CHECK(iris::container::growable_array<std::set<int>>);
    STATIC_CHECK(iris::container::growable_array<std::set<int>&>);
    STATIC_CHECK(!iris::container::growable_array<std::set<int> const&>);
    STATIC_CHECK(!iris::container::fixed_array<std::set<int>>);

    STATIC_CHECK(iris::container::fixed_array<int[5]>);
    STATIC_CHECK(iris::container::fixed_array<int (&)[5]>);
    STATIC_CHECK(!iris::container::fixed_array<int const (&)[5]>);
    STATIC_CHECK(!iris::container::growable_array<int[5]>);

    STATIC_CHECK(iris::container::fixed_array<std::array<int, 5>>);
    STATIC_CHECK(iris::container::fixed_array<std::array<int, 5>&>);
    STATIC_CHECK(!iris::container::fixed_array<std::array<int, 5> const&>);
    STATIC_CHECK(!iris::container::growable_array<std::array<int, 5>>);

    STATIC_CHECK(iris::container::fixed_array<std::span<int, 5>>);
    STATIC_CHECK(iris::container::fixed_array<std::span<int, 5>&>);
    STATIC_CHECK(iris::container::fixed_array<std::span<int, 5> const&>);
    STATIC_CHECK(!iris::container::growable_array<std::span<int, 5>>);

    STATIC_CHECK(!iris::container::fixed_array<std::span<int const, 5>>);
    STATIC_CHECK(!iris::container::fixed_array<std::span<int const, 5>&>);
    STATIC_CHECK(!iris::container::fixed_array<std::span<int const, 5> const&>);
    STATIC_CHECK(!iris::container::growable_array<std::span<int const, 5>>);

    STATIC_CHECK(iris::container::fixed_array<std::span<int>>);
    STATIC_CHECK(iris::container::fixed_array<std::span<int>&>);
    STATIC_CHECK(iris::container::fixed_array<std::span<int> const&>);
    STATIC_CHECK(!iris::container::growable_array<std::span<int>>);
}

TEST_CASE("container: dummy types")
{
    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy::key_value_range<int, int>>);
    STATIC_CHECK(!iris::ranges::mapping_range<iris::ranges::dummy::key_value_range<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy::mapping_range<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::ranges::dummy::mapping_range<int, int>>);
    STATIC_CHECK(!iris::container::mapping_container<iris::ranges::dummy::mapping_range<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::container::dummy::mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::container::dummy::mapping_container<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<iris::container::dummy::mapping_container<int, int>>);
    STATIC_CHECK(!iris::container::unique_mapping_container<iris::container::dummy::mapping_container<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::container::dummy::unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::container::dummy::unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::container::mapping_container<iris::container::dummy::unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::container::unique_mapping_container<iris::container::dummy::unique_mapping_container<int, int>>);

    STATIC_CHECK(iris::container::growable_array<iris::container::dummy::growable_array<int>>);
    STATIC_CHECK(iris::container::fixed_array<iris::container::dummy::fixed_array<int>>);
}


struct recording_item
{
    [[nodiscard]] static recording_item make_moved(recording_item const& other)
    {
        return recording_item{"move[" + other.log + "]"};
    }

    recording_item()
    {
        log += "default";
    }

    recording_item(int a)
    {
        log += std::format("{}", a);
    }

    recording_item(int a, int b)
    {
        log += std::format("({},{})", a, b);
    }

    recording_item(recording_item const& other)
    {
        log = "copy[" + other.log + "]";
    }

    recording_item(recording_item&& other)  // NOLINT(cppcoreguidelines-noexcept-move-operations, performance-noexcept-move-constructor)
    {
        log = "move[" + other.log + "]";
    }

    recording_item& operator=(recording_item const&) = delete;
    recording_item& operator=(recording_item&&) = default;

    [[nodiscard]] bool operator==(recording_item const&) const = default;

    std::string log;

private:
    explicit recording_item(std::string override_log)
        : log(std::move(override_log))
    {}
};

template<>
struct std::formatter<recording_item> : std::formatter<std::string>
{
    auto format(recording_item const& rec, auto& ctx) const
    {
        return std::formatter<std::string>::format(rec.log, ctx);
    }
};

template<bool IsFrontTest, class T, bool HasEmplaceMeow, bool HasPushMeow, bool HasEmplace, bool HasInsert>
struct recording_container
{
    recording_container()
    {
        elems.reserve(100);
    }

    auto begin() const
    {
        if constexpr (IsFrontTest) {
            return std::reverse_iterator{elems.end()};
        } else {
            return elems.begin();
        }
    }
    auto begin()
    {
        if constexpr (IsFrontTest) {
            return std::reverse_iterator{elems.end()};
        } else {
            return elems.begin();
        }
    }
    auto end() const
    {
        if constexpr (IsFrontTest) {
            return std::reverse_iterator{elems.begin()};
        } else {
            return elems.end();
        }
    }
    auto end()
    {
        if constexpr (IsFrontTest) {
            return std::reverse_iterator{elems.begin()};
        } else {
            return elems.end();
        }
    }

    template<class... Args>
    decltype(auto) emplace_front(Args&&... args) requires IsFrontTest && HasEmplaceMeow
    {
        log += " emplace_front";
        return elems.emplace_back(std::forward<Args>(args)...);
    }

    void push_front(T const& value) requires IsFrontTest && HasPushMeow
    {
        log += " push_front";
        elems.push_back(value);
    }

    void push_front(T&& value) requires IsFrontTest && HasPushMeow
    {
        log += " push_front";
        elems.push_back(std::move(value));
    }

    template<class... Args>
    decltype(auto) emplace_back(Args&&... args) requires (!IsFrontTest) && HasEmplaceMeow
    {
        log += " emplace_back";
        return elems.emplace_back(std::forward<Args>(args)...);
    }

    void push_back(T const& value) requires (!IsFrontTest) && HasPushMeow
    {
        log += " push_back";
        elems.push_back(value);
    }

    void push_back(T&& value) requires (!IsFrontTest) && HasPushMeow
    {
        log += " push_back";
        elems.push_back(std::move(value));
    }

    template<class... Args>
    auto emplace(auto it, Args&&... args) requires HasEmplace
    {
        log += " emplace";
        if constexpr (IsFrontTest) {
            return elems.emplace(it.base(), std::forward<Args>(args)...);
        } else {
            return elems.emplace(it, std::forward<Args>(args)...);
        }
    }

    auto insert(auto it, T const& value) requires HasInsert
    {
        log += " insert";
        if constexpr (IsFrontTest) {
            return elems.insert(it.base(), value);
        } else {
            return elems.insert(it, value);
        }
    }

    auto insert(auto it, T&& value) requires HasInsert
    {
        log += " insert";
        if constexpr (IsFrontTest) {
            return elems.insert(it.base(), std::move(value));
        } else {
            return elems.insert(it, std::move(value));
        }
    }

    [[nodiscard]] std::string elems_str() const
    {
        return elems | std::views::transform([](T const& value) {
            return std::format("{}", value);
        }) | std::views::join_with("|"sv) | std::ranges::to<std::string>();
    }

    std::vector<T> elems;
    std::string log;
};

static_assert(std::ranges::range<recording_container<true, int, true, true, true, true>>);
static_assert(std::same_as<std::ranges::range_reference_t<recording_container<true, int, true, true, true, true>>, int&>);
static_assert(std::same_as<std::ranges::range_reference_t<recording_container<true, int, true, true, true, true> const>, int const&>);

TEST_CASE("container: prepend")
{
    {
        // emplace_front + push_front + emplace + insert
        using Cont = recording_container<true, recording_item, true, true, true, true>;

        STATIC_CHECK(iris::container::front_pushable<Cont>);
        STATIC_CHECK(iris::container::front_pushable<Cont, int>);
        STATIC_CHECK(iris::container::front_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont, int>);
        STATIC_CHECK(iris::container::prependable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::prepend(cont); cont_log_expected += " emplace_front"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 1); cont_log_expected += " emplace_front"; elems_str_expected += "|1";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 2, 3); cont_log_expected += " emplace_front"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::prepend_return(cont); cont_log_expected += " emplace_front"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 4); cont_log_expected += " emplace_front"; elems_str_expected += "|4";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{4});
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 5, 6); cont_log_expected += " emplace_front"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // push_front + emplace + insert
        using Cont = recording_container<true, recording_item, false, true, true, true>;

        STATIC_CHECK(!iris::container::front_pushable<Cont>);
        STATIC_CHECK(iris::container::front_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont, int>);
        STATIC_CHECK(iris::container::prependable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::prepend(cont); cont_log_expected += " emplace"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 1); cont_log_expected += " push_front"; elems_str_expected += "|move[1]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 2, 3); cont_log_expected += " emplace"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::prepend_return(cont); cont_log_expected += " emplace"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 4); cont_log_expected += " push_front"; elems_str_expected += "|move[4]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{4}));
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 5, 6); cont_log_expected += " emplace"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // emplace + insert
        using Cont = recording_container<true, recording_item, false, false, true, true>;

        STATIC_CHECK(!iris::container::front_pushable<Cont>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont, int>);
        STATIC_CHECK(iris::container::prependable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::prepend(cont); cont_log_expected += " emplace"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 1); cont_log_expected += " emplace"; elems_str_expected += "|1";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 2, 3); cont_log_expected += " emplace"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::prepend_return(cont); cont_log_expected += " emplace"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 4); cont_log_expected += " emplace"; elems_str_expected += "|4";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{4});
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 5, 6); cont_log_expected += " emplace"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // insert
        using Cont = recording_container<true, recording_item, false, false, false, true>;

        STATIC_CHECK(!iris::container::front_pushable<Cont>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont>);
        STATIC_CHECK(iris::container::prependable<Cont, int>);
        STATIC_CHECK(!iris::container::prependable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::prepend_return), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::prepend(cont); cont_log_expected += " insert"; elems_str_expected += "move[default]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::prepend(cont, 1); cont_log_expected += " insert"; elems_str_expected += "|move[1]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::prepend_return(cont); cont_log_expected += " insert"; elems_str_expected += "|move[default]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{}));
        }
        {
            auto&& elem = iris::container::prepend_return(cont, 4); cont_log_expected += " insert"; elems_str_expected += "|move[4]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{4}));
        }
    }
    {
        // no accessor
        using Cont = recording_container<true, recording_item, false, false, false, false>;

        STATIC_CHECK(!iris::container::front_pushable<Cont>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::front_pushable<Cont, int, int>);

        STATIC_CHECK(!iris::container::default_prependable<Cont>);
        STATIC_CHECK(!iris::container::prependable<Cont>);
        STATIC_CHECK(!iris::container::prependable<Cont, int>);
        STATIC_CHECK(!iris::container::prependable<Cont, int, int>);

        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend), Cont&>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend), Cont&, int, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend_return), Cont&>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend_return), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::prepend_return), Cont&, int, int>);
    }
}

TEST_CASE("container: append")
{
    {
        // emplace_back + push_back + emplace + insert
        using Cont = recording_container<false, recording_item, true, true, true, true>;

        STATIC_CHECK(iris::container::back_pushable<Cont>);
        STATIC_CHECK(iris::container::back_pushable<Cont, int>);
        STATIC_CHECK(iris::container::back_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont, int>);
        STATIC_CHECK(iris::container::appendable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::append(cont); cont_log_expected += " emplace_back"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 1); cont_log_expected += " emplace_back"; elems_str_expected += "|1";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 2, 3); cont_log_expected += " emplace_back"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::append_return(cont); cont_log_expected += " emplace_back"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::append_return(cont, 4); cont_log_expected += " emplace_back"; elems_str_expected += "|4";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{4});
        }
        {
            auto&& elem = iris::container::append_return(cont, 5, 6); cont_log_expected += " emplace_back"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // push_back + emplace + insert
        using Cont = recording_container<false, recording_item, false, true, true, true>;

        STATIC_CHECK(!iris::container::back_pushable<Cont>);
        STATIC_CHECK(iris::container::back_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont, int>);
        STATIC_CHECK(iris::container::appendable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::append(cont); cont_log_expected += " emplace"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 1); cont_log_expected += " push_back"; elems_str_expected += "|move[1]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 2, 3); cont_log_expected += " emplace"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::append_return(cont); cont_log_expected += " emplace"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::append_return(cont, 4); cont_log_expected += " push_back"; elems_str_expected += "|move[4]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{4}));
        }
        {
            auto&& elem = iris::container::append_return(cont, 5, 6); cont_log_expected += " emplace"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // emplace + insert
        using Cont = recording_container<false, recording_item, false, false, true, true>;

        STATIC_CHECK(!iris::container::back_pushable<Cont>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont, int>);
        STATIC_CHECK(iris::container::appendable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::append(cont); cont_log_expected += " emplace"; elems_str_expected += "default";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 1); cont_log_expected += " emplace"; elems_str_expected += "|1";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 2, 3); cont_log_expected += " emplace"; elems_str_expected += "|(2,3)";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::append_return(cont); cont_log_expected += " emplace"; elems_str_expected += "|default";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{});
        }
        {
            auto&& elem = iris::container::append_return(cont, 4); cont_log_expected += " emplace"; elems_str_expected += "|4";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{4});
        }
        {
            auto&& elem = iris::container::append_return(cont, 5, 6); cont_log_expected += " emplace"; elems_str_expected += "|(5,6)";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item{5, 6});
        }
    }
    {
        // insert
        using Cont = recording_container<false, recording_item, false, false, false, true>;

        STATIC_CHECK(!iris::container::back_pushable<Cont>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int, int>);

        STATIC_CHECK(iris::container::default_appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont>);
        STATIC_CHECK(iris::container::appendable<Cont, int>);
        STATIC_CHECK(!iris::container::appendable<Cont, int, int>);

        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append), Cont&, int, int>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&>);
        STATIC_CHECK(std::invocable<decltype(iris::container::append_return), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append_return), Cont&, int, int>);

        Cont cont;
        std::string elems_str_expected;
        std::string cont_log_expected;

        iris::container::append(cont); cont_log_expected += " insert"; elems_str_expected += "move[default]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        iris::container::append(cont, 1); cont_log_expected += " insert"; elems_str_expected += "|move[1]";
        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        CHECK(cont.log == cont_log_expected);
        REQUIRE(cont.elems_str() == elems_str_expected);

        {
            auto&& elem = iris::container::append_return(cont); cont_log_expected += " insert"; elems_str_expected += "|move[default]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{}));
        }
        {
            auto&& elem = iris::container::append_return(cont, 4); cont_log_expected += " insert"; elems_str_expected += "|move[4]";
            CHECK(cont.log == cont_log_expected);
            REQUIRE(cont.elems_str() == elems_str_expected);
            CHECK(elem == recording_item::make_moved(recording_item{4}));
        }
    }
    {
        // no accessor
        using Cont = recording_container<false, recording_item, false, false, false, false>;

        STATIC_CHECK(!iris::container::back_pushable<Cont>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int>);
        STATIC_CHECK(!iris::container::back_pushable<Cont, int, int>);

        STATIC_CHECK(!iris::container::default_appendable<Cont>);
        STATIC_CHECK(!iris::container::appendable<Cont>);
        STATIC_CHECK(!iris::container::appendable<Cont, int>);
        STATIC_CHECK(!iris::container::appendable<Cont, int, int>);

        STATIC_CHECK(!std::invocable<decltype(iris::container::append), Cont&>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append), Cont&, int, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append_return), Cont&>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append_return), Cont&, int>);
        STATIC_CHECK(!std::invocable<decltype(iris::container::append_return), Cont&, int, int>);
    }
}

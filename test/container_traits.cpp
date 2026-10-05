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
#include <flat_set>
#include <list>
#include <memory>
#include <memory_resource>
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

TEST_CASE("container: append into associative containers")
{
    // `emplace(it, x)` of an associative container is a valid expression but not a valid call
    STATIC_CHECK(iris::container::appendable<std::set<int>, int>);
    STATIC_CHECK(iris::container::growable_array<std::map<int, int>>);

    std::set<int> s;
    iris::container::append(s, 2);
    iris::container::append(s, 1);
    CHECK(s == std::set<int>{1, 2});

    std::map<int, int> m;
    iris::container::append(m, std::pair{1, 10});
    CHECK(m == std::map<int, int>{{1, 10}});

    std::map<std::string, std::string> strings{{"k", "a"}};
    std::pair<std::string, std::string> duplicate{"k", "b"};
    iris::container::append(strings, std::move(duplicate));
    CHECK(strings == std::map<std::string, std::string>{{"k", "a"}});
    CHECK(duplicate == std::pair<std::string, std::string>{"k", "b"}); // NOLINT(bugprone-use-after-move)
    CHECK(&iris::container::append_return(strings, std::pair<std::string, std::string>{"k", "c"}) == &*strings.begin());

    std::flat_map<std::string, std::string> flat{{"k", "a"}};
    iris::container::append(flat, std::move(duplicate));
    CHECK(flat == std::flat_map<std::string, std::string>{{"k", "a"}});
    CHECK(duplicate == std::pair<std::string, std::string>{"k", "b"}); // NOLINT(bugprone-use-after-move)

    std::map<std::string, int> converted;
    iris::container::append(converted, std::pair{"k", 1});
    CHECK(converted == std::map<std::string, int>{{"k", 1}});
}

TEST_CASE("container: append_range")
{
    std::vector<std::string> source{"a", "b"};

    std::vector<std::string> v{"x"};
    iris::container::append_range(v, source);
    CHECK(v == std::vector<std::string>{"x", "a", "b"});

    std::vector<std::string> moved{"x"};
    iris::container::append_range(moved, source | std::views::as_rvalue);
    CHECK(moved == std::vector<std::string>{"x", "a", "b"});
    CHECK(source == std::vector<std::string>{"", ""});

    std::string str = "x";
    iris::container::append_range(str, "ab"sv);
    CHECK(str == "xab");

    std::set<int> s{3};
    iris::container::append_range(s, std::vector{2, 1});
    CHECK(s == std::set<int>{1, 2, 3});

    std::map<int, int> m{{1, 10}};
    iris::container::append_range(m, std::vector<std::pair<int, int>>{{2, 20}});
    CHECK(m == std::map<int, int>{{1, 10}, {2, 20}});

    // `std::vector<int>(std::size_t)` is not a conversion
    STATIC_CHECK(!std::invocable<decltype(iris::container::append_range), std::vector<std::vector<int>>&, std::vector<int>>);
    STATIC_CHECK(!std::invocable<decltype(iris::container::append_range), std::array<int, 1>&, std::vector<int>>);
}

template<class T>
struct tagged_allocator
{
    using value_type = T;
    using propagate_on_container_move_assignment = std::true_type;

    int tag = 0;

    tagged_allocator() = default;

    explicit tagged_allocator(int tag) noexcept : tag(tag) {}

    template<class U>
    tagged_allocator(tagged_allocator<U> const& other) noexcept : tag(other.tag) {} // NOLINT(misc-explicit-constructor)

    T* allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }
    void deallocate(T* p, std::size_t n) noexcept { std::allocator<T>{}.deallocate(p, n); }

    template<class U>
    bool operator==(tagged_allocator<U> const& other) const noexcept { return tag == other.tag; }
};

struct stateful_less
{
    bool is_reversed = false;
    bool operator()(int a, int b) const { return is_reversed ? b < a : a < b; }
};

TEST_CASE("container: transfer_from")
{
    using iris::container::transfer_from;

    {
        std::list<std::string> dst{"a"}, src{"b", "c"};
        auto const* const node = &src.front();
        transfer_from(dst, src);
        CHECK(dst == std::list<std::string>{"a", "b", "c"});
        CHECK(&*std::next(dst.begin()) == node);
        CHECK(src.empty());
    }
    {
        std::map<int, std::string> dst{{1, "a"}};
        std::multimap<int, std::string> src{{1, "b"}, {2, "c"}};
        auto const* const node = &src.find(2)->second;
        transfer_from(dst, src);
        CHECK(dst == std::map<int, std::string>{{1, "a"}, {2, "c"}});
        CHECK(&dst.at(2) == node);
        CHECK(src == std::multimap<int, std::string>{{1, "b"}});
    }
    {
        std::unordered_map<int, int> dst{{1, 10}};
        transfer_from(dst, std::unordered_map<int, int>{{1, 20}, {2, 30}});
        CHECK(dst == std::unordered_map<int, int>{{1, 10}, {2, 30}});
    }

    {
        std::vector<std::string> dst{"a"}, src{"b"};
        transfer_from(dst, src);
        CHECK(dst == std::vector<std::string>{"a", "b"});
        CHECK(src == std::vector<std::string>{""});
    }
    {
        std::map<int, std::string> dst{{1, "a"}};
        std::vector<std::pair<int, std::string>> src{{1, "b"}, {2, "c"}};
        transfer_from(dst, src);
        CHECK(dst == std::map<int, std::string>{{1, "a"}, {2, "c"}});
    }
    {
        // Nodes are not moved between different memory resources
        std::pmr::monotonic_buffer_resource dst_resource, src_resource;
        std::pmr::list<int> dst(&dst_resource), src({1, 2}, &src_resource);
        auto const* const node = &src.front();
        transfer_from(dst, src);
        CHECK(dst == std::pmr::list<int>{1, 2});
        CHECK(&dst.front() != node);
        CHECK(dst.get_allocator().resource() == &dst_resource);

        std::pmr::list<int> same_resource_src({3}, &dst_resource);
        auto const* const same_resource_node = &same_resource_src.front();
        transfer_from(dst, same_resource_src);
        CHECK(&dst.back() == same_resource_node);
    }

    // flat containers
    {
        std::flat_map<std::string, int> dst, src{{"a", 1}, {"b", 2}};
        auto const* const keys = src.keys().data();
        transfer_from(dst, src);
        CHECK(dst == std::flat_map<std::string, int>{{"a", 1}, {"b", 2}});
        CHECK(dst.keys().data() == keys);
        CHECK(src.empty());

        std::flat_map<std::string, int> more{{"a", 3}, {"c", 4}};
        transfer_from(dst, more);
        CHECK(dst == std::flat_map<std::string, int>{{"a", 1}, {"b", 2}, {"c", 4}});
    }
    {
        std::flat_set<std::string> dst, src{"b", "a"};
        transfer_from(dst, src);
        CHECK(dst == std::flat_set<std::string>{"a", "b"});

        transfer_from(dst, std::flat_set<std::string>{"c", "a"});
        CHECK(dst == std::flat_set<std::string>{"a", "b", "c"});
    }
    {
        std::flat_multimap<int, int> dst{{1, 10}};
        transfer_from(dst, std::flat_multimap<int, int>{{1, 20}, {2, 30}});
        CHECK(dst == std::flat_multimap<int, int>{{1, 10}, {1, 20}, {2, 30}});
    }
    {
        // A flat container yields its mapped values as lvalues, so they are moved out of the underlying containers
        std::map<std::string, std::unique_ptr<int>> dst;
        std::flat_map<std::string, std::unique_ptr<int>> src;
        src.emplace("a", std::make_unique<int>(1));
        transfer_from(dst, src);
        REQUIRE(dst.contains("a"));
        CHECK(*dst.at("a") == 1);
    }
    {
        // The destination keeps the memory resource of its underlying containers
        using pmr_flat_map = std::flat_map<int, int, std::less<>, std::pmr::vector<int>, std::pmr::vector<int>>;
        std::pmr::monotonic_buffer_resource dst_resource, src_resource;
        pmr_flat_map dst{std::pmr::polymorphic_allocator<int>(&dst_resource)}, src{std::pmr::polymorphic_allocator<int>(&src_resource)};
        src.emplace(2, 20);
        src.emplace(1, 10);
        transfer_from(dst, src);
        CHECK(dst == pmr_flat_map{{1, 10}, {2, 20}});
        CHECK(dst.keys().get_allocator().resource() == &dst_resource);

        pmr_flat_map more{std::pmr::polymorphic_allocator<int>(&src_resource)};
        more.emplace(0, 0);
        more.emplace(2, 200);
        transfer_from(dst, more);
        CHECK(dst == pmr_flat_map{{0, 0}, {1, 10}, {2, 20}});
        CHECK(dst.values().get_allocator().resource() == &dst_resource);
    }
    {
        // An empty destination takes the buffers of a source that has an equal memory resource
        using pmr_flat_set = std::flat_set<int, std::less<>, std::pmr::vector<int>>;
        std::pmr::monotonic_buffer_resource resource;
        pmr_flat_set dst{std::pmr::polymorphic_allocator<int>(&resource)}, src{std::pmr::polymorphic_allocator<int>(&resource)};
        src.insert(1);
        auto const* const element = &*src.begin();
        transfer_from(dst, src);
        CHECK(&*dst.begin() == element);
    }
    {
        // An allocator that propagates on move assignment would replace the allocator of an empty destination
        using tagged_flat_set = std::flat_set<int, std::less<>, std::vector<int, tagged_allocator<int>>>;
        tagged_flat_set dst{tagged_allocator<int>(1)}, src{tagged_allocator<int>(2)};
        src.insert(1);
        transfer_from(dst, src);
        CHECK(dst == tagged_flat_set{1});
        CHECK(std::move(dst).extract().get_allocator().tag == 1);
    }
    {
        // The source is sorted by its own comparator, which the destination cannot rely on
        std::flat_set<int, stateful_less> dst(stateful_less{false}), src({1, 2, 3}, stateful_less{true});
        transfer_from(dst, src);
        CHECK(std::ranges::equal(dst, std::vector{1, 2, 3}));
    }

    STATIC_CHECK(!std::invocable<decltype(transfer_from), std::vector<int>&, std::vector<std::string>&>);
    STATIC_CHECK(!std::invocable<decltype(transfer_from), std::array<int, 1>&, std::vector<int>&>);
}

TEST_CASE("container: clear")
{
    std::vector<int> v{1, 2};
    iris::container::clear(v);
    CHECK(v.empty());

    STATIC_CHECK(!std::invocable<decltype(iris::container::clear), std::array<int, 1>&>);
    STATIC_CHECK(!std::invocable<decltype(iris::container::clear), std::vector<int> const&>);
}

TEST_CASE("container: element_t")
{
    STATIC_CHECK(std::same_as<iris::container::element_t<std::vector<int>>, int>);
    STATIC_CHECK(std::same_as<iris::container::element_t<std::map<int, std::string>>, std::pair<int, std::string>>);
    STATIC_CHECK(std::same_as<iris::container::element_t<std::set<int>>, int>);
}

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/ranges.hpp>

#include <vector>
#include <map>
#include <unordered_map>
#include <flat_map>
#include <set>
#include <utility>
#include <span>
#include <array>

template<class KeyT, class ValueT>
struct FakeNonRangeMap
{
    using key_type = KeyT;
    using value_type = std::pair<KeyT const, ValueT>;
    using mapped_type = ValueT;
};

template<class KeyT, class ValueT>
struct FakeRangeMapWithoutMappedType
{
    using key_type = KeyT;
    using value_type = std::pair<KeyT const, ValueT>;

    value_type const* begin() const;
    value_type const* end() const;
};

template<class KeyT, class ValueT>
struct RangeMapWith2TupleSize
{
    using key_type = KeyT;
    using value_type = std::tuple<KeyT const, ValueT>; // NOT pair
    using mapped_type = ValueT;

    value_type const* begin() const;
    value_type const* end() const;
};

template<class KeyT, class ValueT>
struct FakeRangeMapWithWrongTupleSize
{
    using key_type = KeyT;
    using value_type = std::tuple<KeyT const, ValueT, char>; // additional element
    using mapped_type = ValueT;

    value_type const* begin() const;
    value_type const* end() const;
};

template<class KeyT, class ValueT>
struct FakeRangeMapWithNonTupleKey
{
    using key_type = KeyT;
    using value_type = char; // non-tuple
    using mapped_type = ValueT;

    value_type const* begin() const;
    value_type const* end() const;
};

TEST_CASE("ranges: map related traits")
{
    using iris::ranges::as_map;

    STATIC_CHECK(iris::ranges::key_value_range<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_mapping_container<std::map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_mapping_container<std::multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_mapping_container<std::unordered_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::unordered_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::unordered_multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_mapping_container<std::unordered_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_mapping_container<std::flat_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<std::flat_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<std::flat_multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_mapping_container<std::flat_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::pair<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_range<std::vector<std::pair<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_container<std::vector<std::pair<int, int>>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::tuple<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_range<std::vector<std::tuple<int, int>>>);
    STATIC_CHECK(!iris::ranges::mapping_container<std::vector<std::tuple<int, int>>>);

    // ---------------------------------------------------
    // Viewed maps
    {
        [[maybe_unused]] constexpr auto filter = std::views::filter([](auto&&) { return true; });
        {
            using Map = std::map<int, int>;

            // Map
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<Map>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<Map>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<Map>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<Map>() | filter | as_map)>);

            // Map&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<Map&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<Map&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<Map&>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<Map&>() | filter | as_map)>);

            // Map const&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<Map const&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<Map const&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<Map const&>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<Map const&>() | filter | as_map)>);
        }
        {
            using PairVec = std::vector<std::pair<int, int>>;

            // Map
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec>())>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec>())>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec>() | as_map)>);
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<PairVec>() | filter | as_map)>);

            // Map&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec&>())>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec&>())>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | as_map)>);
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<PairVec&>() | filter | as_map)>);

            // Map const&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec const&>())>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec const&>())>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | as_map)>);
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec const&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | filter | as_map)>);
            STATIC_CHECK(!iris::ranges::mapping_container<decltype(std::declval<PairVec const&>() | filter | as_map)>);
        }
    }

    // ---------------------------------------------------
    // Exotic types

    STATIC_CHECK(!iris::ranges::key_value_range<FakeNonRangeMap<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithoutMappedType<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_range<FakeRangeMapWithoutMappedType<int, int>>);

    static_assert(std::ranges::forward_range<RangeMapWith2TupleSize<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_range<RangeMapWith2TupleSize<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithWrongTupleSize<int, int>>);
    STATIC_CHECK(!iris::ranges::key_value_range<FakeRangeMapWithWrongTupleSize<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithNonTupleKey<int, int>>);
    STATIC_CHECK(!iris::ranges::key_value_range<FakeRangeMapWithNonTupleKey<int, int>>);

    STATIC_CHECK(!iris::ranges::key_value_range<std::vector<int>>);
    STATIC_CHECK(!iris::ranges::key_value_range<std::set<int>>);
}

TEST_CASE("ranges: container related traits")
{
    STATIC_CHECK(iris::ranges::back_emplaceable<std::vector<int>>);
    STATIC_CHECK(iris::ranges::back_emplaceable<std::vector<int>&>);
    STATIC_CHECK(!iris::ranges::back_emplaceable<std::vector<int> const&>);
    STATIC_CHECK(iris::ranges::growable_array_writable<std::vector<int>>);
    STATIC_CHECK(iris::ranges::growable_array_writable<std::vector<int>&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::vector<int> const&>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::vector<int>>);

    STATIC_CHECK(iris::ranges::emplaceable<std::set<int>>);
    STATIC_CHECK(iris::ranges::emplaceable<std::set<int>&>);
    STATIC_CHECK(!iris::ranges::emplaceable<std::set<int> const&>);
    STATIC_CHECK(iris::ranges::growable_array_writable<std::set<int>>);
    STATIC_CHECK(iris::ranges::growable_array_writable<std::set<int>&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::set<int> const&>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::set<int>>);

    STATIC_CHECK(iris::ranges::fixed_array_writable<int[5]>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<int (&)[5]>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<int const (&)[5]>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<int[5]>);

    STATIC_CHECK(iris::ranges::fixed_array_writable<std::array<int, 5>>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<std::array<int, 5>&>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::array<int, 5> const&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::array<int, 5>>);

    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int, 5>>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int, 5>&>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int, 5> const&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::span<int, 5>>);

    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::span<int const, 5>>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::span<int const, 5>&>);
    STATIC_CHECK(!iris::ranges::fixed_array_writable<std::span<int const, 5> const&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::span<int const, 5>>);

    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int>>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int>&>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<std::span<int> const&>);
    STATIC_CHECK(!iris::ranges::growable_array_writable<std::span<int>>);
}

TEST_CASE("ranges: dummy types")
{
    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy_key_value_range<int, int>>);
    STATIC_CHECK(!iris::ranges::mapping_range<iris::ranges::dummy_key_value_range<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy_mapping_range<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::ranges::dummy_mapping_range<int, int>>);
    STATIC_CHECK(!iris::ranges::mapping_container<iris::ranges::dummy_mapping_range<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::ranges::dummy_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<iris::ranges::dummy_mapping_container<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_mapping_container<iris::ranges::dummy_mapping_container<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<iris::ranges::dummy_unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_range<iris::ranges::dummy_unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::mapping_container<iris::ranges::dummy_unique_mapping_container<int, int>>);
    STATIC_CHECK(iris::ranges::unique_mapping_container<iris::ranges::dummy_unique_mapping_container<int, int>>);

    STATIC_CHECK(iris::ranges::growable_array_writable<iris::ranges::dummy_growable_array<int>>);
    STATIC_CHECK(iris::ranges::fixed_array_writable<iris::ranges::dummy_fixed_array<int>>);
}

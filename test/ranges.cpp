// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/iterator.hpp>
#include <iris/ranges.hpp>
#include <iris/container_traits.hpp>

#include <tuple>
#include <vector>
#include <map>
#include <set>
#include <utility>

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

TEST_CASE("ranges: as_map")
{
    using iris::ranges::as_map;

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
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<Map>() | filter | as_map)>);

            // Map&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<Map&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<Map&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<Map&>() | filter | as_map)>);
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<Map&>() | filter | as_map)>);

            // Map const&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<Map const&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<Map const&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<Map const&>() | filter | as_map)>);
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<Map const&>() | filter | as_map)>);
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
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<PairVec>() | filter | as_map)>);

            // Map&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec&>())>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec&>())>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | as_map)>);
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec&>() | filter | as_map)>);
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<PairVec&>() | filter | as_map)>);

            // Map const&
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec const&>())>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec const&>())>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | as_map)>);
            STATIC_CHECK(iris::ranges::key_value_range<decltype(std::declval<PairVec const&>() | filter)>);
            STATIC_CHECK(!iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | filter)>);
            STATIC_CHECK(iris::ranges::mapping_range<decltype(std::declval<PairVec const&>() | filter | as_map)>);
            STATIC_CHECK(!iris::container::mapping_container<decltype(std::declval<PairVec const&>() | filter | as_map)>);
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

// Test cases involving containers must go to `container_traits.cpp`
// Test cases for dummy ranges / dummy containers must go to `container_traits.cpp`

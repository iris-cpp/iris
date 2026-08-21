#include "iris_test.hpp"

#include <iris/ranges.hpp>

#include <vector>
#include <map>
#include <unordered_map>
#include <flat_map>
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

TEST_CASE("ranges: map related traits")
{
    STATIC_CHECK(iris::ranges::key_value_range<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_key_value_container<std::map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::multimap<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_key_value_container<std::multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_key_value_container<std::unordered_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::unordered_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::unordered_multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_key_value_container<std::unordered_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::flat_map<int, int>>);
    STATIC_CHECK(iris::ranges::unique_key_value_container<std::flat_map<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::flat_multimap<int, int>>);
    STATIC_CHECK(iris::ranges::key_value_container<std::flat_multimap<int, int>>);
    STATIC_CHECK(!iris::ranges::unique_key_value_container<std::flat_multimap<int, int>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::pair<int, int>>>);
    STATIC_CHECK(!iris::ranges::key_value_container<std::vector<std::pair<int, int>>>);

    STATIC_CHECK(iris::ranges::key_value_range<std::vector<std::tuple<int, int>>>);
    STATIC_CHECK(!iris::ranges::key_value_container<std::vector<std::tuple<int, int>>>);

    // ---------------------------------------------------
    // Viewed maps
    {
        constexpr auto filter = std::views::filter([](auto&&) { return true; });
        {
            std::map<int, int> map;
            [[maybe_unused]] auto filtered_map_view = map | filter;
            using FilteredMapView = decltype(filtered_map_view);
            STATIC_CHECK(iris::ranges::key_value_range<FilteredMapView>);
            STATIC_CHECK(!iris::ranges::key_value_container<FilteredMapView>);
        }
        {
            std::multimap<int, int> map;
            [[maybe_unused]] auto filtered_map_view = map | filter;
            using FilteredMapView = decltype(filtered_map_view);
            STATIC_CHECK(iris::ranges::key_value_range<FilteredMapView>);
            STATIC_CHECK(!iris::ranges::key_value_container<FilteredMapView>);
        }
        {
            std::unordered_map<int, int> map;
            [[maybe_unused]] auto filtered_map_view = map | filter;
            using FilteredMapView = decltype(filtered_map_view);
            STATIC_CHECK(iris::ranges::key_value_range<FilteredMapView>);
            STATIC_CHECK(!iris::ranges::key_value_container<FilteredMapView>);
        }
        {
            std::flat_map<int, int> map;
            [[maybe_unused]] auto filtered_map_view = map | filter;
            using FilteredMapView = decltype(filtered_map_view);
            STATIC_CHECK(iris::ranges::key_value_range<FilteredMapView>);
            STATIC_CHECK(!iris::ranges::key_value_container<FilteredMapView>);
        }

        {
            std::vector<std::pair<int, int>> vector_pair;
            [[maybe_unused]] auto vector_pair_view = vector_pair | std::views::all;
            using VectorPairView = decltype(vector_pair_view);
            STATIC_CHECK(iris::ranges::key_value_range<VectorPairView>);
            STATIC_CHECK(!iris::ranges::key_value_container<VectorPairView>);
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

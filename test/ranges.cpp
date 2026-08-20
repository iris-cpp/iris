#include "iris_test.hpp"

#include <iris/ranges.hpp>

#include <vector>
#include <map>
#include <unordered_map>
#include <flat_map>
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

TEST_CASE("ranges")
{
    STATIC_CHECK(!iris::is_assoc_container_v<std::vector<int>>);
    STATIC_CHECK(iris::is_assoc_container_v<std::map<int, int>>);
    STATIC_CHECK(iris::is_assoc_container_v<std::unordered_map<int, int>>);
    STATIC_CHECK(iris::is_assoc_container_v<std::flat_map<int, int>>);

    STATIC_CHECK(!iris::is_assoc_container_v<FakeNonRangeMap<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithoutMappedType<int, int>>);
    STATIC_CHECK(!iris::is_assoc_container_v<FakeRangeMapWithoutMappedType<int, int>>);

    static_assert(std::ranges::forward_range<RangeMapWith2TupleSize<int, int>>);
    STATIC_CHECK(iris::is_assoc_container_v<RangeMapWith2TupleSize<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithWrongTupleSize<int, int>>);
    STATIC_CHECK(!iris::is_assoc_container_v<FakeRangeMapWithWrongTupleSize<int, int>>);

    static_assert(std::ranges::forward_range<FakeRangeMapWithNonTupleKey<int, int>>);
    STATIC_CHECK(!iris::is_assoc_container_v<FakeRangeMapWithNonTupleKey<int, int>>);
}

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/indexed_value.hpp>

#include <functional>
#include <string>
#include <concepts>
#include <type_traits>

using iris::indexed_value;

TEST_CASE("indexed_value")
{
    using V =   indexed_value<int, std::string>;
    //using CV =  indexed_value<int, std::string const>;
    using LR =  indexed_value<int, std::string&>;
    using CLR = indexed_value<int, std::string const&>;
    using RR =  indexed_value<int, std::string&&>;
    using CRR = indexed_value<int, std::string const&&>;

    // ---------------------------------------------------

    STATIC_CHECK(std::is_aggregate_v<V>);
    STATIC_CHECK(std::is_aggregate_v<LR>);

    STATIC_CHECK(std::is_trivially_copyable_v<indexed_value<int, int>>);
    STATIC_CHECK(std::is_trivially_copyable_v<indexed_value<int, std::string&>>);
    STATIC_CHECK(!std::is_trivially_copyable_v<indexed_value<int, std::string>>);

    // ---------------------------------------------------

    STATIC_CHECK(std::same_as<std::common_type_t<LR, LR>,   V>);
    STATIC_CHECK(std::same_as<std::common_type_t<CLR, CLR>, V>);
    STATIC_CHECK(std::same_as<std::common_type_t<RR, RR>,   V>);
    STATIC_CHECK(std::same_as<std::common_type_t<CRR, CRR>, V>);

    // ---------------------------------------------------

    STATIC_CHECK(std::same_as<std::common_reference_t<LR&&, V&>,     std::common_reference_t<V&, LR&&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<LR&&, RR&&>,   std::common_reference_t<RR&&, LR&&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<CLR&&, CRR&&>, std::common_reference_t<CRR&&, CLR&&>>);

    STATIC_CHECK(std::common_reference_with<LR&&, V&>);
    STATIC_CHECK(std::common_reference_with<LR&&, RR&&>);
    STATIC_CHECK(std::common_reference_with<RR&&, V const&>);
    STATIC_CHECK(std::same_as<std::common_reference_t<LR&&, V&>,       indexed_value<int const&, std::string&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<LR&&, RR&&>,     indexed_value<int&&, std::string const&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<RR&&, V const&>, indexed_value<int const&, std::string const&>>);

    STATIC_CHECK(std::common_reference_with<CLR&&, V&>);
    STATIC_CHECK(std::common_reference_with<CLR&&, CRR&&>);
    STATIC_CHECK(std::common_reference_with<CRR&&, V const&>);
    STATIC_CHECK(std::same_as<std::common_reference_t<CLR&&, V&>,       indexed_value<int const&, std::string const&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<CLR&&, CRR&&>,    indexed_value<int&&, std::string const&>>);
    STATIC_CHECK(std::same_as<std::common_reference_t<CRR&&, V const&>, indexed_value<int const&, std::string const&>>);

    // ---------------------------------------------------

    STATIC_CHECK(std::same_as<
        std::common_type_t<indexed_value<short, char>, indexed_value<int, long>>,
        indexed_value<int, long>
    >);

    // ---------------------------------------------------

    STATIC_CHECK(std::convertible_to<LR&,   indexed_value<int const&, std::string&>>);
    STATIC_CHECK(std::convertible_to<LR&&,  indexed_value<int&&, std::string&>>);
    STATIC_CHECK(std::convertible_to<RR&&,  indexed_value<int&&, std::string&&>>);
    STATIC_CHECK(std::convertible_to<CRR&&, indexed_value<int const&&, std::string const&&>>);

    // ---------------------------------------------------

    STATIC_CHECK(std::same_as<decltype(get<0>(std::declval<V&>())),       int&>);
    STATIC_CHECK(std::same_as<decltype(get<0>(std::declval<V const&>())), int const&>);
    STATIC_CHECK(std::same_as<decltype(get<0>(std::declval<V&&>())),      int&&>);

    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<V&>())),       std::string&>);
    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<V const&>())), std::string const&>);
    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<V&&>())),      std::string&&>);

    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<LR&&>())),  std::string&>);
    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<RR&&>())),  std::string&&>);
    STATIC_CHECK(std::same_as<decltype(get<1>(std::declval<CLR&&>())), std::string const&>);

    // ---------------------------------------------------

    STATIC_CHECK(std::tuple_size_v<V> == 2);
    STATIC_CHECK(std::same_as<std::tuple_element_t<0, V>, int>);
    STATIC_CHECK(std::same_as<std::tuple_element_t<1, V>, std::string>);

    STATIC_CHECK(std::same_as<std::tuple_element_t<1, LR>, std::string&>);
    STATIC_CHECK(std::same_as<std::tuple_element_t<1, CLR>, std::string const&>);

    {
        std::string value{"foo"};
        LR indexed{42, value};
        CHECK(indexed == indexed);
        CHECK((indexed <=> indexed) == std::strong_ordering::equal);
        auto [index, element] = indexed;
        CHECK(index == 42);
        CHECK(&element == &value);
    }
}

TEST_CASE("indexed_value: tuple/pair")
{
    STATIC_CHECK(std::equal_to{}(indexed_value<int, double>{}, indexed_value<int, double>{}));

    STATIC_CHECK(std::equal_to{}(indexed_value<int, double>{}, std::tuple<int, double>{}));
    STATIC_CHECK(std::equal_to{}(std::tuple<int, double>{}, indexed_value<int, double>{}));

    STATIC_CHECK(std::equal_to{}(indexed_value<int, double>{}, std::pair<int, double>{}));
    STATIC_CHECK(std::equal_to{}(std::pair<int, double>{}, indexed_value<int, double>{}));
}

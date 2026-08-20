#include "iris_test.hpp"

#include <iris/alloy/adapted/std_pair.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>

#include <iris/sfield/serialize_traits.hpp>
#include <iris/sfield/adapt.hpp>
#include <iris/sfield/serialize_json.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std::string_view_literals;

enum non_scoped_enum : int {};
enum struct scoped_enum : int {};

struct NonSerializable {};

TEST_CASE("sfield: serialize (builtin types)")
{
    STATIC_CHECK(!iris::sfield::serializable<NonSerializable>);

    STATIC_CHECK(iris::sfield::serializable_primitive<int>);
    STATIC_CHECK(iris::sfield::serializable_primitive<int const>);
    STATIC_CHECK(iris::sfield::serializable_primitive<non_scoped_enum>);
    STATIC_CHECK(iris::sfield::serializable_primitive<scoped_enum>);

    STATIC_CHECK(iris::sfield::serializable_primitive<std::string>);
    STATIC_CHECK(iris::sfield::serializable_primitive<std::u32string>);
    STATIC_CHECK(iris::sfield::serializable_primitive<std::string_view>);
    STATIC_CHECK(iris::sfield::serializable_primitive<std::u32string_view>);

    STATIC_CHECK(iris::sfield::serializable_primitive<std::optional<int>>);
    STATIC_CHECK(iris::sfield::serializable_primitive<std::optional<int> const>);
    STATIC_CHECK(!iris::sfield::serializable<std::optional<NonSerializable>>);

    STATIC_CHECK(iris::sfield::serializable_tuple<std::pair<int, int>>);
    STATIC_CHECK(!iris::sfield::serializable<std::pair<int, NonSerializable>>);
    STATIC_CHECK(iris::sfield::serializable_tuple<std::tuple<int, int>>);
    STATIC_CHECK(!iris::sfield::serializable<std::tuple<int, NonSerializable>>);

    STATIC_CHECK(iris::sfield::serializable_array<std::vector<int>>);
    STATIC_CHECK(!iris::sfield::serializable<std::vector<NonSerializable>>);

    STATIC_CHECK(iris::sfield::serializable_map<std::map<int, int>>);
    STATIC_CHECK(!iris::sfield::serializable_array<std::map<int, int>>);
    STATIC_CHECK(!iris::sfield::serializable<std::map<int, NonSerializable>>);

    {
        nlohmann::json json;
        int const value = 42;
        iris::sfield::save(json, value);
        CHECK(json.get<int>() == 42);
    }

    {
        nlohmann::json json;
        std::optional<int> const value = 42;
        iris::sfield::save(json, value);
        CHECK(json.get<int>() == 42);
    }

    {
        nlohmann::json json;
        auto const value = non_scoped_enum{42};
        iris::sfield::save(json, value);
        CHECK(json.get<non_scoped_enum>() == non_scoped_enum{42});
    }
    {
        nlohmann::json json;
        auto const value = scoped_enum{42};
        iris::sfield::save(json, value);
        CHECK(json.get<scoped_enum>() == scoped_enum{42});
    }

    {
        nlohmann::json json;
        std::string const value = "foo";
        iris::sfield::save(json, value);
        CHECK(json.get<std::string>() == "foo"sv);
    }
    {
        nlohmann::json json;
        std::u32string const value = U"あいう";
        iris::sfield::save(json, value);
        CHECK(json.get<std::u32string>() == U"あいう"sv);
    }

    {
        nlohmann::json json;
        std::vector<int> const arr{0, 1, 2};
        iris::sfield::save(json, arr);
        CHECK(json.get<std::vector<int>>() == arr);
    }

    {
        nlohmann::json json;
        std::map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        iris::sfield::save(json, map);
        CHECK(json.get<std::map<std::string, int>>() == map);
    }

    {
        nlohmann::json json;
        std::unordered_map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        iris::sfield::save(json, map);
        CHECK(json.get<std::map<std::string, int>>() == std::map<std::string, int>{std::from_range, map});
    }

    {
        nlohmann::json json;
        std::pair const pair{0, 1};
        iris::sfield::save(json, pair);
        CHECK(json.get<std::pair<int, int>>() == std::pair{0, 1});
    }

    {
        nlohmann::json json;
        std::tuple const tuple{0, 1, 2};
        iris::sfield::save(json, tuple);
        CHECK(json.get<std::tuple<int, int, int>>() == std::tuple{0, 1, 2});
    }
}


struct MyData
{
    IRIS_SFIELD_CLASS(MyData)

    IRIS_SFIELD(std::string, name)
    IRIS_SFIELD(std::string, text, "empty text")
    IRIS_SFIELD((std::pair<int, int>), pair)
    IRIS_SFIELD(int, age)
    IRIS_SFIELD(bool, enabled, true)
};

IRIS_SFIELD_ADAPT(
    MyData, name, text, age, enabled
);

TEST_CASE("sfield: serialize (class type)")
{
    STATIC_CHECK(iris::sfield::serializable_class<MyData>);
    STATIC_CHECK(!iris::sfield::serializable_tuple<MyData>);

    {
        MyData my_data;

        STATIC_CHECK(std::same_as<decltype(my_data.get_name()), std::string const&>);
        CHECK(my_data.get_name().empty());
        my_data.set_name("foo");
        CHECK(my_data.get_name() == "foo"sv);

        CHECK(my_data.get_text() == "empty text"sv);

        STATIC_CHECK(std::same_as<decltype(my_data.get_age()), int>);
        CHECK(my_data.get_age() == 0);

        STATIC_CHECK(std::same_as<decltype(my_data.is_enabled()), bool>);
        CHECK(my_data.is_enabled() == true);
    }

    {
        nlohmann::json json;
        MyData my_data;
        my_data.set_name("foo");
        my_data.set_age(42);
        my_data.set_enabled(true);
        iris::sfield::save(json, my_data);

        CHECK(json.at("name").get<std::string>() == "foo"sv);
        CHECK(json.at("text").get<std::string>() == "empty text"sv);
        CHECK(json.at("age").get<int>() == 42);
        CHECK(json.at("enabled").get<bool>() == true);
    }
}

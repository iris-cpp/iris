#include "iris_test.hpp"

#include <iris/alloy/adapted/std_pair.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/adapt.hpp>
#include <iris/marshal/serialize_json.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std::string_view_literals;

namespace marshal = iris::marshal;
namespace json = marshal::json;

enum non_scoped_enum : int {};
enum struct scoped_enum : int {};

struct NonSerializable {};

TEST_CASE("marshal: serialize (builtin types)")
{
    STATIC_CHECK(!marshal::serializable<NonSerializable>);

    STATIC_CHECK(marshal::serializable_scalar<int>);
    STATIC_CHECK(marshal::serializable_scalar<int const>);
    STATIC_CHECK(marshal::serializable_scalar<non_scoped_enum>);
    STATIC_CHECK(marshal::serializable_scalar<scoped_enum>);

    STATIC_CHECK(marshal::serializable_scalar<std::string>);
    STATIC_CHECK(marshal::serializable_scalar<std::u32string>);
    STATIC_CHECK(marshal::serializable_scalar<std::string_view>);
    STATIC_CHECK(marshal::serializable_scalar<std::u32string_view>);

    STATIC_CHECK(marshal::serializable_optional<std::optional<int>>);
    STATIC_CHECK(marshal::serializable_optional<std::optional<int> const>);
    STATIC_CHECK(!marshal::serializable<std::optional<NonSerializable>>);

    STATIC_CHECK(marshal::serializable_tuple<std::pair<int, int>>);
    STATIC_CHECK(!marshal::serializable<std::pair<int, NonSerializable>>);
    STATIC_CHECK(marshal::serializable_tuple<std::tuple<int, int>>);
    STATIC_CHECK(!marshal::serializable<std::tuple<int, NonSerializable>>);

    STATIC_CHECK(marshal::serializable_array<std::vector<int>>);
    STATIC_CHECK(!marshal::serializable<std::vector<NonSerializable>>);

    STATIC_CHECK(marshal::serializable_map<std::map<int, int>>);
    STATIC_CHECK(!marshal::serializable_array<std::map<int, int>>);
    STATIC_CHECK(!marshal::serializable<std::map<int, NonSerializable>>);

    STATIC_CHECK(json::serializable_map<std::map<std::string, int>>);
    STATIC_CHECK(json::serializable_map<std::map<std::string_view, int>>);
    STATIC_CHECK(!json::serializable_map<std::map<int, int>>);
    STATIC_CHECK(json::serializable_map<std::vector<std::pair<std::string, int>>>);
    STATIC_CHECK(!json::serializable_map<std::vector<std::pair<int, int>>>);

    {
        nlohmann::json j;
        int const value = 42;
        json::save(j, value);
        CHECK(j.get<int>() == value);
    }

    {
        nlohmann::json j;
        std::optional<int> const value = 42;
        json::save(j, value);
        CHECK(j.get<int>() == value);
    }

    {
        nlohmann::json j;
        std::vector<std::optional<int>> const value = {42, std::nullopt, 44};
        json::save(j, value);
        CHECK(j.get<std::vector<std::optional<int>>>() == value);
    }

    {
        nlohmann::json j;
        auto const value = non_scoped_enum{42};
        json::save(j, value);
        CHECK(j.get<non_scoped_enum>() == value);
    }
    {
        nlohmann::json j;
        auto const value = scoped_enum{42};
        json::save(j, value);
        CHECK(j.get<scoped_enum>() == value);
    }

    {
        nlohmann::json j;
        std::string const value = "foo";
        json::save(j, value);
        CHECK(j.get<std::string>() == value);
    }
    {
        nlohmann::json j;
        std::u32string const value = U"あいう";
        json::save(j, value);
        CHECK(j.get<std::u32string>() == value);
    }

    {
        nlohmann::json j;
        std::vector<int> const arr{0, 1, 2};
        json::save(j, arr);
        CHECK(j.get<std::vector<int>>() == arr);
    }

    {
        nlohmann::json j;
        std::map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, int>>() == map);
    }

    {
        nlohmann::json j;
        std::unordered_map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, int>>() == std::map<std::string, int>{std::from_range, map});
    }

    {
        nlohmann::json j;
        std::pair const value{0, 1};
        json::save(j, value);
        CHECK(j.get<std::pair<int, int>>() == value);
    }

    {
        nlohmann::json j;
        std::tuple const value{0, 1, 2};
        json::save(j, value);
        CHECK(j.get<std::tuple<int, int, int>>() == value);
    }
}


struct MyData
{
    IRIS_MARSHAL_CLASS(MyData)

    IRIS_MARSHAL_FIELD(std::string, name)
    IRIS_MARSHAL_FIELD(std::string, text, "empty text")
    IRIS_MARSHAL_FIELD((std::pair<int, int>), pair)
    IRIS_MARSHAL_FIELD(int, age)
    IRIS_MARSHAL_FIELD(bool, enabled, true)
};

IRIS_MARSHAL_ADAPT(
    MyData, name, text, age, enabled
);

TEST_CASE("marshal: serialize (class type)")
{
    STATIC_CHECK(marshal::serializable_class<MyData>);
    STATIC_CHECK(!marshal::serializable_tuple<MyData>);

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
        json::save(json, my_data);

        CHECK(json.at("name").get<std::string>() == "foo"sv);
        CHECK(json.at("text").get<std::string>() == "empty text"sv);
        CHECK(json.at("age").get<int>() == 42);
        CHECK(json.at("enabled").get<bool>() == true);
    }
}

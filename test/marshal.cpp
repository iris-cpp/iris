#include "iris_test.hpp"

#include <iris/alloy/adapted/std_pair.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/adapt.hpp>
#include <iris/marshal/serialize_json.hpp>

#include <iris/format.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <flat_map>

using namespace std::string_view_literals;

namespace marshal = iris::marshal;
namespace json = marshal::json;

using marshal::generic_format;

enum non_scoped_enum : int {};
enum struct scoped_enum : int {};

struct NonSerializable {};


TEST_CASE("marshal: type traits (generic_format)")
{
    STATIC_CHECK(marshal::detail::marshal_format<generic_format>);
}

TEST_CASE("marshal: type traits (json::format)")
{
    STATIC_CHECK(marshal::detail::marshal_format<json::format>);

    // CharT = char
    {
        STATIC_CHECK(!marshal::adapted_proxy<char const*, json::format>);
        STATIC_CHECK(!marshal::adapted_proxy<std::string, json::format>);
        STATIC_CHECK(!marshal::adapted_proxy<std::string_view, json::format>);
    }
    // CharT = char32_t
    {
        STATIC_CHECK(marshal::adapted_proxy<char32_t const*, json::format>);
        STATIC_CHECK(std::same_as<marshal::proxy_native_type_t<char32_t const*, json::format>, std::string>);
        STATIC_CHECK(marshal::detail::proxy_writable<char32_t const*, json::format>);

        STATIC_CHECK(marshal::adapted_proxy<std::u32string, json::format>);
        STATIC_CHECK(std::same_as<marshal::proxy_native_type_t<std::u32string, json::format>, std::string>);
        STATIC_CHECK(marshal::detail::proxy_writable<std::u32string, json::format>);

        STATIC_CHECK(marshal::adapted_proxy<std::u32string_view, json::format>);
        STATIC_CHECK(std::same_as<marshal::proxy_native_type_t<std::u32string_view, json::format>, std::string>);
        STATIC_CHECK(marshal::detail::proxy_writable<std::u32string_view, json::format>);
    }
}

TEST_CASE("marshal: serialize (builtin types)")
{
    STATIC_CHECK(!json::serializable<NonSerializable>);

    STATIC_CHECK(json::serializable_scalar<int const>);
    STATIC_CHECK(!json::deserializable_scalar<int const>);

#define IRIS_CHECK_SERIALIZABLE(kind, ...) \
    STATIC_CHECK(json::serializable_ ## kind<__VA_ARGS__>); \
    STATIC_CHECK(json::deserializable_ ## kind<__VA_ARGS__>);

    IRIS_CHECK_SERIALIZABLE(scalar, int);
    IRIS_CHECK_SERIALIZABLE(scalar, non_scoped_enum);
    IRIS_CHECK_SERIALIZABLE(scalar, scoped_enum);

    IRIS_CHECK_SERIALIZABLE(scalar, std::string);
    IRIS_CHECK_SERIALIZABLE(scalar, std::string_view);
    IRIS_CHECK_SERIALIZABLE(proxy, std::u32string);
    IRIS_CHECK_SERIALIZABLE(proxy, std::u32string_view);

    IRIS_CHECK_SERIALIZABLE(optional, std::optional<int>);
    IRIS_CHECK_SERIALIZABLE(optional, std::optional<std::string>);
    IRIS_CHECK_SERIALIZABLE(optional, std::optional<std::u32string>);
    STATIC_CHECK(!json::serializable<std::optional<NonSerializable>>);

    IRIS_CHECK_SERIALIZABLE(tuple, std::pair<int, int>);
    IRIS_CHECK_SERIALIZABLE(tuple, std::tuple<int, int>);
    IRIS_CHECK_SERIALIZABLE(tuple, std::pair<std::string, int>);
    IRIS_CHECK_SERIALIZABLE(tuple, std::tuple<std::string, int>);
    IRIS_CHECK_SERIALIZABLE(tuple, std::pair<std::u32string, int>);
    IRIS_CHECK_SERIALIZABLE(tuple, std::tuple<std::u32string, int>);
    STATIC_CHECK(!json::serializable<std::pair<int, NonSerializable>>);
    STATIC_CHECK(!json::serializable<std::tuple<int, NonSerializable>>);

    IRIS_CHECK_SERIALIZABLE(array, std::vector<int>);
    IRIS_CHECK_SERIALIZABLE(array, std::vector<std::string>);
    IRIS_CHECK_SERIALIZABLE(array, std::vector<std::u32string>);
    STATIC_CHECK(!json::serializable<std::vector<NonSerializable>>);

    IRIS_CHECK_SERIALIZABLE(map, std::map<char const*, int>);
    IRIS_CHECK_SERIALIZABLE(map, std::map<std::string_view, int>);
    IRIS_CHECK_SERIALIZABLE(map, std::map<std::string, int>);

    IRIS_CHECK_SERIALIZABLE(map, std::map<char32_t const*, int>);
    IRIS_CHECK_SERIALIZABLE(map, std::map<std::u32string_view, int>);
    IRIS_CHECK_SERIALIZABLE(map, std::map<std::u32string, int>);

    STATIC_CHECK(!json::serializable_map<std::map<int, int>>);
    STATIC_CHECK(!json::serializable_array<std::map<int, int>>);
    STATIC_CHECK(!json::serializable<std::map<int, NonSerializable>>);

    //IRIS_CHECK_SERIALIZABLE(map, std::vector<std::pair<std::string, int>>); // TODO
    STATIC_CHECK(!json::serializable_map<std::vector<std::pair<int, int>>>);

#undef IRIS_CHECK_SERIALIZABLE

    {
        nlohmann::json j;
        int const value = 42;
        json::save(j, value);
        CHECK(j.get<int>() == value);
        CHECK(json::load<int>(j) == value);
    }

    {
        nlohmann::json j;
        auto const value = non_scoped_enum{42};
        json::save(j, value);
        CHECK(j.get<non_scoped_enum>() == value);
        CHECK(json::load<non_scoped_enum>(j) == value);
    }
    {
        nlohmann::json j;
        auto const value = scoped_enum{42};
        json::save(j, value);
        CHECK(j.get<scoped_enum>() == value);
        CHECK(json::load<scoped_enum>(j) == value);
    }

    {
        nlohmann::json j;
        std::string const str = "foo";
        json::save(j, str);
        CHECK(j.get<std::string>() == str);
        CHECK(json::load<std::string>(j) == str);
    }
    {
        nlohmann::json j;
        std::u32string const str = U"あいう";
        json::save(j, str);
        CHECK(iris::to_u32string_ref(j.get<std::string>()) == str);
        CHECK(json::load<std::u32string>(j) == str);
    }

    {
        nlohmann::json j;
        std::vector<int> const arr{0, 1, 2};
        json::save(j, arr);
        CHECK(j.get<std::vector<int>>() == arr);
        CHECK(json::load<std::vector<int>>(j) == arr);
    }
    {
        nlohmann::json j;
        std::vector<std::string> const arr{"foo", "bar"};
        json::save(j, arr);
        CHECK(j.get<std::vector<std::string>>() == arr);
        CHECK(json::load<std::vector<std::string>>(j) == arr);
    }
    {
        nlohmann::json j;
        std::vector<std::u32string> const arr{U"foo", U"あいう"};
        json::save(j, arr);
        CHECK(json::load<std::vector<std::u32string>>(j) == arr);
    }

    {
        nlohmann::json j;
        std::map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, int>>() == map);
        CHECK(json::load<std::map<std::string, int>>(j) == map);
    }
    {
        nlohmann::json j;
        std::map<std::string, std::string> const map{{"foo", "0"}, {"bar", "1"}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, std::string>>() == map);
        CHECK(json::load<std::map<std::string, std::string>>(j) == map);
    }
    {
        nlohmann::json j;
        std::map<std::u32string, int> const map{{U"foo", 0}, {U"あいう", 1}};
        json::save(j, map);
        CHECK(json::load<std::map<std::u32string, int>>(j) == map);
    }
    {
        nlohmann::json j;
        std::map<std::u32string, std::u32string> const map{{U"foo", U"0"}, {U"あいう", U"1"}};
        json::save(j, map);
        CHECK(json::load<std::map<std::u32string, std::u32string>>(j) == map);
    }

    {
        nlohmann::json j;
        std::unordered_map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, int>>() == std::map<std::string, int>{std::from_range, map});
        CHECK(json::load<std::unordered_map<std::string, int>>(j) == map);
    }

    {
        nlohmann::json j;
        std::flat_map<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::map<std::string, int>>() == std::map<std::string, int>{std::from_range, map});
        CHECK(json::load<std::flat_map<std::string, int>>(j) == map);
    }

    {
        nlohmann::json j;
        std::multimap<std::string, int> const map{{"foo", 0}, {"bar", 1}};
        json::save(j, map);
        CHECK(j.get<std::multimap<std::string, int>>() == map);
        CHECK(json::load<std::multimap<std::string, int>>(j) == map);
    }

    {
        nlohmann::json j;
        std::pair const pair{0, 1};
        json::save(j, pair);
        CHECK(j.get<std::pair<int, int>>() == pair);
        CHECK(json::load<std::pair<int, int>>(j) == pair);
    }

    {
        nlohmann::json j;
        std::pair<std::string, int> const pair{"foo", 0};
        json::save(j, pair);
        CHECK(j.get<std::pair<std::string, int>>() == pair);
        CHECK(json::load<std::pair<std::string, int>>(j) == pair);
    }
    {
        nlohmann::json j;
        std::pair<std::u32string, int> const pair{U"あいう", 0};
        json::save(j, pair);
        CHECK(json::load<std::pair<std::u32string, int>>(j) == pair);
    }

    {
        nlohmann::json j;
        std::tuple const tup{0, 1, 2};
        json::save(j, tup);
        CHECK(j.get<std::tuple<int, int, int>>() == tup);
        CHECK(json::load<std::tuple<int, int, int>>(j) == tup);
    }

    {
        nlohmann::json j;
        std::optional<int> const opt = 42;
        json::save(j, opt);
        CHECK(j.get<std::optional<int>>() == opt);
        CHECK(json::load<std::optional<int>>(j) == opt);
    }
    {
        nlohmann::json j;
        std::optional<std::string> const opt = "foo";
        json::save(j, opt);
        CHECK(j.get<std::optional<std::string>>() == opt);
        CHECK(json::load<std::optional<std::string>>(j) == opt);
    }
    {
        nlohmann::json j;
        std::optional<std::u32string> const opt = U"あいう";
        json::save(j, opt);
        CHECK(json::load<std::optional<std::u32string>>(j) == opt);
    }

    {
        nlohmann::json j;
        std::vector<std::optional<int>> const vec_opt = {42, std::nullopt, 44};
        json::save(j, vec_opt);
        CHECK(j.get<std::vector<std::optional<int>>>() == vec_opt);
        CHECK(json::load<std::vector<std::optional<int>>>(j) == vec_opt);
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

template<>
struct std::formatter<MyData> : iris::no_spec_formatter<char>
{
    template<class Ctx>
    static Ctx::iterator format(MyData const& my_data, Ctx& ctx)
    {
        return std::format_to(
            ctx.out(),
            "{{name: \"{}\", text: \"{}\", pair: ({},{}), age: {}, enabled: {}}}",
            my_data.get_name(),
            my_data.get_text(),
            my_data.get_pair().first, my_data.get_pair().second,
            my_data.get_age(),
            my_data.is_enabled()
        );
    }
};

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
        nlohmann::json j;
        MyData my_data;
        my_data.set_name("foo");
        my_data.set_age(42);
        my_data.set_enabled(true);
        json::save(j, my_data);

        CHECK(j.at("name").get<std::string>() == "foo"sv);
        CHECK(j.at("text").get<std::string>() == "empty text"sv);
        CHECK(j.at("age").get<int>() == 42);
        CHECK(j.at("enabled").get<bool>() == true);

        CHECK(json::load<MyData>(j) == my_data);
    }
}

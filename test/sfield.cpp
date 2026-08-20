#include "iris_test.hpp"

#include <iris/sfield.hpp>

#include <concepts>
#include <string_view>

using namespace std::string_view_literals;

struct MyData
{
    IRIS_SFIELD((std::string), name)
    IRIS_SFIELD((std::string), text, "empty text")
    IRIS_SFIELD((int), age)
    IRIS_SFIELD_BOOL((bool), enabled, true)
};

TEST_CASE("sfield")
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

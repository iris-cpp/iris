#include "iris_test.hpp"

#include <iris/color_format.hpp>

TEST_CASE("colorize_string")
{
    {
        constexpr auto test = [](iris::colorize_string) {};
        test("foo");
        test("[reset]");
        test("[fg:reset]");
        test("[bg:reset]");
        test("[black]");
        test("[red]");
        test("[green]");
        test("[yellow]");
        test("[blue]");
        test("[magenta]");
        test("[cyan]");
        test("[white]");
        test("[red|bold]");
        test("[bold|italic]");
        test("[fg:red|bg:blue]");
    }
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[]")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("]")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[reset|red]")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[red|reset]")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[black|red]")), iris::colorize_error);
}

TEST_CASE("colorize_format_string")
{
    {
        constexpr auto test = [](iris::colorize_format_string<>) {};
        test("foo");
        test("[reset]");
        test("[fg:reset]");
        test("[bg:reset]");
        test("[black]");
        test("[red]");
        test("[green]");
        test("[yellow]");
        test("[blue]");
        test("[magenta]");
        test("[cyan]");
        test("[white]");
        test("[red|bold]");
        test("[bold|italic]");
        test("[fg:red|bg:blue]");
    }
}

TEST_CASE("colorize")
{
    {
        std::string s;
        iris::colorize_to(std::back_inserter(s), "[red]foo");
        CHECK(s == "\033[38;2;255;0;0mfoo");
    }

    {
        auto const s = iris::colorize("[green]bar");
        CHECK(s == "\033[38;2;0;128;0mbar");
    }

    {
        auto const s = iris::colorize("[red]foo[green]bar[reset]baz");
        CHECK(s == "\033[38;2;255;0;0mfoo\033[38;2;0;128;0mbar\033[0mbaz");
    }

    {
        auto const s = iris::colorize("[red][[[green]]]");
        CHECK(s == "\033[38;2;255;0;0m[\033[38;2;0;128;0m]");
    }

    {
        auto const s = iris::colorize("[red|bold]foo");
        CHECK(s == "\033[1;38;2;255;0;0mfoo");
    }

    {
        auto const s = iris::colorize("[bold|italic]foo");
        CHECK(s == "\033[1;3mfoo");
    }

    {
        auto const s = iris::colorize("[fg:red|bg:blue]foo");
        CHECK(s == "\033[38;2;255;0;0;48;2;0;0;255mfoo");
    }

    {
        auto const s = iris::colorize("[fg:red|bg:blue]foo[fg:reset]bar[bg:reset]baz");
        CHECK(s == "\033[38;2;255;0;0;48;2;0;0;255mfoo\033[39mbar\033[49mbaz");
    }

    {
        auto const s = iris::colorize("[lime]foo");
        CHECK(s == "\033[38;2;0;255;0mfoo");
    }

    {
        auto const s = iris::colorize("[bg:gray]foo");
        CHECK(s == "\033[48;2;128;128;128mfoo");
    }

    {
        auto const s = iris::colorize("[rgb(12,34,56)]foo");
        CHECK(s == "\033[38;2;12;34;56mfoo");
    }

    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[rgb(256,34,56)]foo")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[rgb(-1,34,56)]foo")), iris::colorize_error);

    {
        auto const s = iris::colorize("[fg:rgb(12,34,56)]foo");
        CHECK(s == "\033[38;2;12;34;56mfoo");
    }

    {
        auto const s = iris::colorize("[bg:rgb(12,34,56)]foo");
        CHECK(s == "\033[48;2;12;34;56mfoo");
    }
    {
        auto const s = iris::colorize("[fg:rgb(12,34,56)|bg:rgb(78,90,12)]foo");
        CHECK(s == "\033[38;2;12;34;56;48;2;78;90;12mfoo");
    }

    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[fg:rgb(12,34,56)|fg:rgb(78,90,12)]foo")), iris::colorize_error);
    CHECK_THROWS_AS((void)iris::colorize(iris::runtime_colorize("[bg:rgb(12,34,56)|bg:rgb(78,90,12)]foo")), iris::colorize_error);

    CHECK(iris::colorized_size("[red]foo") == 18);

    // TODO
    #ifndef _MSC_VER
        static_assert(iris::colorize("[red]foo") == "\033[38;2;255;0;0mfoo");
    #endif
}

TEST_CASE("format_colorize")
{
    {
        std::string s;
        iris::format_colorize_to(std::back_inserter(s), "[red]{}", 42);
        CHECK(s == "\033[38;2;255;0;0m42");
    }
    {
        auto const s = iris::format_colorize("[green]{}", 42);
        CHECK(s == "\033[38;2;0;128;0m42");
    }
}

#if __cpp_lib_format >= 202311L

TEST_CASE("colorize_format")
{
    {
        std::string s;
        iris::colorize_format_to(std::back_inserter(s), "[red]{}", 42);
        CHECK(s == "\033[38;2;255;0;0m42");
    }
    {
        auto const s = iris::colorize_format("[green]{}", 42);
        CHECK(s == "\033[38;2;0;128;0m42");
    }
}

#endif

TEST_CASE("fixed")
{
    // TODO
#ifndef _MSC_VER
    static constexpr iris::fixed_string str = "[red]foo";
    static_assert(iris::colorized_size(str) == 18);
    static_assert(iris::colorize(str) == "\033[38;2;255;0;0mfoo");
    static_assert(std::string_view{ iris::static_colorize_string<str>::colorized } == "\033[38;2;255;0;0mfoo");
    using namespace iris::colorize_literals;
    auto const s = iris::colorize_format("[red]{}"_col, 42);
    CHECK(s == "\033[38;2;255;0;0m42");
#endif
}


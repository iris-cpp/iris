// SPDX-License-Identifier: MIT

#include "iris_rvariant_test.hpp"

#include <iostream>

namespace {

namespace NonStreamable_ns {

// Not streamable via ADL, but...
struct NonStreamable {};

} // NonStreamable_ns

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, NonStreamable_ns::NonStreamable&)
{
    return os << "polluted &";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, NonStreamable_ns::NonStreamable const&)
{
    return os << "polluted const&";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, [[maybe_unused]] NonStreamable_ns::NonStreamable&&)
{
    return os << "polluted &&";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, NonStreamable_ns::NonStreamable const&&)
{
    return os << "polluted const&&";
}

} // anonymous global

#include <iris/io_fwd.hpp> // this finds `operator<<` in the global ns

#include <iris/rvariant/rvariant.hpp>
#include <iris/rvariant/rvariant_io.hpp>
#include <iris/rvariant/recursive_wrapper.hpp>

#include <string>
#include <sstream>


namespace unit_test {

namespace Thrower_ADL_guard {

std::ostream& operator<<(std::ostream& os, MC_Thrower const&)
{
    return os << "MC_Thrower";
}

} // MC_Thrower_ADL_guard


namespace {

namespace S_ns {

struct S
{
    std::string msg;
};

std::ostream& operator<<(std::ostream& os, S const& s)
{
    return os << s.msg;
}

} // S_ns

namespace ThrowingValue_ns {

struct ThrowingValue {};

// Not derived from std::exception
struct StrangeException {};

std::ostream& operator<<(std::ostream& os, ThrowingValue const&)
{
    os << "ThrowingValue";
    throw StrangeException{};  // NOLINT(hicpp-exception-baseclass)
}

} // ThrowingValue_ns

} // anonymous


TEST_CASE("rvariant.io, simple")
{
    using S_ns::S;

    {
        iris::rvariant<int> v{42};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "42");
    }
    {
        STATIC_REQUIRE(iris::req::ADL_ostreamable_v<S>);
        STATIC_REQUIRE(iris::req::ADL_ostreamable_v<iris::rvariant<S>>);
        iris::rvariant<S> v{S{"foo"}};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "foo");
    }
    {
        struct NonExistent;
        STATIC_REQUIRE(!iris::req::ADL_ostreamable_v<NonExistent>);
        struct NonExistent {};
        STATIC_REQUIRE(!iris::req::ADL_ostreamable_v<iris::rvariant<NonExistent>>);
    }
    {
        // ReSharper disable once CppStaticAssertFailure
        STATIC_REQUIRE(!iris::req::ADL_ostreamable_v<NonStreamable_ns::NonStreamable>);
        // ReSharper disable once CppStaticAssertFailure
        STATIC_REQUIRE(!iris::req::ADL_ostreamable_v<iris::rvariant<NonStreamable_ns::NonStreamable>>);

        std::ostringstream oss;
        NonStreamable_ns::NonStreamable const non_streamable;
        oss << non_streamable;
        CHECK(oss.str() == "polluted const&");
    }

    // alternative = int
    {
        iris::rvariant<int, S> v{42};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "42");
    }
    // alternative = int
    {
        iris::rvariant<S, int> v{42};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "42");
    }

    // alternative = S
    {
        iris::rvariant<int, S> v{S{"foo"}};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "foo");
    }
    // alternative = S
    {
        iris::rvariant<S, int> v{S{"foo"}};
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "foo");
    }
}

TEST_CASE("rvariant.io, exceptions")
{
    using ThrowingValue_ns::ThrowingValue;

    // good at first + no exceptions
    {
        iris::rvariant<ThrowingValue> v;
        std::ostringstream oss; // good at first
        REQUIRE_NOTHROW(oss << v);
        CHECK(oss.str() == "ThrowingValue");
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0);
    }
    // bad at first + no exceptions
    {
        iris::rvariant<ThrowingValue> v;
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        REQUIRE_NOTHROW(oss << v);
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0);
    }

    // good at first + exceptions
    {
        iris::rvariant<ThrowingValue> v;
        std::ostringstream oss; // good at first
        oss.exceptions(std::ios_base::badbit);
        REQUIRE_THROWS_AS(oss << v, ThrowingValue_ns::StrangeException);
        CHECK(oss.str() == "ThrowingValue");
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0);
    }
    // bad at first + exceptions
    {
        iris::rvariant<ThrowingValue> v;
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        try {
            oss.exceptions(std::ios_base::badbit);
        } catch (std::ios_base::failure const&) {  // NOLINT(bugprone-empty-catch)
            // propagate invalid stream
        }
        REQUIRE_THROWS_AS(oss << v, std::ios_base::failure);
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0);
    }

    // [valueless] good at first + no exceptions
    {
        iris::rvariant<int, MC_Thrower> v = make_valueless<int>();
        std::ostringstream oss; // good at first
        REQUIRE_THROWS_AS(oss << v, std::bad_variant_access); // valueless always throws
        CHECK(oss.str().empty());
    }
    // [valueless] bad at first + no exceptions
    {
        iris::rvariant<int, MC_Thrower> v = make_valueless<int>();
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        REQUIRE_NOTHROW(oss << v); // sentry should be engaged
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0); // still BAD
    }

    // [valueless] good at first + exceptions
    {
        iris::rvariant<int, MC_Thrower> v = make_valueless<int>();
        std::ostringstream oss; // good at first
        oss.exceptions(std::ios_base::badbit);
        REQUIRE_THROWS_AS(oss << v, std::bad_variant_access); // valueless always throws
        CHECK(oss.str().empty());
    }
    // [valueless] bad at first + exceptions
    {
        iris::rvariant<int, MC_Thrower> v = make_valueless<int>();
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        try {
            oss.exceptions(std::ios_base::badbit);
        } catch (std::ios_base::failure const&) {  // NOLINT(bugprone-empty-catch)
            // propagate invalid stream
        }
        REQUIRE_THROWS_AS(oss << v, std::ios_base::failure); // should do nothing
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0); // still BAD
    }

    // [nested valueless] good at first + no exceptions
    {
        iris::rvariant<int, iris::rvariant<int, MC_Thrower>> v = make_valueless<int>();
        std::ostringstream oss; // good at first
        REQUIRE_THROWS_AS(oss << v, std::bad_variant_access); // valueless always throws
        CHECK(oss.str().empty());
    }
    // [nested valueless] bad at first + no exceptions
    {
        iris::rvariant<int, iris::rvariant<int, MC_Thrower>> v = make_valueless<int>();
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        REQUIRE_NOTHROW(oss << v);
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0); // still BAD
    }

    // [nested valueless] good at first + exceptions
    {
        iris::rvariant<int, iris::rvariant<int, MC_Thrower>> v = make_valueless<int>();
        std::ostringstream oss; // good at first
        oss.exceptions(std::ios_base::badbit);
        REQUIRE_THROWS_AS(oss << v, std::bad_variant_access); // valueless always throws
        CHECK(oss.str().empty());
    }
    // [nested valueless] bad at first + exceptions
    {
        iris::rvariant<int, iris::rvariant<int, MC_Thrower>> v = make_valueless<int>();
        std::ostringstream oss;
        oss.setstate(std::ios_base::badbit); // BAD at first
        try {
            oss.exceptions(std::ios_base::badbit);
        } catch (std::ios_base::failure const&) {  // NOLINT(bugprone-empty-catch)
            // propagate invalid stream
        }
        REQUIRE_THROWS_AS(oss << v, std::ios_base::failure); // should do nothing
        CHECK(oss.str().empty());
        CHECK((oss.rdstate() & std::ios_base::badbit) != 0); // still BAD
    }
}

TEST_CASE("rvariant.io", "[recursive]")
{
    {
        iris::rvariant<iris::recursive_wrapper<int>> v(42);
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "42");
    }
}

TEST_CASE("rvariant formatter (char)")
{
    using V = iris::rvariant<int, double>;
    CHECK(std::format("{}", V{42}) == "42");
    CHECK(std::format("{}", V{3.14}) == "3.14");

    {
        constexpr auto v_fmt = iris::variant_format<int, double>("{:04d}", "{:.1f}");
        {
            V v(42);
            CHECK(std::format("pre{}post", iris::format_by(v_fmt, v)) == "pre0042post");
        }
        {
            V v(3.14);
            CHECK(std::format("pre{}post", iris::format_by(v_fmt, v)) == "pre3.1post");
        }
    }
    {
        constexpr auto v_fmt = iris::variant_format_for<V>("{:04d}", "{:.1f}");
        {
            V v(42);
            CHECK(std::format("pre{}post", iris::format_by(v_fmt, v)) == "pre0042post");
        }
        {
            V v(3.14);
            CHECK(std::format("pre{}post", iris::format_by(v_fmt, v)) == "pre3.1post");
        }
    }
    {
        auto const v = make_valueless<int>();
        CHECK_THROWS_AS(std::format("{}", v), std::bad_variant_access);
        {
            constexpr auto v_fmt = iris::variant_format_for<decltype(v)>("{}", "");
            CHECK_THROWS_AS(std::format("{}", iris::format_by(v_fmt, v)), std::bad_variant_access);
        }
        {
            constexpr auto v_fmt = iris::variant_format<int, MC_Thrower>("{}", "");
            CHECK_THROWS_AS(std::format("{}", iris::format_by(v_fmt, v)), std::bad_variant_access);
        }
    }
}

TEST_CASE("rvariant formatter (wchar_t)")
{
    using V = iris::rvariant<int, double>;
    CHECK(std::format(L"{}", V{42}) == L"42");
    CHECK(std::format(L"{}", V{3.14}) == L"3.14");

    {
        constexpr auto v_fmt = iris::variant_format<int, double>(L"{:04d}", L"{:.1f}");
        {
            V v(42);
            CHECK(std::format(L"pre{}post", iris::format_by(v_fmt, v)) == L"pre0042post");
        }
        {
            V v(3.14);
            CHECK(std::format(L"pre{}post", iris::format_by(v_fmt, v)) == L"pre3.1post");
        }
    }
    {
        constexpr auto v_fmt = iris::variant_format_for<V>(L"{:04d}", L"{:.1f}");
        {
            V v(42);
            CHECK(std::format(L"pre{}post", iris::format_by(v_fmt, v)) == L"pre0042post");
        }
        {
            V v(3.14);
            CHECK(std::format(L"pre{}post", iris::format_by(v_fmt, v)) == L"pre3.1post");
        }
    }
    {
        auto const v = make_valueless<int>();
        CHECK_THROWS_AS(std::format(L"{}", v), std::bad_variant_access);
        {
            constexpr auto v_fmt = iris::variant_format_for<decltype(v)>(L"{}", L"");
            CHECK_THROWS_AS(std::format(L"{}", iris::format_by(v_fmt, v)), std::bad_variant_access);
        }
        {
            constexpr auto v_fmt = iris::variant_format<int, MC_Thrower>(L"{}", L"");
            CHECK_THROWS_AS(std::format(L"{}", iris::format_by(v_fmt, v)), std::bad_variant_access);
        }
    }
}

TEST_CASE("rvariant formatter (char)", "[recursive]")
{
    CHECK(std::format("{}", iris::rvariant<iris::recursive_wrapper<int>>{42}) == "42");
}

TEST_CASE("rvariant formatter (wchar_t)", "[recursive]")
{
    CHECK(std::format(L"{}", iris::rvariant<iris::recursive_wrapper<int>>{42}) == L"42");
}

} // unit_test

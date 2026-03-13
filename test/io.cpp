// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include "iris_io_test.hpp" // this injects bad global overloads

#include <iris/io_fwd.hpp> // this finds `operator<<` in the global ns

#include <filesystem>
#include <sstream>

TEST_CASE("ADL_ostreamable")
{
    STATIC_CHECK(iris::req::ADL_ostreamable<char>);
    STATIC_CHECK(iris::req::ADL_ostreamable<std::filesystem::path>);

    // `*NonStreamable` do NOT satisfy ADL_ostreamable, though global `operator<<` is visible
    STATIC_CHECK(!iris::req::ADL_ostreamable<DirectNonStreamable_ns::DirectNonStreamable>);
    {
        std::ostringstream oss;
        oss << DirectNonStreamable_ns::DirectNonStreamable{};
        CHECK(oss.str() == "polluted &&");
    }

    STATIC_CHECK(!iris::req::ADL_ostreamable<TemplatedNonStreamable_ns::TemplatedNonStreamable>);
    {
        std::ostringstream oss;
        oss << TemplatedNonStreamable_ns::TemplatedNonStreamable{};
        CHECK(oss.str() == "polluted &&");
    }

    {
        struct Incomplete;
        STATIC_CHECK(!iris::req::ADL_ostreamable<Incomplete>);
    }
}

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/io_fwd.hpp>

#include <filesystem>
#include <sstream>

TEST_CASE("ADL_ostreamable")
{
    STATIC_CHECK(iris::req::ADL_ostreamable<char>);
    STATIC_CHECK(iris::req::ADL_ostreamable<std::filesystem::path>);

    {
        struct Incomplete;
        STATIC_CHECK(!iris::req::ADL_ostreamable<Incomplete>);
    }
}

// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/io_fwd.hpp>

#include <filesystem>
#include <ostream>

TEST_CASE("io")
{
    STATIC_CHECK(iris::req::ADL_ostreamable<std::filesystem::path>);
}

// TODO: we need secure "getenv" in iris library
#define _CRT_SECURE_NO_WARNINGS 1

#include "iris_test.hpp"

#include <iris/unicode/string.hpp>
#include <iris/exception.hpp>

#include <stdexcept>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <array>

namespace iris_unicode_test {

constexpr auto INVALID_LINES = std::to_array<unsigned>({
    75, 76, 83, 84, 85, 93, 102, 103, 105, 106, 107, 108, 109,
    110, 114, 115, 116, 117, 124, 125, 130, 135, 140, 145, 153,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 169, 175, 176,
    177, 207, 208, 209, 210, 211, 220, 221, 222, 223, 224, 232,
    233, 234, 235, 236, 247, 248, 249, 250, 251, 252, 253, 257,
    258, 259, 260, 261, 262, 263, 264,
});

TEST_CASE("utf8_invalid")
{
    namespace unicode = iris::unicode;
    using iris::throwf;

    std::filesystem::path const IRIS_ROOT = [] {
        char const* IRIS_ROOT_str = std::getenv("IRIS_ROOT");
        if (!IRIS_ROOT_str) throwf<std::invalid_argument>("IRIS_ROOT is not defined");
        return std::filesystem::path(IRIS_ROOT_str);
    }();

    auto const test_file_path = IRIS_ROOT / "test" / "unicode" / "string" / "test_data" / "utf8_invalid.txt";
    std::ifstream fs8(test_file_path);
    if (!fs8) {
        throwf<std::invalid_argument>("could not open \"{}\"", test_file_path.string());
    }

    // Read it line by line
    unsigned line_count = 0;
    while (!fs8.eof()) {
        std::string line;

        char byte;
        while ((byte = static_cast<char>(fs8.get())) != '\n' && !fs8.eof()) {
            line.push_back(byte);
        }

        ++line_count;
        bool const expected_valid = std::ranges::find(INVALID_LINES, line_count) == INVALID_LINES.end();

        // Print out lines that contain unexpected invalid UTF-8
        if (!unicode::is_valid(line.begin(), line.end())) {
            if (expected_valid) {
                throwf<std::runtime_error>("unexpected invalid utf-8 at line {}", line_count);
            }

            // try fixing it:
            std::string fixed_line;
            unicode::replace_invalid(line.begin(), line.end(), back_inserter(fixed_line));
            if (!unicode::is_valid(fixed_line.begin(), fixed_line.end())) {
                throwf<std::runtime_error>("replace_invalid() resulted in an invalid utf-8 at line {}", line_count);
            }

        } else if (!expected_valid) {
            throwf<std::runtime_error>("invalid utf-8 NOT detected at line {}", line_count);
        }
    }
    CHECK(true);
}

} // iris_unicode_test

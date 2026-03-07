# SPDX-License-Identifier: MIT

set(
    IRIS_TEST_UNICODE_STRING_TESTS
    string
    utf8_invalid
)

foreach(test_name IN LISTS IRIS_TEST_UNICODE_STRING_TESTS)
    iris_define_test(unicode_string_${test_name} ${test_name}.cpp)
    set_target_properties(unicode_string_${test_name}_test PROPERTIES FOLDER "test/unicode/string")
endforeach()

target_sources(unicode_string_utf8_invalid_test PRIVATE test_data/utf8_invalid.txt)

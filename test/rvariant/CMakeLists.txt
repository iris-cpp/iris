# SPDX-License-Identifier: MIT

set(
    IRIS_TEST_RVARIANT_TESTS
    rvariant
    get_visit
    flexible
    recursive_wrapper
    truly_recursive
    io
)

if(IRIS_CI)
    list(APPEND IRIS_TEST_RVARIANT_TESTS many_alternatives_32)
endif()

iris_define_sub_tests(rvariant ${IRIS_TEST_RVARIANT_TESTS})

foreach(test_name IN LISTS IRIS_TEST_RVARIANT_TESTS)
    iris_define_test_headers(rvariant_${test_name} iris_rvariant_test.hpp)
endforeach()

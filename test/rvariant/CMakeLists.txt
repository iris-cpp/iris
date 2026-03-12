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

foreach(test_name IN LISTS IRIS_TEST_RVARIANT_TESTS)
    iris_define_internal_test(rvariant_${test_name} ${test_name}.cpp)
    iris_define_test_headers(rvariant_${test_name} iris_rvariant_test.hpp)
    set_target_properties(rvariant_${test_name}_test PROPERTIES FOLDER "test/rvariant")
endforeach()

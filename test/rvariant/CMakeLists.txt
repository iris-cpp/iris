# SPDX-License-Identifier: MIT

set(
    IRIS_TEST_RVARIANT_TESTS
    rvariant
    many_alternatives_32
    get_visit
    flexible
    recursive_wrapper
    truly_recursive
    io
)

iris_define_sub_tests(rvariant ${IRIS_TEST_RVARIANT_TESTS})

foreach(test_name IN LISTS IRIS_TEST_RVARIANT_TESTS)
    iris_define_test_headers(rvariant_${test_name} iris_rvariant_test.hpp)
endforeach()


# if(MSVC)
#     target_compile_options(
#         Catch2
#         PUBLIC
#             /wd5072 # ASan intentionally enabled on Release build
#             /fsanitize=address
#         PRIVATE
#             /analyze-
#     )
#     target_link_options(
#         Catch2
#         PUBLIC
#             /ignore:4302 # ASan intentionally enabled on Release build
#             /INCREMENTAL:NO # required for ASan
#     )

#     target_compile_options(
#         iris_rvariant_test
#         PUBLIC
#             /W4 /analyze /analyze:external-
#             /fsanitize=address
#     )
# else()
#     target_compile_options(
#         iris_rvariant_test
#         PUBLIC
#             -fsanitize=undefined,address
#     )
#     target_link_options(
#         iris_rvariant_test
#         PUBLIC
#             -fsanitize=undefined,address
#     )
# endif()

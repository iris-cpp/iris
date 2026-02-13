# SPDX-License-Identifier: MIT

# -----------------------------------------------------------------
# Setup the basic targets

# For internal common settings
add_library(_iris_cxx_test_common INTERFACE)
set_target_properties(_iris_cxx_test_common PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(_iris_cxx_test_common INTERFACE _iris_cxx_common)


# For Iris-specific tests
add_library(iris_cxx_test INTERFACE)
set_target_properties(iris_cxx_test PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(iris_cxx_test INTERFACE _iris_cxx_test_common)

# For external libraries. Excludes strict warning, etc.
add_library(iris_cxx_test_external INTERFACE)
set_target_properties(iris_cxx_test_external PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(iris_cxx_test_external INTERFACE _iris_cxx_test_common)

if(MSVC)
    target_compile_options(
        _iris_cxx_test_common
        INTERFACE
            /wd5072 # ASan intentionally enabled on Release build
            /fsanitize=address
    )
    target_link_options(
        _iris_cxx_test_common
        INTERFACE
            /ignore:4302 # ASan intentionally enabled on Release build
            /INCREMENTAL:NO # required for ASan
    )

    target_compile_options(
        iris_cxx_test
        INTERFACE /W4 /analyze /analyze:external-
    )

    target_compile_options(
        iris_cxx_test_external
        INTERFACE /analyze-
    )

else() # non-MSVC
    target_compile_options(
        _iris_cxx_test_common
        INTERFACE
            -fsanitize=undefined,address
    )
    target_link_options(
        _iris_cxx_test_common
        INTERFACE
            -fsanitize=undefined,address
    )

    target_compile_options(
        iris_cxx_test
        INTERFACE
            -Wall -Wextra -pedantic
    )
endif()


# -----------------------------------------------------------------
# Catch2

# https://github.com/catchorg/Catch2/blob/devel/docs/configuration.md
set(CATCH_CONFIG_FAST_COMPILE ON)
set(CATCH_CONFIG_NO_USE_BUILTIN_CONSTANT_P ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.12.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(Catch2)

set_target_properties(Catch2 PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(Catch2 Catch2WithMain PROPERTIES FOLDER "_deps")

target_compile_definitions(Catch2 PUBLIC DO_NOT_USE_WMAIN)
target_link_libraries(Catch2 PRIVATE iris_cxx_test_external)
target_link_libraries(Catch2WithMain PRIVATE iris_cxx_test_external)

target_link_libraries(iris_cxx_test INTERFACE Catch2::Catch2WithMain)


# -----------------------------------------------------------------
# Common CMake utilities for testing

function(iris_define_test_headers test_name)
    target_sources(${test_name}_test PRIVATE FILE_SET HEADERS FILES ${ARGN})
endfunction()

function(iris_define_test test_name)
    add_executable(${test_name}_test ${ARGN})
    target_include_directories(${test_name}_test PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
    target_include_directories(${test_name}_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    set_target_properties(${test_name}_test PROPERTIES CXX_EXTENSIONS OFF)

    if(MSVC)
        # Prevent "Warning: Conflicting <Type> entries detected" error
        set_source_files_properties(
            "${IRIS_ROOT}/iris.natvis"
            TARGET_DIRECTORY ${test_name}_test
            PROPERTIES VS_SETTINGS "ExcludedFromBuild=true"
        )

        target_sources(${test_name}_test PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cpp.hint")
    endif()

    target_link_libraries(${test_name}_test PRIVATE Iris::Iris iris_cxx_test)
    add_test(NAME ${test_name}_test COMMAND ${test_name}_test --colour-mode=ansi)

    if(MSVC)
        get_property(IRIS_MSVC_ASAN_DIR GLOBAL PROPERTY IRIS_MSVC_ASAN_DIR)
        set_tests_properties(
            ${test_name}_test PROPERTIES
            ENVIRONMENT "PATH=${IRIS_MSVC_ASAN_DIR};$ENV{PATH}"
        )
    endif()
endfunction()

function(iris_define_tests)
    foreach(test_name IN LISTS ARGV)
        iris_define_test(${test_name} ${test_name}.cpp)
    endforeach()
endfunction()

function(iris_define_sub_tests prefix)
    foreach(test_name IN LISTS ARGN)
        iris_define_test(${prefix}_${test_name} ${test_name}.cpp)
        set_target_properties(${prefix}_${test_name}_test PROPERTIES FOLDER "test/${prefix}")
    endforeach()
endfunction()


# -----------------------------------------------------------------
# Iris tests

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(rvariant)

    set(
        IRIS_TEST_IRIS_TESTS
        core
        type_traits
        enum
        indirect
        colorize_format
        preprocess
    )

    iris_define_sub_tests(iris ${IRIS_TEST_IRIS_TESTS})

    foreach(test_name IN LISTS IRIS_TEST_IRIS_TESTS)
        iris_define_test_headers(iris_${test_name} iris_test.hpp)
    endforeach()
endif()

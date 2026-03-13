# SPDX-License-Identifier: MIT

# -----------------------------------------------------------------
# Setup the basic targets


# For internal common settings

# For Iris-specific tests
add_library(iris_cxx_test INTERFACE)
set_target_properties(iris_cxx_test PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(iris_cxx_test INTERFACE iris_cxx_abi)

# For external libraries. Excludes strict warning, etc.
add_library(iris_cxx_test_external INTERFACE)
set_target_properties(iris_cxx_test_external PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(iris_cxx_test_external INTERFACE iris_cxx_abi)

if(MSVC)
    target_compile_options(
        iris_cxx_test_external
        INTERFACE /analyze-
    )
endif()

#
# TODO: separate sanitizer and non-sanitizer versions
#
option(IRIS_TEST_USE_SANITIZER "Enable ASan/UBSan" ON)
if(${IRIS_TEST_USE_SANITIZER})
    target_link_libraries(iris_cxx_test INTERFACE iris_cxx_sanitizer)
    target_link_libraries(iris_cxx_test_external INTERFACE iris_cxx_sanitizer)
endif()


# -----------------------------------------------------------------
# Catch2

# https://github.com/catchorg/Catch2/blob/devel/docs/configuration.md
set(CATCH_CONFIG_FAST_COMPILE ON)
set(CATCH_CONFIG_NO_USE_BUILTIN_CONSTANT_P ON)
set(CATCH_CONFIG_DISABLE_EXCEPTIONS ON) # disable default `catch(...)` to enable debugger

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

if(MSVC)
    target_compile_options(Catch2 PRIVATE /wd6054)
endif()

target_link_libraries(Catch2 PRIVATE iris_cxx_test_external)
target_link_libraries(Catch2WithMain PRIVATE iris_cxx_test_external)

target_link_libraries(iris_cxx_test INTERFACE Catch2::Catch2)


# -----------------------------------------------------------------
# Iris internal test targets

add_library(_iris_internal_test INTERFACE)
target_include_directories(_iris_internal_test INTERFACE ${CMAKE_CURRENT_LIST_DIR})

if(MSVC)
    target_sources(_iris_internal_test INTERFACE "${CMAKE_CURRENT_LIST_DIR}/cpp.hint")
endif()

function(iris_define_internal_test test_name)
    iris_define_test(${test_name} ${ARGN})
    target_link_libraries(${test_name}_test PRIVATE _iris_internal_test)
endfunction()


# -----------------------------------------------------------------
# Common CMake utilities for testing

function(iris_define_test_headers test_name)
    target_sources(${test_name}_test PRIVATE FILE_SET HEADERS FILES ${ARGN})
endfunction()

function(_iris_define_test_impl test_name libs)
    get_property(IRIS_ROOT GLOBAL PROPERTY IRIS_ROOT)
    if(NOT DEFINED IRIS_ROOT OR IRIS_ROOT STREQUAL "")
        message(FATAL_ERROR "IRIS_ROOT is not defined")
    endif()

    target_include_directories(${test_name}_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${test_name}_test PRIVATE Iris::Iris iris_cxx_test ${libs})
    set_target_properties(${test_name}_test PROPERTIES CXX_EXTENSIONS OFF)

    if(MSVC)
        # Prevent "Warning: Conflicting <Type> entries detected" error
        set_source_files_properties(
            "${IRIS_ROOT}/iris.natvis"
            TARGET_DIRECTORY ${test_name}_test
            PROPERTIES VS_SETTINGS "ExcludedFromBuild=true"
        )
    endif()


    set_tests_properties(
        ${test_name}_test PROPERTIES
        ENVIRONMENT "IRIS_ROOT=${IRIS_ROOT}"
    )
    if(MSVC)
        set(
            VS_DEBUGGER_ENVIRONMENT_LIST
            "PATH=$(VC_ExecutablePath_x64)\;%PATH%"
            "ASAN_SYMBOLIZER_PATH=$(VC_ExecutablePath_x64)\\llvm-symbolizer.exe"
            "IRIS_ROOT=$<SHELL_PATH:${IRIS_ROOT}>"
        )
        list(JOIN VS_DEBUGGER_ENVIRONMENT_LIST "\n" VS_DEBUGGER_ENVIRONMENT)

        set_target_properties(
            ${test_name}_test PROPERTIES
            VS_DEBUGGER_ENVIRONMENT "${VS_DEBUGGER_ENVIRONMENT}"
        )

        get_property(IRIS_MSVC_ASAN_DIR GLOBAL PROPERTY IRIS_MSVC_ASAN_DIR)
        set(
            ENV_MODIFICATION
            "PATH=path_list_append:${IRIS_MSVC_ASAN_DIR}"
        )
        set_tests_properties(
            ${test_name}_test PROPERTIES
            ENVIRONMENT_MODIFICATION "${ENV_MODIFICATION}"
        )
    endif()
endfunction()

function(_iris_define_executable_test test_name libs)
    add_executable(${test_name}_test ${ARGN})
    add_test(NAME ${test_name}_test COMMAND ${test_name}_test --colour-mode=ansi)
    _iris_define_test_impl(${test_name} ${libs})
endfunction()


# -----------------------------------------------------------------
# Public test adder functions

function(iris_define_test test_name)
    _iris_define_executable_test(${test_name} Catch2::Catch2WithMain ${ARGN})
endfunction()

function(iris_define_test_no_main test_name)
    _iris_define_executable_test(${test_name} Catch2::Catch2 ${ARGN})
endfunction()

function(iris_define_library_test library_type test_name srcs)
    add_library(${test_name}_test ${library_type} ${srcs})
    add_test(NAME ${test_name}_test COMMAND ${ARGN})
    _iris_define_test_impl(${test_name} Catch2::Catch2)
endfunction()


# -----------------------------------------------------------------
# Iris tests

if(PROJECT_IS_TOP_LEVEL)
    if(NOT DEFINED IRIS_CI_COMPONENT OR IRIS_CI_COMPONENT STREQUAL iris)
        add_subdirectory(rvariant)

        set(
            IRIS_TEST_IRIS_TESTS
            core
            type_traits
            enum
            indirect
            colorize_format
            preprocess
            io
        )

        foreach(test_name IN LISTS IRIS_TEST_IRIS_TESTS)
            iris_define_internal_test(iris_${test_name} ${test_name}.cpp)
            iris_define_test_headers(iris_${test_name} iris_test.hpp iris_io_test.hpp)
            set_target_properties(iris_${test_name}_test PROPERTIES FOLDER "test/iris")
        endforeach()

        add_subdirectory(unicode)
    endif()
endif()

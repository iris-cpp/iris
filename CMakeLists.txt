# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.30)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(iris VERSION 0.0.1 LANGUAGES CXX)

if(NOT DEFINED IRIS_ROOT)
    set(IRIS_ROOT "${CMAKE_CURRENT_LIST_DIR}")
endif()

# -----------------------------------------------------------------
# Global settings
# Handle with care, keep these to the ones that can't be
# accomplished by target-specific settings.

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(IRIS_REMOVE_MINSIZEREL_CONFIG "Remove rarely used MinSizeRel config" ON)

if(MSVC)
    if(IRIS_REMOVE_MINSIZEREL_CONFIG OR PROJECT_IS_TOP_LEVEL)
        list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES MinSizeRel)
    endif()

    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH IRIS_MSVC_BIN_DIR)
    set_property(GLOBAL PROPERTY IRIS_MSVC_ASAN_DIR "${IRIS_MSVC_BIN_DIR}")
    unset(IRIS_MSVC_BIN_DIR)
endif()


# -----------------------------------------------------------------
# Create common base targets

# Iris-specific common target
add_library(_iris_cxx_common INTERFACE)
set_target_properties(_iris_cxx_common PROPERTIES CXX_EXTENSIONS OFF)


# -----------------------------------------------------------------
# Create the main Iris target

if(MSVC)
    # This needs to be `OBJECT` target to set correct `/std:` flags on IDE
    add_library(iris OBJECT EXCLUDE_FROM_ALL)
    set_target_properties(iris PROPERTIES LINKER_LANGUAGE CXX)

    target_sources(
        iris
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/cpp.hint"
            "${CMAKE_CURRENT_LIST_DIR}/iris.natvis"
    )

    target_link_libraries(iris PUBLIC _iris_cxx_common)

else()
    add_library(iris INTERFACE)
    target_link_libraries(iris INTERFACE _iris_cxx_common)
endif()

add_library(Iris::Iris ALIAS iris)
set_target_properties(iris PROPERTIES CXX_EXTENSIONS OFF)


# -----------------------------------------------------------------
# Configure C++ version
#
# Set minimal C++ version to `/std:c++XXpreview` and avoid `/std:c++latest`
# if the user explicitly specifies `-DCMAKE_CXX_STANDARD=XX`.

set(IRIS_CXX_VERSION_BEFORE_LATEST 23)
set(IRIS_CXX_VERSION_LATEST 26)

if(DEFINED CMAKE_CXX_STANDARD AND CMAKE_CXX_STANDARD LESS ${IRIS_CXX_VERSION_BEFORE_LATEST})
    message(FATAL_ERROR "Too old C++ version `${CMAKE_CXX_STANDARD}`; the minimal version supported by Iris is `${IRIS_CXX_VERSION_BEFORE_LATEST}`.")
endif()

if(MSVC)
    if(CMAKE_CXX_STANDARD EQUAL ${IRIS_CXX_VERSION_BEFORE_LATEST})
        set(CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_STANDARD_COMPILE_OPTION /std:c++${IRIS_CXX_VERSION_BEFORE_LATEST}preview)
        set(CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_EXTENSION_COMPILE_OPTION /std:c++${IRIS_CXX_VERSION_BEFORE_LATEST}preview)

        set(CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_STANDARD_COMPILE_OPTION ${CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_STANDARD_COMPILE_OPTION} PARENT_SCOPE)
        set(CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_EXTENSION_COMPILE_OPTION ${CMAKE_CXX${IRIS_CXX_VERSION_BEFORE_LATEST}_EXTENSION_COMPILE_OPTION} PARENT_SCOPE)

        target_compile_options(_iris_cxx_common INTERFACE /std:c++${IRIS_CXX_VERSION_BEFORE_LATEST}preview)

    else()
        # MSVC's CMake support does not provide the latest `cxx_std_XX`
        # feature until the very last stage of the implementation. Instead,
        # the feature number that is one version behind the latest usually
        # resolves to `/std:c++latest`.
        target_compile_features(_iris_cxx_common INTERFACE cxx_std_${IRIS_CXX_VERSION_BEFORE_LATEST})
    endif()

else() # Non-MSVC
    if(DEFINED CMAKE_CXX_STANDARD)
        target_compile_features(_iris_cxx_common INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
    else()
        target_compile_features(_iris_cxx_common INTERFACE cxx_std_${IRIS_CXX_VERSION_LATEST})
    endif()
endif()

unset(IRIS_CXX_VERSION_BEFORE_LATEST)
unset(IRIS_CXX_FEATURE_BEFORE_LATEST)


# -----------------------------------------------------------------
# Advanced compile/link settings

if(MSVC)
    # Don't set too strict flags for testing! They must go to `iris_cxx_test`.
    # ABI-dependent configurations MUST be set here.
    target_compile_definitions(
        _iris_cxx_common
        INTERFACE UNICODE _UNICODE
    )
    target_compile_options(
        _iris_cxx_common
        INTERFACE
            /EHsc /MP /utf-8 /Zc:preprocessor /permissive-
            # $<$<CONFIG:Debug,RelWithDebInfo>:/fsanitize=address> # TODO
    )
    target_link_options(
        _iris_cxx_common
        INTERFACE
            # $<$<CONFIG:Debug,RelWithDebInfo>:/INCREMENTAL:NO> # TODO
    )
else()
    if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
        target_compile_options(
            _iris_cxx_common
            INTERFACE
                -fno-builtin-std-forward_like
                -Wno-c++26-extensions # workaround for warnings emitted when using pack indexing
        )
    endif()
    target_compile_options(
        _iris_cxx_common
        INTERFACE
            # $<$<CONFIG:Debug>:-fsanitize=undefined,address> # TODO
    )
    target_link_options(
        _iris_cxx_common
        INTERFACE
            # $<$<CONFIG:Debug>:-fsanitize=undefined,address> # TODO
    )
endif()


# -----------------------------------------------------------------
# Configure Iris main target

file(
    GLOB_RECURSE IRIS_HEADERS
    ${PROJECT_SOURCE_DIR}/include/iris/*.hpp
    ${PROJECT_SOURCE_DIR}/include/iris/*.ipp
)

target_sources(iris PRIVATE FILE_SET HEADERS TYPE HEADERS FILES ${IRIS_HEADERS})
source_group(TREE ${PROJECT_SOURCE_DIR}/include/iris PREFIX iris FILES ${IRIS_HEADERS})
target_include_directories(iris INTERFACE ${PROJECT_SOURCE_DIR}/include)

option(IRIS_CI "Enables intensive testing for CI" OFF)
if(IRIS_CI)
    target_compile_definitions(iris INTERFACE IRIS_CI=1)
endif()

# -----------------------------------------------------------------
# Test

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

if(BUILD_TESTING)
    add_subdirectory(test)

    if(MSVC AND PROJECT_IS_TOP_LEVEL)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Iris::Iris)
    endif()
endif()

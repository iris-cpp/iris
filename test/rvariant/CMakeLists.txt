# SPDX-License-Identifier: MIT

add_executable(
    iris_rvariant_test

    # Independent Polyfill
    indirect_test.cpp

    # Independent language core / exposition-only utils
    core_test.cpp

    # Main library
    rvariant_test.cpp
    many_alternatives_32_test.cpp
    get_visit_test.cpp
    flexible_test.cpp
    recursive_wrapper_test.cpp
    truly_recursive_test.cpp
    io_test.cpp
)

if(MSVC)
    target_sources(iris_rvariant_test PRIVATE cpp.hint)
endif()

target_sources(
    iris_rvariant_test
    PRIVATE FILE_SET HEADERS FILES
        rvariant_test.hpp
)

set_target_properties(
    iris_rvariant_test
    PROPERTIES CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(
        Catch2
        PUBLIC
            /wd5072 # ASan intentionally enabled on Release build
            /fsanitize=address
        PRIVATE
            /analyze-
    )
    target_link_options(
        Catch2
        PUBLIC
            /ignore:4302 # ASan intentionally enabled on Release build
            /INCREMENTAL:NO # required for ASan
    )

    target_compile_options(
        iris_rvariant_test
        PUBLIC
            /W4 /analyze /analyze:external-
            /fsanitize=address
    )
else()
    target_compile_options(
        iris_rvariant_test
        PUBLIC
            -fsanitize=undefined,address
    )
    target_link_options(
        iris_rvariant_test
        PUBLIC
            -fsanitize=undefined,address
    )
endif()

target_link_libraries(
    iris_rvariant_test
    PRIVATE Iris::Iris Catch2::Catch2WithMain
)

add_test(NAME iris_rvariant_test COMMAND iris_rvariant_test)

# ------------------------------------------------
# benchmark

add_library(
    iris_rvariant_benchmark_support
    SHARED benchmark_support.cpp
)
target_sources(
    iris_rvariant_benchmark_support
    PRIVATE FILE_SET HEADERS FILES
        benchmark_support.hpp
)

if(MSVC)
    target_compile_options(
        iris_rvariant_benchmark_support
        PUBLIC
            /W4 /analyze /analyze:external-
            $<$<CONFIG:Release,MinSizeRel>:/Oi> # intrinsic
    )
else() # non-MSVC
    target_compile_options(
        iris_rvariant_benchmark_support
        PUBLIC
            $<$<CONFIG:Release>:-march=native> # intrinsic
    )
endif()

set_target_properties(
    iris_rvariant_benchmark_support
    PROPERTIES CXX_EXTENSIONS OFF
)
target_link_libraries(
    iris_rvariant_benchmark_support
    PRIVATE _iris_cxx_common
)

add_executable(
    iris_rvariant_benchmark
    benchmark.cpp
)
set_target_properties(
    iris_rvariant_benchmark
    PROPERTIES CXX_EXTENSIONS OFF
)
target_link_libraries(
    iris_rvariant_benchmark
    PRIVATE iris_rvariant_benchmark_support
    PRIVATE Iris::Iris
)

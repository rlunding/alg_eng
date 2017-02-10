cmake_minimum_required(VERSION 3.6)
project(alg_eng)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/*.hpp"
        "src/lib/*.hpp"
        )

file(GLOB TEST_FILES
        "test/tests/*.cpp"
        "test/lib/catch.hpp"
        )

add_library(SOURCE ${SOURCE_FILES})

add_executable(main "src/main.cpp")
target_link_libraries(main SOURCE)

add_executable(testing ${TEST_FILES} "test/test.cpp")
target_link_libraries(testing SOURCE)

add_executable(benchmark "benchmark/benchmark.cpp")
target_link_libraries(benchmark SOURCE)

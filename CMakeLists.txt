cmake_minimum_required(VERSION 3.6)
project(alg_eng)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

set(outdir ${CMAKE_CURRENT_SOURCE_DIR}/output)
add_custom_target(build-time-make-dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${outdir})

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp")
file(GLOB TEST_FILES "test/test.cpp" "test/lib/catch.hpp")

add_library(CORE ${SOURCE_FILES})

add_executable(testing ${TEST_FILES})
set_target_properties(testing PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${outdir})
target_link_libraries(testing CORE)

add_executable(benchmark "benchmark/benchmark.cpp")
set_target_properties(benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${outdir})
target_link_libraries(benchmark CORE)

add_executable(datagen "datagen/datagen.cpp")
set_target_properties(datagen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${outdir})
target_link_libraries(datagen CORE)

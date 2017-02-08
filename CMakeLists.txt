
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

aux_source_directory(test test_sources)
aux_source_directory(app app_sources)

find_package(GTest REQUIRED)

include_directories(2stage)

add_executable( 2stage_tests ${test_sources} )


target_link_libraries(2stage_tests GTest::GTest GTest::Main)

add_executable( prime app/example_prime.cpp )
target_link_libraries(prime)

cmake_minimum_required(VERSION 3.0)

project(rpn)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Debug)
include_directories(include)
add_executable(test source/test.cpp)
target_link_libraries(test ${GTEST_LIBRARIES} gtest_main gtest pthread)

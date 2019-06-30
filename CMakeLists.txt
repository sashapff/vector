cmake_minimum_required(VERSION 2.8)

project(vector)

include_directories(${VECTOR_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

add_executable(vector_testing
        vector_testing.cpp
        vector.h
        counted.h
        counted.cpp
        fault_injection.h
        fault_injection.cpp
        gtest/gtest-all.cc
        gtest/gtest.h

        gtest/gtest_main.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

target_link_libraries(vector_testing -lpthread)
add_executable(main main.cpp)


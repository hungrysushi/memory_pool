
cmake_minimum_required(VERSION 3.0)
project(memory_pool)

set(CMAKE_CXX_STANDARD 14)

add_library(memory_pool
        src/memory_pool.cpp
        )
target_include_directories(memory_pool PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)


enable_testing()
add_executable(memory_pool_tests
        test/test_main.cpp
        test/memory_pool_tests.cpp
        )
target_link_libraries(memory_pool_tests
        gtest
        memory_pool
        )

add_test(memory_pool_tests memory_pool_tests)
add_custom_target(run_memory_pool_tests memory_pool_tests)
add_dependencies(run_memory_pool_tests memory_pool_tests)

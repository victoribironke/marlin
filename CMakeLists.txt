cmake_minimum_required(VERSION 3.16)
project(marlin VERSION 0.1.0 LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings for better code quality
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main engine executable
add_executable(marlin
    src/main.cpp
    src/position.cpp
    src/solver.cpp
    src/transposition.cpp
)

# Test executable
add_executable(marlin_tests
    tests/test_position.cpp
    src/position.cpp
)

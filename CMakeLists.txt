cmake_minimum_required(VERSION 3.12)
project(Matrix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(matrix main.cpp)

if(MSVC)
    #target_compile_options(matrix PRIVATE /W4)
    target_compile_options(matrix PRIVATE /std:c++20)
endif()
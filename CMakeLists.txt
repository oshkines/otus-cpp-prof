cmake_minimum_required(VERSION 3.15)
project(MatrixProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(matrix STATIC src/matrix.cpp)

add_executable(matrix_demo src/main.cpp)

target_link_libraries(matrix_demo PRIVATE matrix)

target_include_directories(matrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(matrix PRIVATE /W4)
    target_compile_options(matrix_demo PRIVATE /W4)
else()
    target_compile_options(matrix PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(matrix_demo PRIVATE -Wall -Wextra -pedantic)
endif()
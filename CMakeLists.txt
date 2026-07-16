cmake_minimum_required(VERSION 3.10)

project(mapreduce VERSION 5.7.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/input)

set(INPUT_FILE ${CMAKE_SOURCE_DIR}/input/AB_NYC_2019.csv)
if(EXISTS ${INPUT_FILE})
    file(COPY ${CMAKE_SOURCE_DIR}/input/ DESTINATION ${CMAKE_BINARY_DIR}/input/)
    message(STATUS "Input data copied to build directory")
else()
    message(WARNING "Input file not found: ${INPUT_FILE}")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
endif()

set(COMMON_SOURCES
    src/mapper.cpp
    src/reducer.cpp
    src/statistics.cpp
)

set(HEADERS
    src/csv_parser.hpp
    src/statistics.hpp
)

add_executable(mapreduce 
    src/main.cpp
    src/mapper.cpp
    src/reducer.cpp
    src/statistics.cpp
    ${HEADERS}
)
target_include_directories(mapreduce PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(mapper 
    src/mapper_main.cpp
    src/statistics.cpp
    ${HEADERS}
)
target_include_directories(mapper PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(reducer 
    src/reducer_main.cpp
    src/statistics.cpp
    ${HEADERS}
)
target_include_directories(reducer PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(TARGETS mapreduce mapper reducer
    RUNTIME DESTINATION bin
)
cmake_minimum_required(VERSION 3.15)
project(bayan VERSION 0.1.0)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
    cmake_policy(SET CMP0167 NEW)
endif()
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.26)
    cmake_policy(SET CMP0144 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

set(Boost_NO_BOOST_CMAKE ON)
set(BOOST_ROOT "C:/vcpkg/installed/x64-windows")

find_package(Boost REQUIRED COMPONENTS filesystem system program_options)

add_executable(bayan
    src/main.cpp
    src/DuplicateFinder.cpp
    src/FileHasher.cpp
    src/Options.cpp
)

target_include_directories(bayan PRIVATE src)
target_include_directories(bayan PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(bayan PRIVATE ${Boost_LIBRARIES})

install(TARGETS bayan RUNTIME DESTINATION bin)

if(MSVC)
    target_compile_options(bayan PRIVATE /W4)
else()
    target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic)
endif()

# Генерация debian-файлов из шаблонов (если есть)
set(DEBIAN_TEMPLATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/debian/templates")
set(DEBIAN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/debian")
if(EXISTS ${DEBIAN_TEMPLATE_DIR})
    configure_file(${DEBIAN_TEMPLATE_DIR}/rules ${DEBIAN_OUTPUT_DIR}/rules @ONLY)
    configure_file(${DEBIAN_TEMPLATE_DIR}/control ${DEBIAN_OUTPUT_DIR}/control @ONLY)
    configure_file(${DEBIAN_TEMPLATE_DIR}/changelog ${DEBIAN_OUTPUT_DIR}/changelog @ONLY)
    if(NOT WIN32)
        file(CHMOD ${DEBIAN_OUTPUT_DIR}/rules PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
    endif()
endif()
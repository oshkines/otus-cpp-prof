cmake_minimum_required(VERSION 3.15)
project(bulk VERSION 0.4.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Генерация debian-файлов из шаблонов
set(DEBIAN_TEMPLATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/debian/templates")
set(DEBIAN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/debian")

set(DEBIAN_TEMPLATES control changelog rules)

foreach(TEMPLATE ${DEBIAN_TEMPLATES})
    configure_file(
        "${DEBIAN_TEMPLATE_DIR}/${TEMPLATE}"
        "${DEBIAN_OUTPUT_DIR}/${TEMPLATE}"
        @ONLY
    )
endforeach()

if(EXISTS "${DEBIAN_OUTPUT_DIR}/rules")
    file(CHMOD "${DEBIAN_OUTPUT_DIR}/rules" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

add_executable(bulk
    src/main.cpp
    src/CommandReader.cpp
    src/BlockProcessor.cpp
    src/BlockHandler.cpp
)

target_include_directories(bulk PRIVATE src)

# Настройки компилятора
if(MSVC)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic)
endif()
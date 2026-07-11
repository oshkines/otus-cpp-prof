cmake_minimum_required(VERSION 3.15)
project(sqlite3server VERSION 5.3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(SQLite3 REQUIRED)

add_executable(sqlite3server
    src/main.cpp
    src/DatabaseManager.cpp
    src/CommandParser.cpp
)

target_include_directories(sqlite3server
    PRIVATE
        src
        ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(sqlite3server
    PRIVATE
        ${Boost_LIBRARIES}
        ${SQLite3_LIBRARIES}
)


if(MSVC)
    target_compile_options(sqlite3server PRIVATE /W4)
else()
    target_compile_options(sqlite3server PRIVATE -Wall -Wextra -pedantic)
endif()


install(TARGETS sqlite3server
    RUNTIME DESTINATION bin
)

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
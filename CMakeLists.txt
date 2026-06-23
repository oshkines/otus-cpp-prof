cmake_minimum_required(VERSION 3.15)
project(async VERSION 0.4.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(async SHARED
    src/async/async.cpp
)

if(WIN32)
    target_compile_definitions(async PRIVATE ASYNC_EXPORT)
endif()

target_include_directories(async
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/async
)

target_include_directories(async
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/async
)

set_target_properties(async PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

add_executable(bulk
    src/main.cpp
)

target_link_libraries(bulk
    PRIVATE async
)

target_include_directories(bulk
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(MSVC)
    target_compile_options(async PRIVATE /W4)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(async PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS async
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/async
)

install(FILES include/async/async.h
    DESTINATION include/async
)

install(TARGETS bulk
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
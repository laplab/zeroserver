cmake_minimum_required(VERSION 3.10)
project(zeroserver)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_GENERATOR STREQUAL "Ninja")
    # Turn on colored output. https://github.com/ninja-build/ninja/wiki/FAQ
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

set(ZEROSERVER_PROJECT_DIR ${PROJECT_SOURCE_DIR})
set(ZEROSERVER_CONTRIB_DIR "${ZEROSERVER_PROJECT_DIR}/contrib")

# Contrib
add_subdirectory(${ZEROSERVER_CONTRIB_DIR})

# FIXME: no idea why it is not in cpp_redis cmake file
target_include_directories(cpp_redis PUBLIC
    "${ZEROSERVER_CONTRIB_DIR}/cpp_redis/includes"
    "${ZEROSERVER_CONTRIB_DIR}/cpp_redis/tacopie/includes"
)

# Sources
add_subdirectory(src)

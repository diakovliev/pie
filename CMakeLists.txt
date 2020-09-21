set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-overloaded-virtual")

# set(CMAKE_C_COMPILER /usr/bin/gcc)
# set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_C_COMPILER /usr/bin/gcc-8)
set(CMAKE_CXX_COMPILER /usr/bin/g++-8)
# set(CMAKE_C_COMPILER ccache /usr/bin/clang)
# set(CMAKE_CXX_COMPILER ccache /usr/bin/clang++)

cmake_minimum_required(VERSION 2.6)
project(pie)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

include_directories("include" "logger" "piel", "commands", "pie")

message ("CMAKE_BUILD_DIRECTORY: ${CMAKE_BUILD_DIRECTORY}")
message ("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")
message ("CMAKE_FILES_DIRECTORY : ${CMAKE_FILES_DIRECTORY}")

add_definitions(-DPIE_VERSION="$ENV{PIE_VERSION}")
message(STATUS "!!! PIE_VERSION: $ENV{PIE_VERSION}")

add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3)
add_definitions(-DBOOST_NO_CXX11_SCOPED_ENUMS)

set(Boost_libraries_list program_options)

add_subdirectory(parsers)
add_subdirectory(logger)
add_subdirectory(piel)
add_subdirectory(commands)
add_subdirectory(pie)
add_subdirectory(python_pie)

enable_testing()
add_subdirectory(tests)

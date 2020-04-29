#set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/boost")
#set (BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

#set(CMAKE_C_COMPILER /usr/bin/clang)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-overloaded-virtual")

set(CMAKE_C_COMPILER ccache /usr/bin/gcc)
set(CMAKE_CXX_COMPILER ccache /usr/bin/g++)

cmake_minimum_required(VERSION 2.6)
project(pie)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_BUILD_TYPE Debug)

include_directories("logger" "piel", "commands", "pie")

message ("CMAKE_BUILD_DIRECTORY: ${CMAKE_BUILD_DIRECTORY}")
message ("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")
message ("CMAKE_FILES_DIRECTORY : ${CMAKE_FILES_DIRECTORY}")

add_definitions(-DPIE_VERSION="$ENV{PIE_VERSION}")
message(STATUS "!!! PIE_VERSION: $ENV{PIE_VERSION}")

add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3)
add_definitions(-DBOOST_NO_CXX11_SCOPED_ENUMS)

set(Boost_libraries_list program_options system filesystem regex thread)

add_subdirectory(logger)
add_subdirectory(piel)
add_subdirectory(commands)
add_subdirectory(pie)

enable_testing()
add_subdirectory(tests)

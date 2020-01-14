#set (BOOST_ROOT /home/okogutenko/projects/boost/boost_1_58_0)
#set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/boost")
#set (BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

#set(CMAKE_C_COMPILER /usr/bin/clang)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-overloaded-virtual")

#set(CMAKE_C_COMPILER /usr/bin/gcc-5)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-5)

#include(CheckCXXCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("--std=c++11" COMPILER_SUPPORTS_CXX11)
#CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
#if(COMPILER_SUPPORTS_CXX11)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
#elseif(COMPILER_SUPPORTS_CXX0X)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()

cmake_minimum_required(VERSION 2.6)
project(pie)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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

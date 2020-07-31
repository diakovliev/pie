set(Module_name python3_pie)
file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")

set(Boost_libraries_list python3)
set(Python_INCLUDE_DIRS /usr/include/python3.6)

# Boost (libboost-all-dev)
find_package( Boost COMPONENTS ${Boost_libraries_list} REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${Python_INCLUDE_DIRS})

add_library(${Module_name} SHARED ${SOURCES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.py
    ${CMAKE_CURRENT_BINARY_DIR}/test.py COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python_pie.py
    ${CMAKE_CURRENT_BINARY_DIR}/python_pie.py COPYONLY)

# Components
target_link_libraries(${Module_name} base_extension commands piel logger pthread)

target_link_libraries(${Module_name} LINK_PUBLIC stdc++fs ${Boost_LIBRARIES})
# target_link_libraries(${Module_name} LINK_PUBLIC
#     stdc++fs
#     libboost_python.a
# )

# OpenSSL (libssl-dev)
# target_link_libraries(${Module_name} crypto)
target_link_libraries(${Module_name} libcrypto.a)

# libcurl (libcurl4-gnutls-dev)
target_link_libraries(${Module_name} curl-gnutls)

# Static c++
# set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

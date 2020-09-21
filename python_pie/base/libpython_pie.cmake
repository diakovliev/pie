file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")

# find_package(Boost COMPONENTS ${Boost_libraries_list} REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${Python_INCLUDE_DIRS})

add_library(${Module_name} SHARED ${SOURCES})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../base/test.py
    ${CMAKE_CURRENT_BINARY_DIR}/test.py
    COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../base/python_pie.py
    ${CMAKE_CURRENT_BINARY_DIR}/python_pie.py
    COPYONLY)

# Components
target_link_libraries(${Module_name} python_pie_base commands piel parsers logger pthread)

target_link_libraries(${Module_name} LINK_PUBLIC stdc++fs ${Boost_LIBRARIES})
# target_link_libraries(${Module_name} LINK_PUBLIC
#      stdc++fs
#      libboost_python.a
# )

# OpenSSL (libssl-dev)
target_link_libraries(${Module_name} crypto)
# target_link_libraries(${Module_name} libcrypto.a)

# libcurl (libcurl4-gnutls-dev)
target_link_libraries(${Module_name} curl-gnutls)

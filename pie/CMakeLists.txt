set(Module_name pie)

file(GLOB SOURCES "*.cpp")
add_executable(${Module_name} ${SOURCES})

# Components
target_link_libraries(${Module_name} commands piel logger pthread)

# Externals
#
# Boost (libboost-all-dev)
find_package( Boost COMPONENTS ${Boost_libraries_list} REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#target_link_libraries(${Module_name} LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${Module_name} LINK_PUBLIC
    libboost_program_options.a
    libboost_system.a
    libboost_filesystem.a
    libboost_regex.a
    libboost_thread.a
)

# OpenSSL (libssl-dev)
target_link_libraries(${Module_name} crypto)

# libzip (libzip-dev)
#target_link_libraries(${Module_name} ${ZIP})
#include_directories("third-party/libzip/include")

# libcurl (libcurl4-openssl-dev)
# target_link_libraries(${Module_name} curl)

# libcurl (libcurl4-gnutls-dev)
target_link_libraries(${Module_name} curl-gnutls)

# Static c++
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

#install(TARGETS pie RUNTIME DESTINATION bin)

set(Module_name depman)

file(GLOB SOURCES "*.cpp")

cmake_policy(SET CMP0015 NEW)
link_directories(../third-party/sgscript/lib)

add_executable(${Module_name} ${SOURCES})

# Externals
#
# Boost (libboost-all-dev)
find_package( Boost COMPONENTS ${Boost_libraries_list} REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ../third-party/sgscript/include)
target_link_libraries(${Module_name} LINK_PUBLIC ${Boost_LIBRARIES} sgscript dl)


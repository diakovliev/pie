find_package(RAGEL)

RAGEL_TARGET(
    gavc_query
    gavc_query.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/gavc_query.cpp
)

RAGEL_TARGET(
    gavc_query_version
    gavc_query_version.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/gavc_query_version.cpp
)

RAGEL_TARGET(
    gavc_query_versions_range
    gavc_query_versions_range.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/gavc_query_versions_range.cpp
)

RAGEL_TARGET(
    uploadfilesspec
    uploadfilesspec.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/uploadfilesspec.cpp
)

set(Module_name parsers)

file(GLOB SOURCES ${RAGEL_gavc_query_OUTPUTS} ${RAGEL_gavc_query_version_OUTPUTS} ${RAGEL_gavc_query_versions_range_OUTPUTS} ${RAGEL_uploadfilesspec_OUTPUTS})

# message("SOURCES: ${SOURCES}")

# add_library(${Module_name} STATIC ${SOURCES})
add_library(${Module_name} STATIC  ${RAGEL_gavc_query_OUTPUTS} ${RAGEL_gavc_query_version_OUTPUTS} ${RAGEL_gavc_query_versions_range_OUTPUTS} ${RAGEL_uploadfilesspec_OUTPUTS})

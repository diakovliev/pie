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
    uploadfilesspec
    uploadfilesspec.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/uploadfilesspec.cpp
)
RAGEL_TARGET(
    properties
    properties.cpp.rl
    ${CMAKE_CURRENT_BINARY_DIR}/properties.cpp
)

set(Module_name parsers)

add_library(${Module_name} STATIC ${RAGEL_gavc_query_OUTPUTS} ${RAGEL_gavc_query_version_OUTPUTS} ${RAGEL_uploadfilesspec_OUTPUTS} ${RAGEL_properties_OUTPUTS})

find_package (Boost COMPONENTS ${Boost_libraries_list} random unit_test_framework REQUIRED)

set(Piel_LIBRARIES commands piel logger crypto curl pthread ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIR})

#############################################################
add_executable(uploadfilesspec_test uploadfilesspec_test.cpp)
target_link_libraries(uploadfilesspec_test ${Piel_LIBRARIES})
add_test (NAME UploadFilesSpecTest COMMAND uploadfilesspec_test)

#############################################################
add_executable(outputstream_test outputstream_test.cpp)
target_link_libraries(outputstream_test ${Piel_LIBRARIES})
add_test (NAME OutputStreamTests COMMAND outputstream_test)

#############################################################
add_executable(art_test art_test.cpp)
target_link_libraries(art_test ${Piel_LIBRARIES})
#add_test (NAME ArtTests COMMAND art_test)

#############################################################
add_executable(properties properties.cpp)
target_link_libraries(properties ${Piel_LIBRARIES})
add_test (NAME Properties COMMAND properties)
set_tests_properties(Properties
    PROPERTIES ENVIRONMENT "test_env_var=test_env_var_default_value")

#############################################################
add_executable(gavc_query_parsing gavc_query_parsing.cpp)
target_link_libraries(gavc_query_parsing ${Piel_LIBRARIES})
add_test (NAME GacvQueryParsing COMMAND gavc_query_parsing)

#############################################################
add_executable(maven_metadata_test maven_metadata_test.cpp)
target_link_libraries(maven_metadata_test ${Piel_LIBRARIES})
add_test (NAME MavenMetadataTest COMMAND maven_metadata_test)

#############################################################
add_executable(gavc_versions_filter gavc_versions_filter.cpp)
target_link_libraries(gavc_versions_filter ${Piel_LIBRARIES})
add_test (NAME GavcVersionsFilterTest COMMAND gavc_versions_filter)

#############################################################
add_executable(gavc_versions_matcher gavc_versions_matcher.cpp)
target_link_libraries(gavc_versions_matcher ${Piel_LIBRARIES})
add_test (NAME GavcVersionsMatcherTest COMMAND gavc_versions_matcher)

#############################################################
add_executable(gavc_versions_comparator gavc_versions_comparator.cpp)
target_link_libraries(gavc_versions_comparator ${Piel_LIBRARIES})
add_test (NAME GavcVersionsComparatorTest COMMAND gavc_versions_comparator)

#############################################################
add_executable(gavc_cache gavc_cache.cpp)
target_link_libraries(gavc_cache ${Piel_LIBRARIES})
add_test (NAME GAVCCache COMMAND gavc_cache)

include(FindPkgConfig)

pkg_check_modules(PYTHON2 REQUIRED python2)

message(" --- python2 includes: ${PYTHON2_INCLUDE_DIRS}")

set(Module_name python2_pie)
set(Python_INCLUDE_DIRS ${PYTHON2_INCLUDE_DIRS})

set(Boost_libraries_list python2)
find_package(Boost COMPONENTS ${Boost_libraries_list})
if (NOT Boost_PYTHON2_FOUND)
    message(" --- python2 fallback to python !!!")
    set(Boost_libraries_list python)
    find_package(Boost COMPONENTS ${Boost_libraries_list})
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../base/libpython_pie.cmake)

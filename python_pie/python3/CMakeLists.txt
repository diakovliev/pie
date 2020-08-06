include(FindPkgConfig)

pkg_check_modules(PYTHON3 REQUIRED python3)

message(" --- python3 includes: ${PYTHON3_INCLUDE_DIRS}")

set(Module_name python3_pie)
set(Python_INCLUDE_DIRS ${PYTHON3_INCLUDE_DIRS})

set(Boost_DEBUG ON)

set(Boost_libraries_list python3)
find_package(Boost COMPONENTS ${Boost_libraries_list})
if (NOT Boost_PYTHON3_FOUND)
    message(" --- python3 fallback to python-py35 !!!")
    set(Boost_libraries_list python-py35)
    find_package(Boost COMPONENTS ${Boost_libraries_list})
    if (NOT Boost_PYTHON-PY35_FOUND)
        message(" --- python3 fallback to python !!!")
        set(Boost_libraries_list python)
        find_package(Boost COMPONENTS ${Boost_libraries_list})
    endif()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../base/libpython_pie.cmake)

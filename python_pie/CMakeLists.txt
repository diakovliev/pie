include(FindPkgConfig)

include_directories("base")

add_subdirectory(base)

pkg_check_modules(PYTHON2 python2)
if(PYTHON2_FOUND)
  add_subdirectory(python2)
endif()

add_subdirectory(python3)

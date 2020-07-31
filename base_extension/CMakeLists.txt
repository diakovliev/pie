set(Module_name base_extension)
file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")

add_library(${Module_name} STATIC ${SOURCES})

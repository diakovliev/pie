set(Module_name piel)
file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")

add_library(${Module_name} STATIC ${SOURCES})

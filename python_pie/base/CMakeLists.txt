set(Module_name python_pie_base)
file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")

add_library(${Module_name} STATIC ${SOURCES})

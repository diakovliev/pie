set(Module_name logger)
file(GLOB SOURCES "*.cpp" "*.h" "logger/*.cpp" "logger/*.h" "logger/app/*.cpp" "logger/app/*.h" "logger/out/*.cpp" "logger/out/*.h" "logger/dispatcher/*.cpp" "logger/dispatcher/*.h")

add_library(${Module_name} STATIC ${SOURCES})

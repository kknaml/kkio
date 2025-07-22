

file(GLOB_RECURSE KKIO_UTIL_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_UTIL_MODULES "src/*.ixx")

add_library(kkio-util STATIC ${KKIO_UTIL_SOURCES})
target_sources(kkio-util PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${KKIO_UTIL_MODULES}
)


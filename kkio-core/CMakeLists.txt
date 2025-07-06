

file(GLOB_RECURSE KKIO_CORE_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_CORE_MODULES "src/*.ixx")

add_library(kkio-core STATIC ${KKIO_CORE_SOURCES})
target_sources(kkio-core PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${KKIO_CORE_MODULES}
)

target_link_libraries(kkio-core PUBLIC uring pthread)
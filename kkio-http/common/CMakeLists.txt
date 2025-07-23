

file(GLOB_RECURSE KKIO_HTTP_COMMON_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_HTTP_COMMON_MODULES "src/*.ixx")

add_library(kkio-http-common STATIC ${KKIO_HTTP_COMMON_SOURCES})
target_sources(kkio-http-common PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${KKIO_HTTP_COMMON_MODULES}
)

target_link_libraries(kkio-http-common PUBLIC kkio-core kkio-tls kkio-util)


file(GLOB_RECURSE KKIO_HTTP_CLIENT_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_HTTP_CLIENT_MODULES "src/*.ixx")

add_library(kkio-http-client STATIC ${KKIO_HTTP_CLIENT_SOURCES})
target_sources(kkio-http-client PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${KKIO_HTTP_CLIENT_MODULES}
)

target_link_libraries(kkio-http-client PUBLIC kkio-http-common)

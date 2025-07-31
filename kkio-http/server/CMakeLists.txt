

file(GLOB_RECURSE KKIO_HTTP_SERVER_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_HTTP_SERVER_MODULES "src/*.ixx")

add_library(kkio-http-server STATIC ${KKIO_HTTP_SERVER_SOURCES})
target_sources(kkio-http-server PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${KKIO_HTTP_SERVER_MODULES}
)

target_link_libraries(kkio-http-server PUBLIC kkio-http-common)



file(GLOB_RECURSE KKIO_TLS_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE KKIO_TLS_MODULES "src/*.ixx")

add_library(kkio-tls STATIC ${KKIO_TLS_SOURCES})
target_sources(kkio-tls PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${KKIO_TLS_MODULES}
)

target_link_libraries(kkio-tls PUBLIC kkio-core ssl)
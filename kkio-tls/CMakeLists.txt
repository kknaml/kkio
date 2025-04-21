file(GLOB_RECURSE KKP_TLS_SOURCES src/*.cpp test/*.c)
file(GLOB_RECURSE KKP_TLS_MODULES src/*.ixx)

add_library(kkio-tls STATIC ${KKP_TLS_SOURCES})
target_sources(kkio-tls PUBLIC FILE_SET CXX_MODULES TYPE CXX_MODULES FILES ${KKP_TLS_MODULES})

set(BOTAN_INCLUDE_DIR "/usr/local/include/botan-3")
set(BOTAN_LIBRARY "/usr/local/lib/libbotan-3.so")
add_library(Botan::Botan SHARED IMPORTED)
set_target_properties(Botan::Botan PROPERTIES
    IMPORTED_LOCATION "${BOTAN_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${BOTAN_INCLUDE_DIR}"
)
target_link_options(kkio-tls PUBLIC)

target_link_libraries(kkio-tls PUBLIC kkio-core Botan::Botan)
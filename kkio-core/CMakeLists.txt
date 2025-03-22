
file(GLOB_RECURSE KKP_SOURCES src/*.cpp test/*.c)
file(GLOB_RECURSE KKP_MODULES src/*.ixx)

add_library(kkio-core STATIC ${KKP_SOURCES})
target_sources(kkio-core PUBLIC FILE_SET CXX_MODULES TYPE CXX_MODULES FILES ${KKP_MODULES})
target_link_libraries(kkio-core PUBLIC uring)

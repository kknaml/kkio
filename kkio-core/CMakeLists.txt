
file(GLOB_RECURSE KKP_SOURCES 
    "src/*.cpp"
    "test/*.c"
)
list(FILTER KKP_SOURCES EXCLUDE REGEX ".*platform/.*")

file(GLOB_RECURSE KKP_MODULES "src/*.ixx")
list(FILTER KKP_MODULES EXCLUDE REGEX ".*platform/.*")

if (WIN32)
    file(GLOB_RECURSE PLATFORM_SOURCES "src/platform/windows/*.cpp")
    file(GLOB_RECURSE PLATFORM_MODULES "src/platform/windows/*.ixx" "src/platform/mod.ixx")
else()
    file(GLOB_RECURSE PLATFORM_SOURCES "src/platform/linux/*.cpp")
    file(GLOB_RECURSE PLATFORM_MODULES "src/platform/linux/*.ixx" "src/platform/mod.ixx")
endif()

add_library(kkio-core STATIC ${KKP_SOURCES} ${PLATFORM_SOURCES})
target_sources(kkio-core 
    PUBLIC
        FILE_SET CXX_MODULES 
        TYPE CXX_MODULES 
        FILES ${KKP_MODULES} ${PLATFORM_MODULES}
)

if (WIN32)

else()
    target_link_libraries(kkio-core PUBLIC uring)

endif ()


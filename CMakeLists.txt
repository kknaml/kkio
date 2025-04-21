cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

# if clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using Clang compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -lc++")
    add_link_options("-lc++ -lc++abi")
endif ()

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
project(kkio LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_MODULE_STD 1)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD ON)





add_subdirectory(kkio-core)
add_subdirectory(kkio-tls)
add_subdirectory(kkio-http)
add_subdirectory(test)
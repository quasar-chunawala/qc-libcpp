cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)              # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Error, if the compiler doesn't support it

include(FetchContent)

fetchcontent_declare(googletest
    GIT_REPOSITORY 
        https://github.com/google/googletest.git
)

fetchcontent_makeavailable(googletest)

file(GLOB_RECURSE CORE_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(cppx_tests STATIC)

target_sources(core
    PRIVATE 
        ${CORE_SOURCES}
    PUBLIC 
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${CORE_HEADERS}
)

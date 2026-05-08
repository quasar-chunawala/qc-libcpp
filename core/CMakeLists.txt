cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_CXX_STANDARD 23)              # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Error, if the compiler doesn't support it

file(GLOB_RECURSE CORE_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(core STATIC)

target_sources(core
    PRIVATE 
        ${CORE_SOURCES}
    PUBLIC 
        FILE_SET HEADERS
        BASE_DIRS "${CPPX_ROOT}"
        FILES ${CORE_HEADERS}
)



# Set compiler flags

message("setting compiler flags")

# General GNU Debug options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -v -w -std=c++23")
    # -g3 Builds the binaries with debugging symbols
    # -O0 No optimization, faster compile time
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0") 
endif()

set(GCC_ROOT "$ENV{GCC_ROOT}" CACHE STRING "gcc root directory")

# Specific compiler options for debug and release
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0")
        message(FATAL_ERROR "GCC versions older than 11 are not maintained")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "20.0")
            message(FATAL_ERROR "clang/LLVM versions older than 20 are not maintained")
        endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.51")
            message(FATAL_ERROR "MSVC versions older than 19.51 are not maintained")
        endif()
endif()

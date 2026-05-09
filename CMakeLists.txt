cmake_minimum_required(VERSION 4.0)

project(cppx VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(set_cppx_dirs)
include(set_compiler_flags)

# #################################################################
# Global Settings
# #################################################################
set(CMAKE_CXX_STANDARD 23) # -std=c++23
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Full C++26 support is needed


# #################################################################
# Project components
# #################################################################
add_subdirectory(core)
add_subdirectory(tests)
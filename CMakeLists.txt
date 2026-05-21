cmake_minimum_required(VERSION 3.3...4.0)

project(qc_libcpp VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(set_libcpp_dirs)
include(clang_tidy)

# #################################################################
# Global Settings
# #################################################################
set(CMAKE_CXX_STANDARD 20) # -std=c++23
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Full C++26 support is needed


option(ENABLE_CLANG_TIDY "" OFF)
# set(ENABLE_CLANG_TIDY ON)

# #################################################################
# Project components
# #################################################################
add_subdirectory(core)
add_subdirectory(tests)

# #################################################################
# Enable clang-tidy
# #################################################################
enable_clang_tidy_for(core)
cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_CXX_STANDARD 23)              # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Error, if the compiler doesn't support it

include(FetchContent)

fetchcontent_declare(googletest
    GIT_REPOSITORY 
        https://github.com/google/googletest.git
    GIT_TAG main
)

fetchcontent_makeavailable(googletest)

file(GLOB_RECURSE TESTS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE TESTS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(cppx_tests)

target_sources(cppx_tests
    PRIVATE 
        ${TESTS_SOURCES}
        ${TESTS_HEADERS}
)

target_link_libraries(cppx_tests
    PRIVATE
        core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cppx_tests)
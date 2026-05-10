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


message("TESTS_ROOT : ${TESTS_ROOT}")

set(TESTS_HEADERS
    ""
)

set(TESTS_SOURCES
    "${TESTS_ROOT}/forward_list_test/forward_list_test.cpp"
    "${TESTS_ROOT}/shared_ptr_test/shared_ptr_test.cpp"
    "${TESTS_ROOT}/spsc_queue_test/spsc_queue_test.cpp"
    "${TESTS_ROOT}/string_test/string_test.cpp"
    "${TESTS_ROOT}/threadsafe_queue_test/threadsafe_queue_test.cpp"
    "${TESTS_ROOT}/threadsafe_stack_test/threadsafe_stack_test.cpp"
    "${TESTS_ROOT}/unique_ptr_test/unique_ptr_test.cpp"
    "${TESTS_ROOT}/variant_test/variant_test.cpp"
    "${TESTS_ROOT}/vector_test/vector_test.cpp"
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


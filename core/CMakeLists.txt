cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_CXX_STANDARD 23)              # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Error, if the compiler doesn't support it

set(CORE_SOURCES
    "${CPPX_CORE_ROOT}/string/string.cpp"
)

set(CORE_HEADERS
    "${CPPX_CORE_ROOT}/concurrency/threadsafe_queue.h"
    "${CPPX_CORE_ROOT}/concurrency/threadsafe_stack.h"
    "${CPPX_CORE_ROOT}/containers/forward_list.h"
    "${CPPX_CORE_ROOT}/containers/vector.h"
    "${CPPX_CORE_ROOT}/lock_free/spsc_queue.h"
    "${CPPX_CORE_ROOT}/smart_pointers/shared_ptr.h"
    "${CPPX_CORE_ROOT}/smart_pointers/unique_ptr.h"
    "${CPPX_CORE_ROOT}/string/string.h"
    "${CPPX_CORE_ROOT}/tagged_union/variant.h"
    "${CPPX_CORE_ROOT}/tagged_union/visit.h"
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



cmake_minimum_required(VERSION 3.3...4.0)

set(CMAKE_CXX_STANDARD 20)              # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Error, if the compiler doesn't support it

set(CORE_SOURCES
    "${LIBCPP_CORE_ROOT}/string/string.cpp"
)

set(CORE_HEADERS
    "${LIBCPP_CORE_ROOT}/compat.h"
    "${LIBCPP_CORE_ROOT}/concurrency/threadsafe_queue.h"
    "${LIBCPP_CORE_ROOT}/concurrency/threadsafe_stack.h"
    "${LIBCPP_CORE_ROOT}/containers/forward_list.h"
    "${LIBCPP_CORE_ROOT}/containers/vector.h"
    "${LIBCPP_CORE_ROOT}/lock_free/spsc_queue.h"
    "${LIBCPP_CORE_ROOT}/smart_pointers/shared_ptr.h"
    "${LIBCPP_CORE_ROOT}/smart_pointers/unique_ptr.h"
    "${LIBCPP_CORE_ROOT}/string/string.h"
    "${LIBCPP_CORE_ROOT}/tagged_union/variant.h"
    "${LIBCPP_CORE_ROOT}/tagged_union/visit.h"
)

add_library(core SHARED)

target_sources(core
    PRIVATE 
        ${CORE_SOURCES}
    PUBLIC 
        FILE_SET HEADERS
        BASE_DIRS "${LIBCPP_ROOT}"
        FILES ${CORE_HEADERS}
)



cmake_minimum_required(VERSION 3.21)

project(
    stdx
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A header-only C++ library that polyfills the standard library."
)

include(cmake/get_cpm.cmake)
if(PROJECT_IS_TOP_LEVEL)
    cpmaddpackage("gh:intel/cicd-repo-infrastructure#dev")
else()
    cpmaddpackage("gh:intel/cicd-repo-infrastructure#3e2bef0")
endif()

add_versioned_package("gh:boostorg/mp11#boost-1.83.0")
fmt_recipe(11.1.3)
add_versioned_package("gh:intel/cpp-baremetal-concurrency#06e5901")

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

add_library(stdx INTERFACE)
target_compile_features(stdx INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
target_compile_options(
    stdx
    INTERFACE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-missing-braces>
)
target_link_libraries_system(stdx INTERFACE concurrency boost_mp11
                             fmt::fmt-header-only)

target_sources(
    stdx
    INTERFACE FILE_SET
              stdx
              TYPE
              HEADERS
              BASE_DIRS
              include
              FILES
              include/stdx/algorithm.hpp
              include/stdx/atomic.hpp
              include/stdx/atomic_bitset.hpp
              include/stdx/bit.hpp
              include/stdx/bitset.hpp
              include/stdx/byterator.hpp
              include/stdx/cached.hpp
              include/stdx/call_by_need.hpp
              include/stdx/compiler.hpp
              include/stdx/concepts.hpp
              include/stdx/ct_conversions.hpp
              include/stdx/ct_format.hpp
              include/stdx/ct_string.hpp
              include/stdx/cx_map.hpp
              include/stdx/cx_multimap.hpp
              include/stdx/cx_queue.hpp
              include/stdx/cx_set.hpp
              include/stdx/cx_vector.hpp
              include/stdx/detail/bitset_common.hpp
              include/stdx/detail/list_common.hpp
              include/stdx/env.hpp
              include/stdx/for_each_n_args.hpp
              include/stdx/function_traits.hpp
              include/stdx/functional.hpp
              include/stdx/intrusive_forward_list.hpp
              include/stdx/intrusive_list.hpp
              include/stdx/iterator.hpp
              include/stdx/latched.hpp
              include/stdx/memory.hpp
              include/stdx/numeric.hpp
              include/stdx/optional.hpp
              include/stdx/panic.hpp
              include/stdx/pp_map.hpp
              include/stdx/priority.hpp
              include/stdx/ranges.hpp
              include/stdx/rollover.hpp
              include/stdx/span.hpp
              include/stdx/static_assert.hpp
              include/stdx/tuple.hpp
              include/stdx/tuple_algorithms.hpp
              include/stdx/tuple_destructure.hpp
              include/stdx/type_traits.hpp
              include/stdx/udls.hpp
              include/stdx/utility.hpp)

if(PROJECT_IS_TOP_LEVEL)
    add_docs(docs)
    add_subdirectory(test)
    clang_tidy_interface(stdx)
endif()

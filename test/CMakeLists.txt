add_unit_test(
    stdx_test
    CATCH2
    FILES
    always_false.cpp
    bit.cpp
    bitset.cpp
    callable.cpp
    concepts.cpp
    conditional.cpp
    function_traits.cpp
    intrusive_list.cpp
    overload.cpp
    priority.cpp
    remove_cvref.cpp
    to_underlying.cpp
    with_result_of.cpp
    udls.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    ct_conversions_test
    CATCH2
    FILES
    ct_conversions.cpp
    LIBRARIES
    warnings
    stdx)

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_unit_test(
        ct_string_test
        CATCH2
        FILES
        ct_string.cpp
        LIBRARIES
        warnings
        stdx)
endif()

add_unit_test(
    cx_map_test
    CATCH2
    FILES
    cx_map.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    cx_multimap_test
    CATCH2
    FILES
    cx_multimap.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    cx_queue_test
    CATCH2
    FILES
    cx_queue.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    cx_set_test
    CATCH2
    FILES
    cx_set.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    cx_vector_test
    CATCH2
    FILES
    cx_vector.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    default_panic_test
    CATCH2
    FILES
    default_panic.cpp
    LIBRARIES
    warnings
    stdx)

add_unit_test(
    panic_test
    CATCH2
    FILES
    panic.cpp
    LIBRARIES
    warnings
    stdx)

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_unit_test(
        tuple_test
        CATCH2
        FILES
        indexed_tuple.cpp
        tuple.cpp
        tuple_algorithms.cpp
        LIBRARIES
        warnings
        stdx)
endif()

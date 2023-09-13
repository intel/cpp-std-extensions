function(add_tests)
    foreach(name ${ARGN})
        add_unit_test(
            "${name}_test"
            CATCH2
            FILES
            "${name}.cpp"
            LIBRARIES
            warnings
            stdx)
    endforeach()
endfunction()

add_tests(
    always_false
    bit
    bitset
    callable
    concepts
    conditional
    ct_conversions
    cx_map
    cx_multimap
    cx_queue
    cx_set
    cx_vector
    default_panic
    function_traits
    intrusive_list
    overload
    panic
    priority
    remove_cvref
    to_underlying
    with_result_of
    udls)

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_unit_test(
        ct_string_test
        CATCH2
        FILES
        ct_string.cpp
        LIBRARIES
        warnings
        stdx)

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

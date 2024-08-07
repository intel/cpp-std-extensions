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
    algorithm
    always_false
    bind
    bit
    bitset
    byterator
    cached
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
    for_each_n_args
    function_traits
    intrusive_forward_list
    intrusive_list
    intrusive_list_properties
    is_constant_evaluated
    iterator
    memory
    numeric
    optional
    overload
    panic
    priority
    ranges
    remove_cvref
    span
    to_underlying
    type_map
    type_traits
    with_result_of
    utility
    udls)

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_tests(ct_format ct_string indexed_tuple tuple tuple_algorithms)
endif()

add_subdirectory(fail)

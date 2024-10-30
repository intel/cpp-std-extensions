function(add_fail_tests)
    foreach(name ${ARGN})
        add_compile_fail_test("${name}.cpp" LIBRARIES stdx)
    endforeach()
endfunction()

add_fail_tests(
    as_signed_bool
    as_unsigned_bool
    bitset_signed_storage
    bitset_nonintegral_bit_places
    bitset_to_uint64_over_64_bits
    for_each_n_args_bad_size
    optional_without_tombstone
    optional_integral_with_tombstone_traits
    span_insufficient_storage
    span_larger_prefix
    span_larger_suffix
    subspan_late_start
    subspan_late_start_dynamic
    subspan_late_end
    subspan_too_big
    subspan_wrapping
    template_for_each_not_list
    to_address_undefined_on_function)

function(add_formatted_error_tests)
    add_fail_tests(ct_check)
    add_fail_tests(static_assert)
    add_fail_tests(static_assert_format)
endfunction()

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
       AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL 15)
        add_formatted_error_tests()
    endif()
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION}
                                                   VERSION_GREATER_EQUAL 13.2)
        add_formatted_error_tests()
    endif()

    add_fail_tests(
        atomic_bool_dec
        dynamic_span_no_ct_capacity
        dynamic_container_no_ct_capacity
        tuple_index_out_of_bounds
        tuple_equality_mismatch
        tuple_equality_with_element
        tuple_spaceship_mismatch
        tuple_spaceship_with_element
        tuple_type_not_found)
endif()

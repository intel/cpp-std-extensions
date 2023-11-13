function(add_fail_tests)
    foreach(name ${ARGN})
        add_compile_fail_test("${name}.cpp" LIBRARIES stdx)
    endforeach()
endfunction()

add_fail_tests(
    bitset_signed_storage
    bitset_nonintegral_bit_places
    bitset_to_uint64_over_64_bits
    for_each_n_args_bad_size
    optional_without_tombstone
    span_larger_prefix
    span_larger_suffix
    subspan_late_start
    subspan_late_start_dynamic
    subspan_late_end
    subspan_too_big
    subspan_wrapping
    to_address_undefined_on_function)

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_fail_tests(tuple_index_out_of_bounds tuple_type_not_found)
endif()

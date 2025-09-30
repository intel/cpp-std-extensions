function(add_fail_tests)
    foreach(name ${ARGN})
        add_compile_fail_test("${name}.cpp" LIBRARIES stdx)
    endforeach()
endfunction()

add_fail_tests(
    as_signed_bool
    as_unsigned_bool
    bitset_mixed_enumeration
    bitset_nonintegral_bit_places
    bitset_signed_storage
    bitset_to_uint64_over_64_bits
    for_each_n_args_bad_size
    optional_without_tombstone
    optional_integral_with_tombstone_traits
    rollover_less_than
    rollover_signed
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

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_fail_tests(
        atomic_bool_dec
        call_by_need
        ct_format_mismatch
        dynamic_span_no_ct_capacity
        dynamic_container_no_ct_capacity
        tuple_index_out_of_bounds
        tuple_equality_mismatch
        tuple_equality_with_element
        tuple_spaceship_mismatch
        tuple_spaceship_with_element
        tuple_type_not_found)
endif()

function(add_test_by_compiler CPP_NAME CXX_VERSION COMPILER_ID COMPILER_VERSION)
    if("cxx_std_${CXX_VERSION}" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        if(${CMAKE_CXX_COMPILER_ID} STREQUAL "${COMPILER_ID}"
           AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL
               ${COMPILER_VERSION})
            set(TEST_NAME "${CPP_NAME}_${COMPILER_ID}_${COMPILER_VERSION}")
            add_compile_fail_test("${CPP_NAME}.cpp" NAME ${TEST_NAME} LIBRARIES
                                  stdx)
            target_compile_features("EXPECT_FAIL.${TEST_NAME}"
                                    PRIVATE "cxx_std_${CXX_VERSION}")
        endif()
    endif()
endfunction()

add_test_by_compiler(static_assert 20 Clang 15)
add_test_by_compiler(static_assert 20 GNU 13.2)
add_test_by_compiler(static_assert_format 20 Clang 15)
add_test_by_compiler(static_assert_format 20 GNU 13.2)

add_test_by_compiler(static_assert 26 Clang 17)
add_test_by_compiler(static_assert 26 GNU 14)
add_test_by_compiler(static_assert_format 26 Clang 17)
add_test_by_compiler(static_assert_format 26 GNU 14)

add_test_by_compiler(rollover_less_than_26 26 Clang 19)
add_test_by_compiler(rollover_less_than_26 26 GNU 15)

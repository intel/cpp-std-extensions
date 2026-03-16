function(add_tests)
    set(multiValueArgs FILES MULL_EXCLUSIONS)
    cmake_parse_arguments(TEST "" "" "${multiValueArgs}" ${ARGN})

    foreach(name ${TEST_FILES})
        add_unit_test(
            "${name}_test"
            CATCH2
            FILES
            "${name}.cpp"
            LIBRARIES
            warnings
            stdx)
        if(NOT name IN_LIST TEST_MULL_EXCLUSIONS)
            add_mull_test("${name}_test" EXCLUDE_CTEST)
        endif()
    endforeach()
endfunction()

add_tests(
    FILES
    algorithm
    always_false
    array
    atomic
    atomic_override
    atomic_bitset
    atomic_bitset_override
    bind
    bit
    bitset
    byterator
    cached
    call_by_need
    callable
    compiler
    concepts
    conditional
    ct_conversions
    ct_format
    ct_string
    cx_map
    cx_multimap
    cx_queue
    cx_set
    cx_vector
    default_panic
    env
    for_each_n_args
    function_traits
    functional
    indexed_tuple
    intrusive_forward_list
    intrusive_list
    intrusive_list_properties
    is_constant_evaluated
    iterator
    latched
    memory
    numeric
    optional
    overload
    panic
    pp_map
    priority
    ranges
    remove_cvref
    rollover
    span
    to_underlying
    tuple
    tuple_algorithms
    type_bitset
    type_map
    type_traits
    with_result_of
    utility
    udls)

target_compile_definitions(
    atomic_bitset_override_test
    PRIVATE -DATOMIC_CFG="${CMAKE_CURRENT_LIST_DIR}/detail/atomic_cfg.hpp")

target_compile_definitions(
    atomic_override_test
    PRIVATE -DATOMIC_CFG="${CMAKE_CURRENT_LIST_DIR}/detail/atomic_cfg.hpp")

add_unit_test(
    "ct_format_freestanding_test"
    CATCH2
    FILES
    "ct_format.cpp"
    LIBRARIES
    warnings
    stdx)
target_compile_definitions(ct_format_freestanding_test
                           PRIVATE STDX_FREESTANDING)

add_subdirectory(fail)
add_subdirectory(pbt)

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
    atomic
    atomic_override
    atomic_bitset
    atomic_bitset_override
    bind
    bit
    bitset
    byterator
    cached
    callable
    compiler
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
    rollover
    span
    to_underlying
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

if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_tests(FILES ct_format ct_string indexed_tuple tuple tuple_algorithms)
endif()

add_subdirectory(fail)
add_subdirectory(pbt)

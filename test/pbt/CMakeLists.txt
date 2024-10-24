if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_executable(pbt_prototype_driver EXCLUDE_FROM_ALL
                   pbt_prototype_driver.cpp)
    target_link_libraries(pbt_prototype_driver PRIVATE sanitizers warnings stdx)
    target_compile_options(
        pbt_prototype_driver
        PRIVATE -fconstexpr-depth=4096
                $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=4000000000>
                $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=4000000000>
                $<$<CXX_COMPILER_ID:Clang>:-fbracket-depth=1024>)

    add_unit_test(
        tuple
        PYTEST
        FILES
        tuple.py
        EXTRA_DEPS
        ${CMAKE_SOURCE_DIR}/include/stdx/tuple.hpp
        ${CMAKE_SOURCE_DIR}/include/stdx/tuple_algorithms.hpp
        EXTRA_ARGS
        -vv
        -n2
        --compile-commands=${CMAKE_BINARY_DIR}/compile_commands.json
        --prototype-driver=${CMAKE_CURRENT_SOURCE_DIR}/pbt_prototype_driver.cpp)
endif()

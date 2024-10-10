if(${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
    add_executable(pbt_prototype_driver EXCLUDE_FROM_ALL
                   pbt_prototype_driver.cpp)
    target_link_libraries(pbt_prototype_driver PUBLIC sanitizers warnings stdx)

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

rapidcheck_dep = dependency('rapidcheck', required: false, disabler: true)

atomic_override_cdata = configuration_data()
atomic_override_cdata.set_quoted(
    'ATOMIC_CFG',
    meson.current_source_dir() / 'detail' / 'atomic_cfg.hpp',
)

concurrency_dep_or_disabler = concurrency_dep.found() ? concurrency_dep : disabler()

tests = {
    'algorithm': {},
    'always_false': {},
    'array': {},
    'atomic': {
        'dependencies': [concurrency_dep_or_disabler],
    },
    'atomic_override': {
        'dependencies': [concurrency_dep_or_disabler],
        'cpp_args': [
            '-DATOMIC_CFG=' + atomic_override_cdata.get('ATOMIC_CFG'),
        ],
    },
    'atomic_bitset': {
        'dependencies': [concurrency_dep_or_disabler],
    },
    'atomic_bitset_override': {
        'dependencies': [concurrency_dep_or_disabler],
        'cpp_args': [
            '-DATOMIC_CFG=' + atomic_override_cdata.get('ATOMIC_CFG'),
        ],
    },
    'bind': {},
    'bit': {},
    'bitset': {},
    'byterator': {},
    'cached': {},
    'call_by_need': {},
    'callable': {},
    'compiler': {},
    'concepts': {},
    'conditional': {},
    'ct_conversions': {},
    'ct_format': {},
    'ct_format_freestanding': {
        'source': ['ct_format.cpp'],
        'cpp_args': ['-DSTDX_FREESTANDING'],
    },
    'ct_string': {},
    'cx_map': {},
    'cx_multimap': {},
    'cx_queue': {},
    'cx_set': {},
    'cx_vector': {},
    'default_panic': {},
    'env': {},
    'for_each_n_args': {},
    'function_traits': {},
    'functional': {},
    'indexed_tuple': {},
    'intrusive_forward_list': {},
    'intrusive_list': {},
    'intrusive_list_properties': {'dependencies': [rapidcheck_dep]},
    'is_constant_evaluated': {},
    'iterator': {},
    'latched': {},
    'memory': {},
    'numeric': {},
    'optional': {},
    'overload': {},
    'panic': {},
    'pp_map': {},
    'priority': {},
    'ranges': {},
    'remove_cvref': {},
    'rollover': {},
    'span': {},
    'to_underlying': {},
    'tuple': {},
    'tuple_algorithms': {},
    'type_bitset': {},
    'type_map': {},
    'type_traits': {},
    'with_result_of': {},
    'utility': {},
    'udls': {},
}

foreach name, args : tests
    test(
        name,
        executable(
            f'@name@_test',
            args.get('source', [f'@name@.cpp']),
            dependencies: [stdx_dep, catch2_dep] + args.get('dependencies', []),
            cpp_args: args.get('cpp_args', []),
            build_by_default: false,
        ),
        args: ['-r', 'tap'],
        protocol: 'tap',
    )
endforeach

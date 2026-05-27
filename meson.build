project(
    'stdx',
    'cpp',
    version: '0.1.0',
    license: 'BSL-1.0',
    default_options: ['cpp_std=c++23'],
    meson_version: '>=0.63.0',
)

mp11_dep = dependency(
    'boost',
    'boost-mp11',
    modules: ['mp11'],
    version: '>=1.83.0',
)
fmt_dep = dependency('fmt', version: '>=11.0.0', required: get_option('fmt'))
concurrency_dep = dependency('baremetal-concurrency', required: get_option('conc'))

deps = [mp11_dep, fmt_dep, concurrency_dep]

inc = include_directories('include')
src = files(
    'include/stdx/algorithm.hpp',
    'include/stdx/array.hpp',
    'include/stdx/bit.hpp',
    'include/stdx/bitset.hpp',
    'include/stdx/byterator.hpp',
    'include/stdx/cached.hpp',
    'include/stdx/call_by_need.hpp',
    'include/stdx/compiler.hpp',
    'include/stdx/concepts.hpp',
    'include/stdx/ct_conversions.hpp',
    'include/stdx/ct_string.hpp',
    'include/stdx/cx_map.hpp',
    'include/stdx/cx_multimap.hpp',
    'include/stdx/cx_queue.hpp',
    'include/stdx/cx_set.hpp',
    'include/stdx/cx_vector.hpp',
    'include/stdx/env.hpp',
    'include/stdx/for_each_n_args.hpp',
    'include/stdx/function_traits.hpp',
    'include/stdx/functional.hpp',
    'include/stdx/intrusive_forward_list.hpp',
    'include/stdx/intrusive_list.hpp',
    'include/stdx/iterator.hpp',
    'include/stdx/latched.hpp',
    'include/stdx/memory.hpp',
    'include/stdx/numeric.hpp',
    'include/stdx/optional.hpp',
    'include/stdx/panic.hpp',
    'include/stdx/pp_map.hpp',
    'include/stdx/priority.hpp',
    'include/stdx/ranges.hpp',
    'include/stdx/rollover.hpp',
    'include/stdx/span.hpp',
    'include/stdx/tuple.hpp',
    'include/stdx/tuple_algorithms.hpp',
    'include/stdx/tuple_destructure.hpp',
    'include/stdx/type_traits.hpp',
    'include/stdx/udls.hpp',
    'include/stdx/utility.hpp',
)
src_detail = files(
    'include/stdx/detail/bitset_common.hpp',
    'include/stdx/detail/freestanding.hpp',
    'include/stdx/detail/list_common.hpp',
)

if fmt_dep.found()
    src += files(
        'include/stdx/ct_format.hpp',
        'include/stdx/static_assert.hpp',
    )
    src_detail += files(
        'include/stdx/detail/fmt.hpp',
    )
endif

if concurrency_dep.found()
    src += files(
        'include/stdx/atomic.hpp',
        'include/stdx/atomic_bitset.hpp',
    )
endif

install_headers(src, subdir: meson.project_name())
install_headers(src_detail, subdir: meson.project_name() / 'detail')

stdx_dep = declare_dependency(
    dependencies: deps,
    include_directories: inc,
)

meson.override_dependency(meson.project_name(), stdx_dep)

pkgconfig = import('pkgconfig', required: false)
if pkgconfig.found()
    pkgconfig.generate(
        name: meson.project_name(),
        description: 'A header-only C++ library that polyfills the standard library.',
        url: 'https://intel.github.io/cpp-std-extensions/',
        libraries: deps,
        install_dir: get_option('datadir') / 'pkgconfig',
    )
endif

asciidoctor_exe = find_program('asciidoctor', required: get_option('docs'))
subdir('docs', if_found: asciidoctor_exe)

catch2_dep = dependency('catch2-with-main', required: get_option('tests'))
subdir('test', if_found: catch2_dep)

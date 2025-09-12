#include <stdx/algorithm.hpp>
#include <stdx/atomic.hpp>
#include <stdx/atomic_bitset.hpp>
#include <stdx/bit.hpp>
#include <stdx/bitset.hpp>
#include <stdx/byterator.hpp>
#include <stdx/cached.hpp>
#include <stdx/call_by_need.hpp>
#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/ct_conversions.hpp>
#ifndef SIMULATE_FREESTANDING
#include <stdx/ct_format.hpp>
#endif
#include <stdx/ct_string.hpp>
#include <stdx/cx_map.hpp>
#include <stdx/cx_multimap.hpp>
#include <stdx/cx_queue.hpp>
#include <stdx/cx_set.hpp>
#include <stdx/cx_vector.hpp>
#include <stdx/env.hpp>
#ifndef SIMULATE_FREESTANDING
#include <stdx/for_each_n_args.hpp>
#include <stdx/function_traits.hpp>
#endif
#include <stdx/functional.hpp>
#include <stdx/intrusive_forward_list.hpp>
#include <stdx/intrusive_list.hpp>
#include <stdx/iterator.hpp>
#include <stdx/latched.hpp>
#include <stdx/memory.hpp>
#include <stdx/numeric.hpp>
#include <stdx/optional.hpp>
#include <stdx/panic.hpp>
#include <stdx/pp_map.hpp>
#include <stdx/priority.hpp>
#include <stdx/ranges.hpp>
#include <stdx/rollover.hpp>
#include <stdx/span.hpp>
#ifndef SIMULATE_FREESTANDING
#include <stdx/static_assert.hpp>
#endif
#include <stdx/tuple.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/tuple_destructure.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/udls.hpp>
#include <stdx/utility.hpp>

#if __STDC_HOSTED__ == 0
extern "C" auto main() -> int;
#endif

auto main() -> int { return 0; }

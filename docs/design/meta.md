# meta

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/meta.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_meta_hpp

## Filtering out `void`

`filtered_index` solves a subproblem of aggregating function results. You have a
list of functions:

```cpp
// f0 :: () -> int
// f1 :: () -> void
// f2 :: () -> int

using func_list_t = std::tuple<F0, F1, F2>;
```

Some of those functions return `void`. It is easy enough to make a `tuple` of
results with the `void` elements filtered out:

```cpp
template <typename F>
using returns_void = std::is_void<std::invoke_result_t<F>>;

using non_void_func_list_t = boost::mp11::mp_remove_if<func_list_t, returns_void>;
// -> std::tuple<F0, F2>

using results_t = boost::mp11::mp_transform<std::invoke_result_t, non_void_func_list_t>;
// -> std::tuple<int, int>
```

The problem is: after running say `f2` and getting the result, where in the
results tuple should it be placed? This is what `filtered_index` answers: where
does the original index corresponding to `f2` map to in the filtered list?

If this problem seems solvable a simpler way, imagine that the functions are
asynchronous, we need to provision the results tuple space up front, and handle
asynchronous placement of the results as they come in. We need to deal with
indices because any of the function types or result types might coincide, i.e.
we can't find them reliably by type.

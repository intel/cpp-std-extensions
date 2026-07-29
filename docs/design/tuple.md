# tuple

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/tuple.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_tuple_hpp

## A faster `tuple_cat`

One of the original reasons for writing our own `tuple` was compilation speed.
In particular, CIB nexus leans heavily on `tuple_cat`.

The current best known strategy for implementing variadic `tuple_cat` is to
generate the "outer indices" and "inner indices", then zip them together and use
the resulting pairs to pull the elements from each input tuple.

## The workhorse

`stdx::tuple` is a basic dependency for many things; for that reason `tuple.hpp`
doesn't include very much, and in particular must avoid circular dependencies.
For example, it can't use formatted `static_assert`s (because they make use of
`tuple`).

## `tuplelike` vs `has_tuple_protocol`

`tuplelike` means the type exposes `is_tuple`, which in practice means
`tuplelike` is modelled by `stdx::tuple` and `stdx::indexed_tuple` only.

`has_tuple_protocol` means that a type models the `get` protocol. This applies
to much more: `stdx::tuple` but also `std::tuple`, `std::pair`, `std::array`,
etc.

## `one_of`

A common use case is to determine whether or not a value is in a small set.
One way is to just `or` together equality checks:

```cpp
auto b = x == 1
         or x == 2
         or x == 3;
```

But this can be not very easy to read, and begets more complex conditions.
So we used to have code that looked like:

```cpp
auto b = is_one_of(x, 1, 2, 3);
```

This is better, but still not the best. The first function argument is one thing
here and the rest are something else, which doesn't particularly come across in
the formatting or structure.

To improve this, we settled on making `one_of` a type and overloading equality:

```cpp
auto b = x == one_of{1, 2, 3};
```

...and this is quite a natural formulation.

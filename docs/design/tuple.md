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
`tuple`). The diagnostics emitted when `get` fails to compile are helped by
suitable type names in the `stdx::error` namespace:

```cpp
static_assert(always_false_v<looking_for<type>, in_tuple<Ts...>>,
              "Type not found in tuple!");
```

## Constructors

There are a couple of basic ways of implementing tuple. The standard uses
recursion like this (sketch):

```
struct tuple<T, Ts...> : tuple<Ts...> {
  T element;
};
```

This means that the actual layout of the elements is "in reverse", because the
first element is in the `struct` that inherits from all the rest, etc. This also
means that `std::tuple` has many complex constructors.

`stdx::tuple` uses the other way: tag each type with an index (this is necessary
for uniqueness, so that we can have repeated types in the tuple) and inherit
from the expanded pack of such elements.

This means the layout is in order, and the rule of zero can apply. To ease the
construction, we turn off `-Wmissing-braces` so that we can write:

```cpp
auto t = stdx::tuple{1, 2, 3};
```

and the compiler doesn't complain about brace elision (otherwise it would want
us to write e.g. `stdx::tuple{{1}, {2}, {3}}`).

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

# optional

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/optional.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_optional_hpp

## A problem with the size of `std::optional`

Because it is completely generic, `std::optional` basically needs to use a
`bool` on the inside to indicate whether or not it's engaged. This means that
because of alignment and padding, `sizeof(std::optional<std::uint32_t>)` is
twice `sizeof(std::uint32_t)`. And in general, `std::optional` of an integral
type is going to be twice as large as the integral type itself.

In the embedded space, `std::uint32_t` is a _very_ common vocabulary type.
Incurring a 4-byte overhead doesn't sound like much, but it's "distributed fat"
when multiplied across a codebase.

On the other hand, many use cases for `std::optional` don't need a separate
sentinel value; they can use one of the possible values of the type as
"invalid". So it makes sense to have an optional type that identifies
"disengaged" with a sentinel value instead of using an extra flag.

This is what `stdx::optional` does.

### Other ideas

Of note: the use of a sentinel value "within the type" means that
`stdx::optional` can be comparatively simple with respect to special member
functions. This also means that `stdx::optional<T>` _could_ be a structural type
when `T` is a structural type. At the moment it is not, because the contained
`T` is `private`. But with only one member, `stdx::optional` has no class
invariant to uphold; the contained `T` could arguably be `public`.

## Bad accesses

Accessing a disengaged `stdx::optional` simply gives the tombstone value; it
doesn't throw `std::bad_optional_access` or error in some other way. Exceptions
aren't used in general in `stdx` since it is designed for embedded systems.

## How to define the sentinel value

There are two ways to define a sentinel value:

- generally, for a type 
- specifically, for _this_ use of `stdx::optional`
 
The first way is done by specializing `stdx::tombstone_traits`. 

```cpp
template <>
struct stdx::tombstone_traits<my_type> {
  constexpr auto operator()() const {
    return my_type{-1};
  }
};
```

This is typically useful for dealing with enumeration types and other
user-defined types.

The second way is done by providing the sentinel value directly in the type.

```cpp
auto o = stdx::optional<my_type, stdx::tombstone_value<my_type{-1}>>{};
```

This is more useful for integral types. The same integral type might be used to
represent lots of different kinds of runtime values; you don't want to define
just one tombstone value for the type when you have multiple use cases.

In fact, we `static_assert` if you create a `stdx::optional` after specializing
`stdx::tombstone_traits` for an integral type. Because integral types might be
used everywhere, this would be actively dangerous to other parts of the code.

## Why is it called a tombstone value?

This is from hash map implementations: they use the term to denote slots which
used to contain objects but have been marked as "free" again. It's perhaps not a
particularly apt name for use with `optional`.

## What should tombstone values be?

There are a couple of `tombstone_traits` (partial) specializations that are
useful:

- for pointer types, a tombstone value of `nullptr`
- for floating point types, a tombstone value of `infinity`

The obvious first thought about floating point types is that the tombstone value
ought to be `NaN`. But of course it needs to compare equal, and `NaN` never does
that.

### Other ideas

The embedded space raises the point that zero is a completely valid pointer
value. In practice, it is still rare to have to interact with address zero? And
C++ as a whole doesn't really deal with a null pointer being valid.

## Deriving tombstones

If we know a tombstone value for `T`, and a tombstone value for `U`, we ought to
know a tombstone value for `std::pair<T, U>` and `std::tuple<T, U>` without
further specializations.

For product types that are of this "type list" form, we can synthesize such
tombstone values.

### Other ideas

Reflection should allow us to extend this functionality from "type list" types
to aggregates in general.

Note that this is possible for product types where all the contained types have
tombstone values. It is also possible -- but not currently implemented -- for
sum types, where we would need at least one of the types to have a tombstone
value.

## Dealing with awkward types

Some types (e.g. `std::unique_ptr`) are move-only. Some (e.g `std::mutex`) are
not movable. We need to deal with both.

Of course non-movable types must be constructed in place using the
`std::in_place_t` tag constructor. (Or possibly using `stdx::with_result_of`.)

We also need to deal with _non-structural_ types being used as tombstone values.
We can't do this:

```cpp
auto o = stdx::optional<std::string_view, stdx::tombstone_value<std::string_view{}>>{};
```

...because `std::string_view` is not a structural type: a value of it can't be
passed as a template argument. To get around this we allow passing a lambda
expression (whose closure object _is_ structural).

```cpp
auto o = stdx::optional<std::string_view, 
                        stdx::tombstone_value<[] { return std::string_view{}; }>>{};
```

## Functional patterns

The "monadic functions" are `transform`, `and_then`, and `or_else`.

A particular use case that `std::optional` doesn't serve well is combining
multiple optional values. Since `transform` is a member function on
`std::optional` it can only deal with one optional argument rather than
multiple.

To ameliorate this, `transform` is not just a member function on
`stdx::optional`; it's also a free function. As a free function, it can take an
n-ary function and the corresponding number of arguments. In other words, it's
an n-ary version of fmap, which is very useful.

This means `stdx::optional` is not just a functor and a monad; it's also an
[applicative functor](https://www.youtube.com/watch?v=At-b4PHNxMg).

To avoid unnecessary boilerplate, if necessary, (member-function-) `transform`
and `and_then` can unpack types that use the "tuple protocol" like `std::tuple`
and `std::pair`:

```cpp
auto o1 = stdx::optional{std::make_pair(17, 42)};
auto o2 = o1.transform([] (int x, int y) { return x + y; });
```

## Interoperability with `std::optional`

There is no need for `stdx::optional` to provide its own `nullopt_t` or
`in_place_t` type: we can just use the standard types in constructors.

`stdx::transform` defined in `optional.hpp` also works with `std::optional`.

```cpp
auto o = stdx::transform([] (auto x) { return x + 1; },
                         std::optional{42});
// o == std::optional{43}
```

### Other ideas

`stdx::transform` requires that _either_ all its arguments be `stdx::optional`
_or_ all its arguments be `std::optional`. It might be possible to allow
heterogeneous arguments, and make a choice about what to return.

# bitset

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/bitset.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_bitset_hpp

## Controlling the underlying storage

A platform might have different performance characteristics depending on the
type used inside `bitset`. `std::bitset` doesn't allow this to be controlled;
`stdx::bitset` does. For convenience, we don't always want to specify the
underlying type; by default we want to automatically pick the smallest unsigned
integral type that will work.

```cpp
using A = stdx::bitset<8>;
using B = stdx::bitset<8, std::uint32_t>;
```

`A` and `B` hold the same number of bits. `A` has size 1. `B` has size 4. But
manipulating `B`, assuming it fits "natively" into a register, might be cheaper
than manipulating `A`.

### Other ideas

There is a tradeoff of size/performance here that we expose with the underlying
type, but for larger bitsets the size consideration becomes less important; it
becomes a smaller percentage. Should the underlying type for a
`stdx::bitset<97>` be `std::array<std::uint64_t, 13>` or
`std::array<std::uint8_t, 97>`? If we get better performance from
`std::uint64_t` manipulation it probably outweighs the size overhead.

Would it be possible to make this determination automatically?

## Construction

`std::bitset` offers
[construction](https://en.cppreference.com/cpp/utility/bitset/bitset) from
integral types (a "basic" case) and stringish types (not a common case in embedded work).

Two construction use cases that are not provided in `std::bitset`, but are useful
for clear code without magic numbers, are an "in-place"-style constructor and a
constructor that sets all the bits:

```cpp
auto bs1 = stdx::bitset<8>{stdx::place_bits, 1, 2, 3}; // 0b0000'1110
auto bs2 = stdx::bitset<8>{stdx::all_bits};            // 0b1111'1111
```

It is surprising that `std::bitset` doesn't have an easy way, in the general
case, to do the second option here!

Constructing with all bits set is _sometimes_ possible with `std::bitset`:
- construction from an `unsigned long long int` (if the bitset size is not greater)
- construction from a stringish type (which is potentially expensive and/or error-prone with respect to length)

### Other ideas

Is it worth preserving the stringish constructors in `stdx`?

## Better conversions

`std::bitset` is equipped only to handle conversions to `unsigned long [long]
int`. We want to take a hint from ranges and offer a `to` function to
convert to whichever integral type is desired.

```cpp
auto bs = stdx::bitset<8>{stdx::place_bits, 1, 2, 3};
auto val = bs.to<std::uint32_t>();
```

This is much more convenient, and safer: we express the type directly where we
need it. The alternative would be `static_cast`.

Given `to`, it is also natural to provide `to_natural`.

```cpp
auto bs = stdx::bitset<8>{stdx::place_bits, 1, 2, 3};
auto val = bs.to_natural();
```

This exposes the actual "natural" underlying type according to how many bits are
in the bitset.

### Other ideas 

At the moment `to` accepts only an unsigned integral type -- meaning it can't
really be used for bitsets over 64 bits in size. It might be useful to allow
conversion to a `std::array` of unsigned integral type.

We might also spell `to_natural` as `to<>`, but this is probably harder to read.

## `bitset` as a container

`std::bitset` is confused: are we supposed to think of it as its underlying integral type, with supporting functions?

- construction from `unsigned long int`
- `to_ulong`, `to_ullong`
- bitwise operations

Or, are we supposed to think of it as a container, with supporting functions?

- `operator[]`
- `all`, `any`, `none`
- `count`

The balance is probably on the container interpretation, but `std::bitset` confuses this again:

- `size` returns what a normal container would call `capacity`

`stdx::bitset` chooses the container interpretation and makes it more consistent with other containers.

- `size` returns the same as `count`
- `capacity` returns the number of bits that can be held
- `empty` is equivalent to `none` (i.e. `size() == 0`)

Note: `capacity` is implemented as a `std::integral_constant<std::size_t, N>`
following the design
[elucidated by Jonathan Müller](https://www.think-cell.com/en/career/devblog/the-new-static-constexpr-std-integral_constant-idiom).

### Other ideas

There may be other possible "container" operations. e.g. is it useful to expose `data` in
some way (presumably as a span over the underlying storage)?

## `bitset` as a set

If we really want a _set_ of bits, then it makes sense to provide set operations.
Of course we already have the AND and OR bitwise operations which correspond to
intersection and union respectively. But a set difference operation is also
useful.

```cpp
// intersection and union are familiar
auto i = a & b;
auto u = a | b;

// difference is equivalent to a & ~b
auto d = a - b;
```

### Other ideas

Perhaps we should expose named functions for `set_intersection`, `set_union` and
`set_difference`.

Is it surprising that `operator-` doesn't do a regular integral subtraction? (I
hope not.)

## Use case: `enum` flags

Many electrons have been spent over the years trying make C++ enumerations
behave like bit flags. [P4313](https://wg21.link/p4313) is an attempt to put
this into the language. The basic approach has been to define bitwise operations
on `enum` types, and variously to fight with language mechanisms to achieve this
in more-or-less "nicer" ways.

This is likely to continue to be a popular direction, especially with new
reflection tools, but it seems fundamentally muddled (notwithstanding the fact
that it is the direction enshrined in several other ALGOL-family languages). It
is _easy_ but not _simple_. It confuses _naming_ the bits (what we really want)
with _representing_ the bits.

It generally requires doing something like:

```cpp
enum struct permissions {
  READ  = 0b001,
  WRITE = 0b010,
  EXEC  = 0b100
};

permissions p = get_permissions();
if ((p & permissions::READ) != 0) {
  // ...
}
```

This has a couple of desirable features over `std::bitset`:

- type safety: `std::bitset<N>` cannot distinguish between a bitset-over-enum-A and a bitset-over-enum-B; this approach disallows implicit conversions
- ergonomics: `std::bitset` cannot deal with enum values, so may require wrapping functions to provide meaningful names

But it also has drawbacks:

- providing bitwise operations with the correct name lookup is _difficult_ to get right, especially when applying it to 3rd party libraries
- writing pre-shifted values as the enumerations is verbose and more accident-prone than letting the compiler provide the integral bit-number values
- bitwise operations in C and C++ are famously subject to "wrong" precedence rules

And it also subverts/conflates the ideas of _type_ and _value_. `permissions{7}`
is not a member of the enumeration type `permissions`, but we are asked to
accept that it is based on interpreting its value. We wanted to _name_ the bits
but we are complecting that with _representing_ them.

`std::bitset` cannot change to fix these problems, but `stdx::bitset` can.

We would prefer for example:

```cpp
enum struct permissions {
  READ, WRITE, EXEC,
  MAX
}

std::bitset<permissions::MAX> p = get_permissions();
if (p[permissions::READ]) {
}
```

This has the features we desire (type safety, ergonomics) and does not have the
undesirable features (name lookup complexity, verbosity, precedence sensitivity).
This separates names from representation. And when combined with the [easier
conversion functions](#better-conversions) and the [in-place style
constructor](#construction) on `stdx::bitset`, this is a better way to deal with
such use cases.

## Use case: iteration

**Q.** What do you do if you want to iterate over the set bits?  
**A.** (for `std::bitset`) You extract the underlying data, write a bit-twiddling loop, and hope that this is clear to other programmers and the optimizer.

`stdx::bitset` takes a more declarative approach with `for_each`.

```cpp
auto bs = stdx::bitset<8>{0b1010'1010ul};
for_each([](auto i) { /* i == 1, 3, 5, 7 */ }, bs);
```

### Other ideas

Should more algorithms be defined? In particular should `all_of`, `any_of`,
`none_of` be equivalent to the member functions?

## Use case: tracking items

A semi-common use case for a bitset is to track "used" items in a second data
structure. In this case you typically want to know which is the lowest currently
unset bit, representing the next unused item in the other data structure.

So let's provide that.

```cpp
auto bs = stdx::bitset<8>{0b11'0111ul};
auto i = bs.lowest_unset(); // i == 3u
```

If there are _no_ unset bits, `lowest_unset` should return the `capacity` of the
`stdx::bitset`.

### Other ideas

Would other bit operations be useful, e.g. `lowest_set`?

## Use case: a type list `bitset`

Sometimes it is useful to have a bitset representing types. In familiar
metaprogramming, a type list can easily be converted to a `std::tuple<bool...>`
but it could be desirable to use a bitset instead.

We would like a `type_bitset` to be a "normal" type list for ease of use with metaprogramming.
```cpp
using T = stdx::type_bitset<int, float bool>;
```

We would also like easy and safe construction.

```cpp
// construction from a type list
auto bs = stdx::type_bitset<int, float, bool>{type_list<int, bool>{}}; // 0b101
```

Construction with a deduction guide would be possible but might be dangerous,
because it would be easily confused with explicit-type construction but would
always lead to a bitset with all the bits set, so we don't do this.

```cpp
// construction using a deduction guide: possible but risky
auto bs = stdx::type_bitset{type_list<int, bool>{}}; // 0b11
```

The usual bitwise operations (`set`, `reset`, `flip`) all become function
templates; this is fine.

```cpp
bs.set<float>();
bs.reset<int>();
bs.flip<bool>();
```

Indexing takes any type that exposes a `type` member alias. Canonically this is
`type_identity` but any such (defined) type works.

```cpp
auto has_float = bs[stdx::type_identity<float>{}];
```

Unfortunately, there is no `std::type_identity_v`, but we do have `stdx::type_identity_v`.

Iteration over a `type_bitset` changes `for_each` a little. The callable is a
function template that receives (for each set bit) both the corresponding type
and its index.

```cpp
auto bs = stdx::type_bitset<int, float, bool>{type_list<int, bool>{}}; // 0b101
bs.for_each([&]<typename T, std::size_t I>() -> void {
  // <T,I> <- [<int, 0>, <bool, 2>]
});
```

With any of the functions on `type_bitset`, calling with a type that doesn't
exist in the bitset should produce a useful `static_assert` help message.

### Other ideas

Is it worth revisiting CTAD and making it safe some other way (e.g. with a tag constructor)?

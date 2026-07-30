# type_traits

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/type_traits.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_type_traits_hpp

## `conditional_t`

The standard defines `conditional<bool B, typename T, typename U>` as a
`struct`. This means that for every combination of `<B, T, U>` the compiler
instantiates a template and creates a type: a relatively expensive compile-time
operation.

We define `conditional_t<bool B, typename T, typename U>` as an alias template.
And we define `conditional` parameterized on the `bool` only, with an internal
alias template used to select either `T` or `U`. This means that the actual
template is only instantiated twice (for `true` and `false`) and otherwise the
operations are done through alias templates, which are much cheaper.

STL implementations typically do the same thing for internal use, but the
standard constrains them to provide the more expensive interface.

## `to_underlying`

In the standard, `underlying_type_t` is in `<type_traits>` while `to_underlying`
is in `<utility>`. `stdx` puts both in `<type_traits>`.

In the standard, this fails to compile:

```cpp
auto x = to_underlying(42);
```

because `to_underlying` is defined only on enumeration types. But in practice,
it is very useful in generic code to have an "idempotent" form of
`to_underlying` that reduces an enumeration to the underlying integral type and
is a no-op otherwise. That's what `stdx::to_underlying` does.

### Other ideas

`stdx::to_underlying` could be constrained to work on either integral or
enumeration types, but it is currently unconstrained.

## Tuple helpers

The standard seems ambiguous on where the primary template declarations for
`tuple_element` and `tuple_size` are to be kept, or if it matters. We choose to
put them in `<type_traits.hpp>`. Each class that specializes them then includes
`<type_traits.hpp>` rather than `<tuple.hpp>`.

## `type_identity`

`std::type_identity` and `std::type_identity_t` exist from C++20, so in theory
we could get rid of their counterparts in `stdx`. However, the standard does not
define `type_identity_v` -- which is actually very useful. Given that utility,
it is consistent to provide all three in `stdx`; especially given that they are
trivial.

## Miscellaneous

We expect to remove several type traits from `stdx` as standard adoption allows.

`is_specialization_of` and `is_same_template_v` get a lot easier particularly
with reflection.

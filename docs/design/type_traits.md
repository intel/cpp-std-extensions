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


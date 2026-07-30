# utility

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/utility.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_utility_hpp

## sized

This exists so that we don't have to reason about "raw" size calculations
everywhere:

```cpp
// not this
auto size_in_dwords = (size_in_bytes + 3) / 4;
// but this
auto size_in_dwords = stdx::sized8{size_in_bytes}.in<std::uint32_t>();
```

More verbose, but almost certainly less error-prone and easier to read. These
are simple operations, and sometimes they should be terse. If the values are
known at compile time:

```cpp
constexpr auto size_in_dwords = 42_z8->z32;
```

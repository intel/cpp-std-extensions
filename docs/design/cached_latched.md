# cached and latched

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/latched.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_latched_hpp

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/cached.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_latched_hpp

## Lazy values

Both `cached` and `latched` model lazy computation of values. Under the hood
they are just `std::optional`. 

Note that use of `cached` and `latched` generally incurs a branch check for each
use.

`latched` does not provide for `reset`/`refresh`: it's intended for values that
are read once at startup time.

`cached` values can be `reset` or `refresh`ed. The default should probably be to
`reset` and lazily recompute but `refresh` is for the use case of "recompute
now".

### Other ideas

We don't use `stdx::optional` here; that requires tombstone value(s). Maybe
there is a way to select the optional implementation based on detecting a
tombstone? in practice many use cases for `cached`/`latched` are register
(`std::uint32_t`) reads rather than strong types that could use tombstones.

# *stdx* - C++ Standard Extensions and Polyfill

[![Unit Tests](https://github.com/intel/cpp-std-extensions/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/intel/cpp-std-extensions/actions/workflows/unit_tests.yml)

 *stdx* is a C++ header-only library that:
 
- backports to C++N some standard library features from C++(>N) and beyond
- adds some useful extra utilities that are missing from the standard

See the [full documentation](https://intel.github.io/cpp-std-extensions/).

C++ standard support is as follows:

- C++23: [main branch](https://github.com/intel/cpp-std-extensions/tree/main) (active development)
- C++20: [cpp20 branch](https://github.com/intel/cpp-std-extensions/tree/main) (supported)
- C++17: unsupported, but many features in the [cpp20 branch](https://github.com/intel/cpp-std-extensions/tree/cpp20) will work with C++17 also.

Compiler support:

| Branch | GCC versions | Clang versions |
| --- | --- | --- |
| [main](https://github.com/intel/cpp-baremetal-concurrency/tree/main) | 12 thru 14 | 18 thru 21 |
| [cpp20](https://github.com/intel/cpp-baremetal-concurrency/tree/cpp20) | 12 thru 14 | 14 thru 21 |


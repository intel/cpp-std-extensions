# rollover_t

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/rollover.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_rollover_hpp

## Hardware counters wrap around

To measure time, microcontrollers often have continually-incrementing hardware
counters. A common version might be a 32-bit value that increments every
microsecond. The problem arises: how to deal with when this wraps around?

Because we need to measure time in the past as well as in the future (because we
need timers to actually expire!), we need to treat time values as signed.
`rollover_t` defines a type that models "now" as always being the central value
in a window that is rolling. Half the bit space is always in the future, and
half is in the past.

## An arithmetic type

`rollover_t` is supposed to behave like an integral type, with all the usual
arithmetic operations defined, mod 2ⁿ.

## Comparisons

`rollover_t` is equality comparable. This is fine. However, other comparison
operators are deleted.

The reason is that comparisons on `rollover_t` don't define a strict weak order,
which is required by STL algorithms like `std::sort`. And trying to sort things
with a misbehaving comparison operator can lead to undefined behaviour.

Concretely, comparisons on `rollover_t` are neither antisymmetric nor
transitive. To see why, consider a `rollover_t` limited to 3 bits.

With such a `rollover_t`, all of the following are true:

`0 < 1`  
`1 < 2`  
`2 < 3`  
`3 < 4`  
`4 < 5`  
`5 < 6`  
`6 < 7`  
`7 < 0`  

Note also that `5` is less than each of `6`,`7`,`0`,`1`,  
_and_ `5` is greater than each of `1`,`2`,`3`,`4`.

So comparison is not antisymmetric (`5 < 1` and `5 > 1`).  
And comparison is non-transitive (like rock-paper-scissors).

Naively using `std::sort` is not something that is safe, so we delete
`operator<` and friends. However, sometimes it is necessary to sort
`rollover_t`s and in that case we use `cmp_less` as the comparator.

`cmp_less` doesn't magically fix the properties so that `std::sort` works; it's
just spelt differently so that it stands out on code review.

Undefined behaviour is a _runtime_ property of a program: with a given data set,
it may not occur. So when we need to sort `rollover_t`s we are warranting that
the data is such that UB will not occur, even though the comparison operation
doesn't have the right properties. It is still UB-safe to sort `rollover_t`s for
example when they all lie within one half of the bit space. In that case they
can be correctly ordered.

### Other ideas

Is it perhaps sensible to restore antisymmetry? Can this be done in the same way
as `int`, i.e. by having "now" as a notional zero, with n values less but only
n-1 values greater?

e.g. for a (signed) char, 128 values are less than zero but only 127 are greater?

Note that this would not restore transitivity.

## Use with `std::chrono`

Since we're measuring time, we want to use `rollover_t` as the `rep` type inside
a `std::chrono::duration` and its corresponding `time_point_t`.

To do this requires a specialization of `std::common_type` for `rollover_t`.



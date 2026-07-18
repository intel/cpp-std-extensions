# algorithms

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/algorithm.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_algorithm_hpp

## Variadic algorithms

When ranges were introduced, standard (non-range) algorithms stopped getting
many updates. They are spotty in their overloads (iterator pair vs iterator &
count) and they don't tend to be variadic (although some are binary, like
`std::transform`).

All the algorithms here are variadic, and adhere to the law of useful return:
they return all the iterators that they advanced.

## `for_each`

Like `std::for_each`, `stdx::for_each` returns the operation it's given. This is
for the use case of passing a mutable lambda or other function object that
updates its internal state as it works.

## `for_each_butlast`

A (surprising?) omission from the standard. Sometimes we want to do something
for each element of a sequence except the last. It turns out that the more
general case, `for_each_butlastn` is just as easy to write.

This doesn't exist in ranges either (it can be achieved of course for sized
ranges, but not generally). But the same idea exists in other languages
(Haskell's `init` or Common Lisp's `butlast`/`butlastn`).

## `initial_medial_final`

Sometimes in embedded systems/hardware interaction we need to do one thing at
first, then do another thing for the bulk elements (but the same for all),
then do something else at the end. Typically interacting with different
start/end registers, or something like that. This is the envisioned use case for
`initial_medial_final`.

It is clear what to do with a range of 3 or more elements. When we have a
smaller range, we have to make (somewhat arbitrary) decisions. The decisions at
the moment are:

- For a range of 0 elements, do nothing. Obviously.
- For a range of 2 elements, the medials are omitted. This also seems sensible.
- The real question is what to do for a range of 1 element. At the moment we treat it like an
  initial element. Should it be final? Or something else?

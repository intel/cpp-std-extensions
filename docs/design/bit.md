# bit

Source code: https://github.com/intel/cpp-std-extensions/blob/main/include/stdx/bit.hpp  
Documentation: https://intel.github.io/cpp-std-extensions/#_bit_hpp

## No more manual bit twiddling!

A lot of the functionality in this header exists to put an end to manual bit
shifting, masking, etc. Yes it's easy, but it's not simple. Manual bit
operations are prone to sign and integer promotion errors, and sprinkling
`static_cast` everywhere to guard against this makes things ugly and less
understandable.

## `to_be`, `from_be`, `to_le`, `from_le`

`to_be` and `from_be` have identical implementations. As do `to_le` and
`from_le`. But they are different in showing the intent of user code. And also
different in pre and post conditions:

```cpp
// pre: x is big-endian
// post: x is platform-endian
auto x = stdx::from_be(y);

// pre: x is platform-endian
// post: x is big-endian
auto x = stdx::to_be(y);
```

In other words, the use of `from_be`/`to_be` is very different from the use of
`std::byteswap` in cross-platform friendly code.

## `bit_pack` and `bit_unpack`

These functions have some interesting endianness concerns. It is important that
the functions round-trip properly.

```cpp
std::uint8_t a{0x12u}, b{0x34u}, c{0x56u}, d{0x78u};
auto x = stdx::bit_pack<std::uint32_t>(a, b, c, d);
auto [a_, b_, c_, d_] = stdx::bit_unpack<std::uint8_t>(x);
// a == a_, b == b_; c == c_, d == d_
```

This also means that that "written order" (big-endian) is the expected ordering.

```cpp
auto [a, b, c, d] = stdx::bit_unpack<std::uint8_t>(0x12345678u);
// a == 0x12, b == 0x34; c == 0x56, d == 0x78
```

## `bit_destructure`

`bit_destructure` and `bit_unpack` are similar, but where `bit_unpack` is
"big-endian" (for the above reasons), `bit_destructure` is "little-endian".
Because it makes sense to count **up** in bits.

Also, N split points mean N+1 values.

```cpp
auto [a, b, c, d] = stdx::bit_destructure<8, 16, 24>(0x12345678u);
// a == 0x78, b == 0x56; c == 0x34, d == 0x12
```

#include <stdx/bitset.hpp>

// EXPECT: smallest_uint override must be an unsigned type

auto main() -> int { auto b = stdx::bitset<32, int>{}; }

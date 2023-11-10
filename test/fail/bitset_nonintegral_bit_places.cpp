#include <stdx/bitset.hpp>

// EXPECT: Bit places must be integral

auto main() -> int { auto b = stdx::bitset<32>{stdx::place_bits, 1.0f}; }

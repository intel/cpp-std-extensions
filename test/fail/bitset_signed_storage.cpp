#include <stdx/bitset.hpp>

// EXPECT: Underlying storage of bitset must be an unsigned type

auto main() -> int { auto b = stdx::bitset<32, int>{}; }

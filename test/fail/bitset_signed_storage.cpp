#include <stdx/bitset.hpp>

// EXPECT: Storage element for bitset must be an unsigned type

auto main() -> int { auto b = stdx::bitset<32, int>{}; }

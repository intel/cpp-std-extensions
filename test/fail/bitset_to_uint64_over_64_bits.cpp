#include <stdx/bitset.hpp>

// EXPECT: Bitset too big for conversion to T

auto main() -> int {
    auto b = stdx::bitset<65, unsigned char>{};
    auto i = b.to<std::uint64_t>();
}

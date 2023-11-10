#include <stdx/bitset.hpp>

// EXPECT: Bitset too big for conversion to std::uint64_t

auto main() -> int {
    auto b = stdx::bitset<65, unsigned char>{};
    auto i = b.to_uint64_t();
}

#include <stdx/span.hpp>

#include <array>

// EXPECT: subspan cannot be longer than span

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = stdx::span{a};
    auto s2 = s.subspan<0, 5>();
}

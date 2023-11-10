#include <stdx/span.hpp>

#include <array>

// EXPECT: subspan cannot end beyond span

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = stdx::span{a};
    auto s2 = s.subspan<1, 4>();
}

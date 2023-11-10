#include <stdx/span.hpp>

#include <array>

// EXPECT: cannot form a larger span

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = stdx::span{a};
    auto s2 = s.last<5>();
}

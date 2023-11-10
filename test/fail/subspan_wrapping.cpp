#include <stdx/span.hpp>

#include <array>
#include <cstddef>
#include <limits>

// EXPECT: subspan cannot end beyond span

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = stdx::span<int, stdx::dynamic_extent>{a};
    auto s2 = s.subspan<3, std::numeric_limits<std::size_t>::max() - 2>();
}

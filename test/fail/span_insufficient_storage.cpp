#include <stdx/span.hpp>

#include <array>
#include <cstdint>

// EXPECT: Span extends beyond available storage
auto main() -> int {
    std::array<std::uint32_t, 1> storage{};
    stdx::span<std::uint32_t, 2> s{storage};
}

#include <stdx/atomic.hpp>

// EXPECT: T must support operator--

auto main() -> int {
    auto x = stdx::atomic<bool>{true};
    --x;
}

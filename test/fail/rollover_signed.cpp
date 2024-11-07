#include <stdx/rollover.hpp>

// EXPECT: Argument to rollover_t must be an unsigned integral type

auto main() -> int {
    using X = stdx::rollover_t<int>;
    [[maybe_unused]] X x{};
}

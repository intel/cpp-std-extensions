#include <stdx/rollover.hpp>

// EXPECT: deleted (operator|function)

auto main() -> int {
    using X = stdx::rollover_t<unsigned int>;
    [[maybe_unused]] auto cmp = X{} < X{1u};
}

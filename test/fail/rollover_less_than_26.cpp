#include <stdx/rollover.hpp>

// EXPECT: deleted because they are non-transitive

auto main() -> int {
    using X = stdx::rollover_t<unsigned int>;
    [[maybe_unused]] auto cmp = X{} < X{1u};
}

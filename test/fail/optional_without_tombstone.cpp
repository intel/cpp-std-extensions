#include <stdx/optional.hpp>

// EXPECT: To use stdx::optional you must specialize stdx::tombstone_traits

auto main() -> int { [[maybe_unused]] auto o = stdx::optional{42}; }

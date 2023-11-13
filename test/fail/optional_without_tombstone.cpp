#include <stdx/optional.hpp>

// EXPECT: To use stdx::optional you must specialize stdx::tombstone_traits

enum struct E {};

auto main() -> int { [[maybe_unused]] auto o = stdx::optional{E{}}; }

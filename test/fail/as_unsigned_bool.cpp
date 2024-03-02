#include <stdx/utility.hpp>

// EXPECT: as_unsigned is not applicable to bool
auto main() -> int { auto b = stdx::as_unsigned(true); }

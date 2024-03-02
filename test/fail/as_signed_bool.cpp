#include <stdx/utility.hpp>

// EXPECT: as_signed is not applicable to bool
auto main() -> int { auto b = stdx::as_signed(true); }

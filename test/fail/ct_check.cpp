#include <stdx/ct_string.hpp>

// EXPECT: 01234567890123456789012345678901234567890123456789

constexpr auto msg =
    stdx::ct_string{"01234567890123456789012345678901234567890123456789"};

auto main() -> int {
    [[maybe_unused]] auto x = stdx::ct_check<true>.emit<"not emitted">();
    stdx::ct_check<false>.emit<msg>();
}

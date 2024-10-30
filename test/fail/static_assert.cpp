#include <stdx/static_assert.hpp>

// EXPECT: 01234567890123456789012345678901234567890123456789

constexpr auto msg =
    stdx::ct_string{"01234567890123456789012345678901234567890123456789"};

auto main() -> int {
    static_assert(STATIC_ASSERT(true, "not emitted"));
    STATIC_ASSERT(false, msg);
}

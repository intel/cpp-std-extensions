#include <stdx/static_assert.hpp>
#include <stdx/utility.hpp>

// EXPECT: hello world int 123

static_assert(STATIC_ASSERT(true, "hello {} {} {}", "world", int, 42));

template <typename T> constexpr auto f() {
    STATIC_ASSERT(false, "hello {} {} {}", "world", T, 123);
}

auto main() -> int { f<int>(); }

#include <stdx/static_assert.hpp>
#include <stdx/utility.hpp>

// EXPECT: hello world int 123

template <typename T> constexpr auto f() {
    STATIC_ASSERT(false, "hello {} {} {}", CX_VALUE("world"), CX_VALUE(T), 123);
}

auto main() -> int { f<int>(); }

#include <stdx/tuple.hpp>

// EXPECT: ambiguous

auto main() -> int {
    auto t = stdx::tuple{1, 2.0, 3};
    auto u = stdx::tuple{1, 2.0};
    auto b = t == u;
}

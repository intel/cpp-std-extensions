#include <stdx/tuple.hpp>

// EXPECT: deleted

auto main() -> int {
    auto t = stdx::tuple{1};
    auto b = t <=> 1;
}

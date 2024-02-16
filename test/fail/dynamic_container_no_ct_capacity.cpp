#include <stdx/iterator.hpp>

#include <vector>

// EXPECT: Type does not support compile-time capacity

auto main() -> int {
    auto v = std::vector{1, 2, 3, 4};
    constexpr auto c = stdx::ct_capacity(v);
}

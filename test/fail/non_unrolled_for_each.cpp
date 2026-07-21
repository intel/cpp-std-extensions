#include <stdx/tuple_algorithms.hpp>

#include <array>

// EXPECT: Calling for_each on array-like type

auto main() -> int {
    auto a = std::array{1, 2, 3};
    stdx::for_each([](auto) {}, a);
}

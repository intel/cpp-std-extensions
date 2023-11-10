#include <stdx/tuple.hpp>

// EXPECT: Type not found in tuple

auto main() -> int {
    auto t = stdx::tuple<int>{};
    auto elem = stdx::get<float>(t);
}

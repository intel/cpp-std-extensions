#include <stdx/tuple.hpp>

// EXPECT: Tuple index out of bounds

auto main() -> int {
    auto t = stdx::tuple<int>{};
    auto elem = stdx::get<1>(t);
}

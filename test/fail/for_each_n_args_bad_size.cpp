#include <stdx/for_each_n_args.hpp>

// EXPECT: number of args must be a multiple of

auto main() -> int {
    auto f = [](int, int) {};
    stdx::for_each_n_args<2>(f, 2, 3, 3);
}

#include <stdx/call_by_need.hpp>

// EXPECT: call_by_need could not find calls for all the given functions

auto main() -> int {
    constexpr auto r =
        stdx::call_by_need(stdx::tuple{[](int) {}}, stdx::tuple{});
}

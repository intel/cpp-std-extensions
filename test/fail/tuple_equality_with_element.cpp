#include <stdx/tuple.hpp>

// EXPECT: deleted

struct S {
    constexpr friend auto operator==(S, S) -> bool = default;
};

auto main() -> int {
    auto t = stdx::tuple{S{}};
    auto b = t == S{};
}

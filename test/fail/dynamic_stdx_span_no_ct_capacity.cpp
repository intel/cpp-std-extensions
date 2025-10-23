#include <stdx/iterator.hpp>
#include <stdx/span.hpp>

#include <array>
#include <iterator>

// EXPECT: Type does not support compile-time capacity

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = stdx::span{std::begin(a), std::end(a)};
    constexpr auto c = stdx::ct_capacity(s);
}

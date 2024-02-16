#include <stdx/iterator.hpp>

#include <array>
#include <iterator>
#include <span>

// EXPECT: Type does not support compile-time capacity

auto main() -> int {
    auto a = std::array<int, 4>{};
    auto s = std::span{std::begin(a), std::end(a)};
    constexpr auto c = stdx::ct_capacity(s);
}

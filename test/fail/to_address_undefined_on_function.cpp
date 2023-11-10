#include <stdx/memory.hpp>

// EXPECT: to_address on a function pointer is ill-formed

namespace {
auto f() -> void {}
} // namespace

auto main() -> int { [[maybe_unused]] auto p = stdx::to_address(f); }

#include <stdx/ct_format.hpp>

// EXPECT: mismatch between the number of format specifiers and arguments

auto main() -> int { [[maybe_unused]] auto x = stdx::ct_format<"Hello">(42); }

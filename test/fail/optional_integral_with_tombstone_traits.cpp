#include <stdx/optional.hpp>

// EXPECT: Don't define tombstone traits for plain integral types

template <> struct stdx::tombstone_traits<int> {
    constexpr auto operator()() const { return -1; }
};

auto main() -> int { [[maybe_unused]] auto o = stdx::optional{42}; }

#include <stdx/ranges.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>

TEST_CASE("range concept", "[ranges]") {
    static_assert(not stdx::range<int>);
    static_assert(stdx::range<std::array<int, 4>>);
}

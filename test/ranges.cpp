#include <stdx/ranges.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>

TEST_CASE("range concept", "[ranges]") {
    STATIC_REQUIRE(not stdx::range<int>);
    STATIC_REQUIRE(stdx::range<std::array<int, 4>>);
}

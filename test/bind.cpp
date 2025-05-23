#include "detail/tuple_types.hpp"

#include <stdx/functional.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("bind_front basic operation", "[bind]") {
    constexpr auto f = stdx::bind_front([](int x, int y) { return x - y; }, 42);
    STATIC_REQUIRE(f(17) == 25);
}

TEST_CASE("bind_front with rvalue", "[bind]") {
    auto f = stdx::bind_front([](move_only x, int y) { return x.value - y; },
                              move_only{42});
    CHECK(std::move(f)(17) == 25);
}

namespace {
auto g(int x, int y) -> int { return x - y; }
} // namespace

TEST_CASE("bind_front with compile-time function pointer", "[bind]") {
    auto const f = stdx::bind_front<g>(42);
    CHECK(f(17) == 25);
}

#if __cpp_nontype_template_args >= 201911L
TEST_CASE("bind_front with lambda template argument", "[bind]") {
    auto const f = stdx::bind_front<[](int x, int y) { return x - y; }>(42);
    CHECK(f(17) == 25);
}
#endif

TEST_CASE("bind_back basic operation", "[bind]") {
    constexpr auto f = stdx::bind_back([](int x, int y) { return x - y; }, 17);
    STATIC_REQUIRE(f(42) == 25);
}

TEST_CASE("bind_back with rvalue", "[bind]") {
    auto f = stdx::bind_back([](int x, move_only y) { return x - y.value; },
                             move_only{17});
    CHECK(std::move(f)(42) == 25);
}

TEST_CASE("bind_back with compile-time function pointer", "[bind]") {
    auto const f = stdx::bind_back<g>(17);
    CHECK(f(42) == 25);
}

#if __cpp_nontype_template_args >= 201911L
TEST_CASE("bind_back with lambda template argument", "[bind]") {
    auto const f = stdx::bind_back<[](int x, int y) { return x - y; }>(17);
    CHECK(f(42) == 25);
}
#endif

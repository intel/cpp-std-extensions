#include <stdx/functional.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace {
template <typename T, typename = void>
constexpr auto detect_is_transparent = false;
template <typename T>
constexpr auto
    detect_is_transparent<T, std::void_t<typename T::is_transparent>> = true;

struct S {};
constexpr auto operator+(S) { return 17; }
} // namespace

TEST_CASE("unary_plus", "[functional]") {
    STATIC_REQUIRE(stdx::unary_plus<int>{}(17) == 17);
    STATIC_REQUIRE(stdx::unary_plus<>{}(17) == 17);
}

TEST_CASE("unary_plus transparency", "[functional]") {
    STATIC_REQUIRE(not detect_is_transparent<stdx::unary_plus<int>>);
    STATIC_REQUIRE(detect_is_transparent<stdx::unary_plus<>>);
}

TEST_CASE("unary_plus calls operator+", "[functional]") {
    STATIC_REQUIRE(stdx::unary_plus<>{}(S{}) == 17);
}

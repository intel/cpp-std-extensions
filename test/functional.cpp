#include "detail/tuple_types.hpp"

#include <stdx/functional.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <utility>

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

TEST_CASE("safe_identity returns unchanged argument", "[functional]") {
    static_assert(stdx::safe_identity(17) == 17);
    static_assert(stdx::safe_identity(move_only{17}).value == 17);
}

TEST_CASE("safe_identity (copy)", "[functional]") {
    counter::reset();
    counter c0{};
    [[maybe_unused]] auto c1 = stdx::safe_identity(c0);
    CHECK(counter::copies == 1);
}

TEST_CASE("safe_identity (move)", "[functional]") {
    counter::reset();
    counter c0{};
    [[maybe_unused]] auto c1 = stdx::safe_identity(std::move(c0));
    CHECK(counter::copies == 0);
    CHECK(counter::moves == 1);
}

TEST_CASE("safe_identity transparency", "[functional]") {
    STATIC_REQUIRE(detect_is_transparent<stdx::safe_identity_t>);
}

namespace {
template <typename T> auto declval() -> T;
}

TEST_CASE("safe_identity value categories", "[functional]") {
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int>())), int>);
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int &>())), int &>);
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int &&>())), int>);
}

TEST_CASE("safe_identity cvref categories", "[functional]") {
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int const &>())),
                       int const &>);
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int volatile &>())),
                       int volatile &>);
    static_assert(std::is_same_v<decltype(stdx::safe_identity(
                                     declval<int const volatile &>())),
                                 int const volatile &>);
    static_assert(
        std::is_same_v<decltype(stdx::safe_identity(declval<int const &&>())),
                       int>);
    static_assert(
        std::is_same_v<
            decltype(stdx::safe_identity(declval<int volatile &&>())), int>);
    static_assert(std::is_same_v<decltype(stdx::safe_identity(
                                     declval<int const volatile &&>())),
                                 int>);
}

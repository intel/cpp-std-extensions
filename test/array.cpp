#include <stdx/array.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>

TEST_CASE("make_array (variadic arguments)", "[array]") {
    auto arr = stdx::make_array(1, 2, 3, 4, 5);
    STATIC_CHECK(std::is_same_v<decltype(arr), std::array<int, 5>>);
    CHECK(arr == std::array{1, 2, 3, 4, 5});
}

namespace {
template <auto I> using V = std::integral_constant<decltype(I), I + 1>;
}

TEST_CASE("make_array by sequence (template with ::value)", "[array]") {
    auto arr = stdx::make_array<V>(std::make_integer_sequence<int, 5>{});
    STATIC_CHECK(std::is_same_v<decltype(arr), std::array<int, 5>>);
    CHECK(arr == std::array{1, 2, 3, 4, 5});
}

TEST_CASE("make_array by extent (template with ::value)", "[array]") {
    auto arr = stdx::make_array<V, 5>();
    STATIC_CHECK(std::is_same_v<decltype(arr), std::array<int, 5>>);
    CHECK(arr == std::array{1, 2, 3, 4, 5});
}

namespace {
struct {
    template <auto I> auto operator()() { return I + 1; }
} f;
} // namespace

TEST_CASE("make_array by sequence (factory function template)", "[array]") {
    auto arr = stdx::make_array(std::make_integer_sequence<int, 5>{}, f);
    STATIC_CHECK(std::is_same_v<decltype(arr), std::array<int, 5>>);
    CHECK(arr == std::array{1, 2, 3, 4, 5});
}

TEST_CASE("make_array by extent (factory function template)", "[array]") {
    auto arr = stdx::make_array<5>(f);
    STATIC_CHECK(std::is_same_v<decltype(arr), std::array<int, 5>>);
    CHECK(arr == std::array{1, 2, 3, 4, 5});
}

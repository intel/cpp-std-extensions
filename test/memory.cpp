#include <stdx/memory.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <memory>
#include <vector>

TEST_CASE("to_address for pointer (constexpr)", "[memory]") {
    constexpr static int a{};
    constexpr static auto p = &a;
    STATIC_REQUIRE(stdx::to_address(p) == p);
}

TEST_CASE("to_address for pointer (runtime)", "[memory]") {
    int a; // deliberately uninitialized
    auto p = &a;
    CHECK(stdx::to_address(p) == p);
}

TEST_CASE("to_address for iterator", "[memory]") {
    std::vector<int> v{1, 2, 3};
    auto p = std::begin(v);
    CHECK(stdx::to_address(p) == std::data(v));
}

namespace {
struct fancy {};
} // namespace

template <> struct std::pointer_traits<fancy> {
    constexpr static auto to_address(fancy) { return 42; }
};

TEST_CASE("to_address for fancy pointer", "[memory]") {
    fancy f{};
    CHECK(stdx::to_address(f) == 42);
}

namespace {
struct fancier {
    constexpr static int value = 17;
    constexpr auto operator->() const { return &value; }
};
} // namespace

template <> struct std::pointer_traits<fancier> {
    constexpr static auto to_address(fancier) { return 42; }
};

TEST_CASE("to_address for fancy pointer (prefer pointer_traits)", "[memory]") {
    fancier f{};
    CHECK(stdx::to_address(f) == 42);
}

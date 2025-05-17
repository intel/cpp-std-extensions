#include <stdx/latched.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("construction", "[latched]") {
    auto c = stdx::latched{[] { return 42; }};
    CHECK(not c);
    CHECK(not c.has_value());
}

TEST_CASE("exposed value_type", "[latched]") {
    auto c = stdx::latched{[] { return 42; }};
    static_assert(std::is_same_v<typename decltype(c)::value_type, int>);
}

TEST_CASE("latched_value_t", "[latched]") {
    auto const c = stdx::latched{[] { return 42; }};
    static_assert(std::is_same_v<stdx::latched_value_t<decltype(c)>, int>);
}

TEST_CASE("automatic population", "[latched]") {
    auto c = stdx::latched{[] { return 42; }};
    CHECK(c.value() == 42);
    CHECK(c.has_value());
}

TEST_CASE("operator*", "[latched]") {
    auto c = stdx::latched{[] { return 42; }};
    CHECK(*c == 42);
    CHECK(c.has_value());
}

namespace {
struct S {
    int x{};
};
} // namespace

TEST_CASE("operator->", "[latched]") {
    auto c = stdx::latched{[] { return S{42}; }};
    CHECK(c->x == 42);
    CHECK(c.has_value());
}

namespace {
struct move_only {
    constexpr move_only(int i) : value{i} {}
    constexpr move_only(move_only &&) = default;
    int value{};
};

struct non_movable {
    constexpr non_movable(int i) : value{i} {}
    constexpr non_movable(non_movable &&) = delete;
    int value{};
};
} // namespace

TEST_CASE("move-only value", "[latched]") {
    auto c = stdx::latched{[] { return move_only{42}; }};
    CHECK(c->value == 42);
}

TEST_CASE("non-movable value", "[latched]") {
    auto c = stdx::latched{[] { return non_movable{42}; }};
    CHECK(c->value == 42);
}

TEST_CASE("preserving value categories", "[latched]") {
    {
        auto c = stdx::latched{[] { return 42; }};
        static_assert(std::is_same_v<int const &, decltype(*c)>);
        static_assert(std::is_same_v<int const &&, decltype(*std::move(c))>);
    }
    {
        auto const c = stdx::latched{[] { return 42; }};
        static_assert(std::is_same_v<int const &, decltype(*c)>);
        static_assert(std::is_same_v<int const &&, decltype(*std::move(c))>);
    }
}

#include <stdx/cached.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("construction", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    CHECK(not c);
    CHECK(not c.has_value());
}

TEST_CASE("exposed value_type", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    static_assert(std::is_same_v<typename decltype(c)::value_type, int>);
}

TEST_CASE("cached_value_t", "[cached]") {
    auto const c = stdx::cached{[] { return 42; }};
    static_assert(std::is_same_v<stdx::cached_value_t<decltype(c)>, int>);
}

TEST_CASE("automatic population", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    CHECK(c.value() == 42);
    CHECK(c.has_value());
}

TEST_CASE("operator*", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    CHECK(*c == 42);
    CHECK(c.has_value());
}

namespace {
struct S {
    int x{};
};
} // namespace

TEST_CASE("operator->", "[cached]") {
    auto c = stdx::cached{[] { return S{42}; }};
    CHECK(c->x == 42);
    CHECK(c.has_value());
}

TEST_CASE("reset clears value", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    CHECK(*c == 42);
    c.reset();
    CHECK(not c.has_value());
}

TEST_CASE("refresh immediately repopulates value", "[cached]") {
    int count{};
    auto c = stdx::cached{[&] {
        ++count;
        return 42;
    }};
    CHECK(*c == 42);
    CHECK(count == 1);
    c.refresh();
    CHECK(count == 2);
}

TEST_CASE("refresh returns a reference to value", "[cached]") {
    auto c = stdx::cached{[] { return 42; }};
    static_assert(std::is_same_v<decltype(c.refresh()), int &>);
    CHECK(c.refresh() == 42);
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

TEST_CASE("move-only value", "[cached]") {
    auto c = stdx::cached{[] { return move_only{42}; }};
    CHECK(c->value == 42);
}

TEST_CASE("non-movable value", "[cached]") {
    auto c = stdx::cached{[] { return non_movable{42}; }};
    CHECK(c->value == 42);
}

TEST_CASE("preserving value categories", "[cached]") {
    {
        auto c = stdx::cached{[] { return 42; }};
        static_assert(std::is_same_v<int &, decltype(*c)>);
        static_assert(std::is_same_v<int &&, decltype(*std::move(c))>);
    }
    {
        auto const c = stdx::cached{[] { return 42; }};
        static_assert(std::is_same_v<int const &, decltype(*c)>);
        static_assert(std::is_same_v<int const &&, decltype(*std::move(c))>);
    }
}

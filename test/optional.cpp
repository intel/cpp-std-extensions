#include <stdx/optional.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace {
enum struct E : std::uint8_t { VALUE, ALT_VALUE };

struct S {
    constexpr explicit S(int i) : value{i} {}
    constexpr friend auto operator==(S const &x, S const &y) -> bool {
        return x.value == y.value;
    }
    constexpr friend auto operator<(S const &x, S const &y) -> bool {
        return x.value < y.value;
    }
    int value{};
};
} // namespace

template <> struct stdx::tombstone_traits<E> {
    constexpr auto operator()() const {
        return static_cast<E>(std::uint8_t{0xffu});
    }
};

template <> struct stdx::tombstone_traits<S> {
    constexpr auto operator()() const { return S{-1}; }
};

TEST_CASE("sizeof(optional<T>) == sizeof(T)", "[optional]") {
    using O1 = stdx::optional<E>;
    static_assert(sizeof(O1) == sizeof(E));
    using O2 = stdx::optional<S>;
    static_assert(sizeof(O2) == sizeof(S));
}

TEST_CASE("alignof(optional<T>) == alignof(T)", "[optional]") {
    using O1 = stdx::optional<E>;
    static_assert(alignof(O1) == alignof(E));
    using O2 = stdx::optional<S>;
    static_assert(alignof(O2) == alignof(S));
}

TEST_CASE("optional exposes value_type", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_same_v<typename O::value_type, E>);
}

TEST_CASE("default construction", "[optional]") {
    constexpr auto o = stdx::optional<E>{};
    static_assert(not o);
}

TEST_CASE("construction from nullopt", "[optional]") {
    constexpr auto o = stdx::optional<E>{std::nullopt};
    static_assert(not o);
}

TEST_CASE("construction from value", "[optional]") {
    constexpr auto o = stdx::optional<E>{E::VALUE};
    static_assert(o);
}

TEST_CASE("trivially copy constructible", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_trivially_copy_constructible_v<O>);
}

TEST_CASE("trivially move constructible", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_trivially_move_constructible_v<O>);
}

TEST_CASE("trivially destructible", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_trivially_destructible_v<O>);
}

TEST_CASE("in-place construction", "[optional]") {
    using O = stdx::optional<S>;
    constexpr O o{std::in_place, 42};
    static_assert(o);
}

TEST_CASE("in-place construction (no args)", "[optional]") {
    using O = stdx::optional<E>;
    constexpr O o{std::in_place};
    static_assert(o);
}

TEST_CASE("retrieve value", "[optional]") {
    auto o = stdx::optional<E>{E::VALUE};
    CHECK(o.value() == E::VALUE);
}

TEST_CASE("operator*", "[optional]") {
    auto o = stdx::optional<E>{E::VALUE};
    CHECK(*o == E::VALUE);
}

TEST_CASE("assignment from nullopt", "[optional]") {
    auto o = stdx::optional<E>{E::VALUE};
    o = std::nullopt;
    CHECK(not o);
}

TEST_CASE("assignment from value", "[optional]") {
    auto o = stdx::optional<E>{};
    o = E::VALUE;
    REQUIRE(o);
    CHECK(*o == E::VALUE);
}

TEST_CASE("trivially copy assignable", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_trivially_copy_assignable_v<O>);
}

TEST_CASE("trivially move assignable", "[optional]") {
    using O = stdx::optional<E>;
    static_assert(std::is_trivially_move_assignable_v<O>);
}

TEST_CASE("has_value and boolean conversion", "[optional]") {
    constexpr auto o = stdx::optional<E>{E::VALUE};
    static_assert(o.has_value());
    static_assert(o);
}

TEST_CASE("operator->", "[optional]") {
    auto o = stdx::optional<S>{42};
    CHECK(o->value == 42);
}

TEST_CASE("value() preserves value categories", "[optional]") {
    {
        auto o = stdx::optional<E>{E::VALUE};
        static_assert(std::is_same_v<E &, decltype(*o)>);
        static_assert(std::is_same_v<E &&, decltype(*std::move(o))>);
    }
    {
        auto const o = stdx::optional<E>{E::VALUE};
        static_assert(std::is_same_v<E const &, decltype(*o)>);
        static_assert(std::is_same_v<E const &&, decltype(*std::move(o))>);
    }
}

TEST_CASE("value or default", "[optional]") {
    auto o1 = stdx::optional<E>{E::VALUE};
    CHECK(o1.value_or(E::ALT_VALUE) == E::VALUE);
    auto o2 = stdx::optional<E>{};
    CHECK(o2.value_or(E::ALT_VALUE) == E::ALT_VALUE);
}

TEST_CASE("reset clears value", "[optional]") {
    auto o = stdx::optional<E>{E::VALUE};
    o.reset();
    CHECK(not o.has_value());
}

TEST_CASE("emplace constructs value in-place", "[optional]") {
    auto o1 = stdx::optional<S>{17};
    auto &v = o1.emplace(42);
    CHECK(v.value == 42);
    CHECK(o1->value == 42);
}

TEST_CASE("equality", "[optional]") {
    auto o = stdx::optional<S>{17};
    CHECK(o == stdx::optional<S>{17});
    CHECK(o != stdx::optional<S>{42});
}

TEST_CASE("less than - both engaged", "[optional]") {
    auto o1 = stdx::optional<S>{17};
    auto o2 = stdx::optional<S>{42};
    CHECK(o1 < o2);
    CHECK(not(o1 < o1));
}

TEST_CASE("less than - neither engaged", "[optional]") {
    auto o1 = stdx::optional<S>{};
    auto o2 = stdx::optional<S>{};
    CHECK(not(o1 < o2));
}

TEST_CASE("less than - lhs engaged", "[optional]") {
    auto o1 = stdx::optional<S>{42};
    auto o2 = stdx::optional<S>{};
    CHECK(not(o1 < o2));
}

TEST_CASE("less than - rhs engaged", "[optional]") {
    auto o1 = stdx::optional<S>{};
    auto o2 = stdx::optional<S>{42};
    CHECK(o1 < o2);
}

TEST_CASE("ordering comparisons", "[optional]") {
    auto o = stdx::optional<S>{17};
    CHECK(o < stdx::optional<S>{42});
    CHECK(o <= stdx::optional<S>{42});
    CHECK(o > stdx::optional<S>{3});
    CHECK(o >= stdx::optional<S>{3});
}

TEST_CASE("transform (mutable lvalue ref)", "[optional]") {
    auto o1 = stdx::optional<S>{17};
    auto o2 = o1.transform([](S &) { return E::VALUE; });
    CHECK(o2.value() == E::VALUE);

    auto o3 = stdx::optional<S>{};
    auto o4 = o3.transform([](S &) { return E::VALUE; });
    CHECK(not o4);
}

TEST_CASE("transform (const lvalue ref)", "[optional]") {
    auto const o1 = stdx::optional<S>{17};
    auto o2 = o1.transform([](S const &) { return E::VALUE; });
    CHECK(o2.value() == E::VALUE);

    auto const o3 = stdx::optional<S>{};
    auto o4 = o3.transform([](S const &) { return E::VALUE; });
    CHECK(not o4);
}

TEST_CASE("transform (mutable rvalue ref)", "[optional]") {
    auto o1 = stdx::optional<S>{17};
    auto o2 = std::move(o1).transform([](S &&) { return E::VALUE; });
    CHECK(o2.value() == E::VALUE);

    auto o3 = stdx::optional<S>{};
    auto o4 = std::move(o3).transform([](S &&) { return E::VALUE; });
    CHECK(not o4);
}

TEST_CASE("transform (const rvalue ref)", "[optional]") {
    auto const o1 = stdx::optional<S>{17};
    auto o2 = std::move(o1).transform([](S const &&) { return E::VALUE; });
    CHECK(o2.value() == E::VALUE);

    auto const o3 = stdx::optional<S>{};
    auto o4 = std::move(o3).transform([](S const &&) { return E::VALUE; });
    CHECK(not o4);
}

TEST_CASE("or_else (const lvalue ref)", "[optional]") {
    auto const o1 = stdx::optional<E>{E::VALUE};
    auto o2 = o1.or_else([] { return stdx::optional<E>{E::ALT_VALUE}; });
    CHECK(o2.value() == E::VALUE);

    auto const o3 = stdx::optional<E>{};
    auto o4 = o3.or_else([] { return stdx::optional<E>{E::ALT_VALUE}; });
    CHECK(o4.value() == E::ALT_VALUE);
}

TEST_CASE("or_else (mutable rvalue ref)", "[optional]") {
    auto o1 = stdx::optional<E>{E::VALUE};
    auto o2 =
        std::move(o1).or_else([] { return stdx::optional<E>{E::ALT_VALUE}; });
    CHECK(o2.value() == E::VALUE);

    auto o3 = stdx::optional<E>{};
    auto o4 =
        std::move(o3).or_else([] { return stdx::optional<E>{E::ALT_VALUE}; });
    CHECK(o4.value() == E::ALT_VALUE);
}

TEST_CASE("and_then (mutable lvalue ref)", "[optional]") {
    auto o1 = stdx::optional<S>{42};
    auto o2 = o1.and_then([](S &s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(o2->value == 43);

    auto o3 = stdx::optional<S>{};
    auto o4 = o3.and_then([](S &s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(not o4);
}

TEST_CASE("and_then (const lvalue ref)", "[optional]") {
    auto const o1 = stdx::optional<S>{42};
    auto o2 =
        o1.and_then([](S const &s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(o2->value == 43);

    auto const o3 = stdx::optional<S>{};
    auto o4 =
        o3.and_then([](S const &s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(not o4);
}

TEST_CASE("and_then (mutable rvalue ref)", "[optional]") {
    auto o1 = stdx::optional<S>{42};
    auto o2 = std::move(o1).and_then(
        [](S &&s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(o2->value == 43);

    auto o3 = stdx::optional<S>{};
    auto o4 = std::move(o3).and_then(
        [](S &&s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(not o4);
}

TEST_CASE("and_then (const rvalue ref)", "[optional]") {
    auto const o1 = stdx::optional<S>{42};
    auto o2 = std::move(o1).and_then(
        [](S const &&s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(o2->value == 43);

    auto const o3 = stdx::optional<S>{};
    auto o4 = std::move(o3).and_then(
        [](S const &&s) { return stdx::optional<S>{s.value + 1}; });
    CHECK(not o4);
}

TEST_CASE("transform (multi-arg)", "[optional]") {
    auto o1 = stdx::optional<S>{17};
    auto o2 = stdx::optional<S>{42};
    auto o3 =
        transform([](S &x, S &y) { return S{x.value + y.value}; }, o1, o2);
    CHECK(o3->value == 59);
}

namespace {
struct move_only {
    constexpr move_only(int i) : value{i} {}
    constexpr move_only(move_only &&) = default;
    int value{};
    constexpr friend auto operator==(move_only const &x,
                                     move_only const &y) -> bool {
        return x.value == y.value;
    }
};

struct non_movable {
    constexpr non_movable(int i) : value{i} {}
    constexpr non_movable(non_movable &&) = delete;
    int value{};
    constexpr friend auto operator==(non_movable const &x,
                                     non_movable const &y) -> bool {
        return x.value == y.value;
    }
};
} // namespace

template <> struct stdx::tombstone_traits<move_only> {
    constexpr auto operator()() const { return move_only{-1}; }
};
template <> struct stdx::tombstone_traits<non_movable> {
    constexpr auto operator()() const { return non_movable{-1}; }
};

TEST_CASE("optional can hold move-only types)", "[optional]") {
    auto o = stdx::optional<move_only>{17};
    CHECK(o->value == 17);
    o.emplace(42);
    CHECK(o->value == 42);
    o.reset();
    CHECK(not o);
}

TEST_CASE("optional can hold non-movable types)", "[optional]") {
    auto o = stdx::optional<non_movable>{17};
    CHECK(o->value == 17);
    o.emplace(42);
    CHECK(o->value == 42);
    o.reset();
    CHECK(not o);
}

TEST_CASE("CTAD", "[optional]") {
    [[maybe_unused]] auto o = stdx::optional{E::VALUE};
    static_assert(std::is_same_v<decltype(o), stdx::optional<E>>);
}

TEST_CASE("easy tombstone with value", "[optional]") {
    constexpr auto o = stdx::optional<int, stdx::tombstone_value<-1>>{};
    static_assert(not o);
    static_assert(*o == -1);
}

TEST_CASE("optional floating-point value has default sentinel", "[optional]") {
    auto const o1 = stdx::optional<float>{};
    CHECK(not o1);
    CHECK(std::isinf(*o1));
    auto const o2 = stdx::optional{1.0f};
    CHECK(o1 < o2);
}

TEST_CASE("optional pointer value has default sentinel", "[optional]") {
    auto const o1 = stdx::optional<float *>{};
    CHECK(not o1);
    CHECK(*o1 == nullptr);
    float f{1.0f};
    auto const o2 = stdx::optional<float *>{&f};
    CHECK(**o2 == 1.0f);
    CHECK(o1 < o2);
}

TEST_CASE("transform (non-movable)", "[optional]") {
    auto o1 = stdx::optional<non_movable>{17};
    auto o2 = o1.transform([](auto &x) { return non_movable{x.value + 42}; });
    CHECK(o2->value == 59);
}

TEST_CASE("transform (multi-arg nonmovable)", "[optional]") {
    auto o1 = stdx::optional<non_movable>{17};
    auto o2 = stdx::optional<non_movable>{42};
    auto o3 = transform(
        [](auto &x, auto &y) { return non_movable{x.value + y.value}; }, o1,
        o2);
    CHECK(o3->value == 59);
}

#if __cpp_nontype_template_args >= 201911L
TEST_CASE("tombstone with non-structural value", "[optional]") {
    constexpr auto ts_value = CX_VALUE(std::string_view{});
    auto const o =
        stdx::optional<std::string_view, stdx::tombstone_value<ts_value>>{};
    CHECK(not o);
    CHECK(*o == std::string_view{});
}
#endif

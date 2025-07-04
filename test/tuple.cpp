#include "detail/tuple_types.hpp"

#include <stdx/tuple.hpp>
#include <stdx/tuple_destructure.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <compare>
#include <type_traits>
#include <utility>

TEST_CASE("empty tuple", "[tuple]") {
    STATIC_REQUIRE(std::is_empty_v<stdx::tuple<>>);
    constexpr auto t = stdx::tuple{};
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(std::is_same_v<T, stdx::tuple<>>);
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 0);
    STATIC_REQUIRE(T::size() == 0);
}

TEST_CASE("single element tuple", "[tuple]") {
    constexpr auto t = stdx::tuple{1};
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(std::is_same_v<T, stdx::tuple<int>>);
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 1);
    STATIC_REQUIRE(T::size() == 1);
    STATIC_REQUIRE(sizeof(T) == sizeof(int));

    auto u = stdx::tuple{1};
    using U = decltype(u);
    STATIC_REQUIRE(std::is_same_v<U, stdx::tuple<int>>);
    STATIC_REQUIRE(stdx::tuple_size_v<U> == 1);
    STATIC_REQUIRE(U::size() == 1);
    STATIC_REQUIRE(sizeof(U) == sizeof(int));
}

TEST_CASE("multi element tuple", "[tuple]") {
    constexpr auto t = stdx::tuple{1, 2.0f};
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(std::is_same_v<T, stdx::tuple<int, float>>);
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 2);
    STATIC_REQUIRE(T::size() == 2);
}

TEST_CASE("constexpr tuple of references", "[tuple]") {
    constexpr static int x = 1;
    constexpr auto t = stdx::tuple<int const &>{x};
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 1);
    STATIC_REQUIRE(T::size() == 1);
}

TEST_CASE("free get", "[tuple]") {
    constexpr auto t = stdx::tuple{5, true, 10l};

    CHECK(stdx::get<0>(t) == 5);
    CHECK(stdx::get<1>(t));
    CHECK(stdx::get<2>(t) == 10);
    STATIC_REQUIRE(stdx::get<0>(t) == 5);
    STATIC_REQUIRE(stdx::get<1>(t));
    STATIC_REQUIRE(stdx::get<2>(t) == 10);

    CHECK(stdx::get<int>(t) == 5);
    CHECK(stdx::get<bool>(t));
    CHECK(stdx::get<long>(t) == 10);
    STATIC_REQUIRE(stdx::get<int>(t) == 5);
    STATIC_REQUIRE(stdx::get<bool>(t));
    STATIC_REQUIRE(stdx::get<long>(t) == 10);
}

TEST_CASE("free get (ADL)", "[tuple]") {
    constexpr auto t = stdx::tuple{5, true, 10l};

    CHECK(get<0>(t) == 5);
    CHECK(get<1>(t));
    CHECK(get<2>(t) == 10);
    STATIC_REQUIRE(get<0>(t) == 5);
    STATIC_REQUIRE(get<1>(t));
    STATIC_REQUIRE(get<2>(t) == 10);

    CHECK(get<int>(t) == 5);
    CHECK(get<bool>(t));
    CHECK(get<long>(t) == 10);
    STATIC_REQUIRE(get<int>(t) == 5);
    STATIC_REQUIRE(get<bool>(t));
    STATIC_REQUIRE(get<long>(t) == 10);
}

TEST_CASE("free get value categories", "[tuple]") {
    {
        auto const t = stdx::tuple{42};
        STATIC_REQUIRE(std::is_same_v<decltype(get<0>(t)), int const &>);
        STATIC_REQUIRE(std::is_same_v<decltype(get<int>(t)), int const &>);
    }
    {
        auto t = stdx::tuple{42};
        STATIC_REQUIRE(std::is_same_v<decltype(get<0>(t)), int &>);
        STATIC_REQUIRE(std::is_same_v<decltype(get<int>(t)), int &>);
        STATIC_REQUIRE(std::is_same_v<decltype(get<0>(std::move(t))), int &&>);
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<int>(std::move(t))), int &&>);
    }
}

TEST_CASE("indexing", "[tuple]") {
    using namespace stdx::literals;

    constexpr auto t = stdx::tuple{5, true, 10l};
    STATIC_REQUIRE(t[0_idx] == 5);
    STATIC_REQUIRE(t[1_idx] == true);
    STATIC_REQUIRE(t[2_idx] == 10l);

    STATIC_REQUIRE(std::is_same_v<decltype(t[0_idx]), int const &>);
    auto u = stdx::tuple{1};
    STATIC_REQUIRE(std::is_same_v<decltype(u[0_idx]), int &>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::tuple{1}[0_idx]), int &&>);
}

TEST_CASE("indexing with small literals", "[tuple]") {
    using namespace stdx::literals;

    constexpr auto t = stdx::tuple{5, true, 10l};
    CHECK(get<"index"_0>(t) == 5);
    CHECK(get<"index"_1>(t));
    CHECK(get<"index"_2>(t) == 10);
    STATIC_REQUIRE(t["index"_0] == 5);
    STATIC_REQUIRE(t["index"_1]);
    STATIC_REQUIRE(t["index"_2] == 10l);
}

TEST_CASE("tuple of lvalue references", "[tuple]") {
    auto x = 1;
    auto t = stdx::tuple<int &>{x};
    CHECK(get<0>(t) == 1);
    get<0>(t) = 2;
    CHECK(get<0>(t) == 2);
    CHECK(x == 2);
}

TEST_CASE("tuple of lambdas", "[tuple]") {
    auto x = 1;
    auto t = stdx::tuple{[&] { x += 2; }, [&] { x += 3; }};
    get<0>(t)();
    CHECK(x == 3);
}

TEST_CASE("tuple size/elements", "[tuple]") {
    using T = stdx::tuple<int, bool>;
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 2);
    STATIC_REQUIRE(std::is_same_v<stdx::tuple_element_t<0, T>, int>);
    STATIC_REQUIRE(std::is_same_v<stdx::tuple_element_t<1, T>, bool>);

    using A = stdx::tuple<int &>;
    STATIC_REQUIRE(std::is_same_v<stdx::tuple_element_t<0, A>, int &>);
    using B = stdx::tuple<int const &>;
    STATIC_REQUIRE(std::is_same_v<stdx::tuple_element_t<0, B>, int const &>);
    using C = stdx::tuple<int &&>;
    STATIC_REQUIRE(std::is_same_v<stdx::tuple_element_t<0, C>, int &&>);
}

TEST_CASE("destructuring", "[tuple]") {
    auto const t = stdx::tuple{1, 3.14f};
    auto const [i, f] = t;
    CHECK(i == 1);
    CHECK(f == 3.14f);
}

namespace {
struct A {};
struct B {
    B(int) {}
};
} // namespace

TEST_CASE("default constructability", "[tuple]") {
    STATIC_REQUIRE(std::is_default_constructible_v<stdx::tuple<A>>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<stdx::tuple<A>>);
    STATIC_REQUIRE(not std::is_default_constructible_v<stdx::tuple<B>>);
    STATIC_REQUIRE(not std::is_nothrow_default_constructible_v<stdx::tuple<B>>);
}

TEMPLATE_TEST_CASE("constructability", "[tuple]",
                   (stdx::detail::element<0, int>), stdx::tuple<>,
                   (stdx::tuple<A, int, bool>)) {
    STATIC_REQUIRE(std::is_default_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<TestType>);
}

TEMPLATE_TEST_CASE("copyability", "[tuple]", (stdx::detail::element<0, int>),
                   stdx::tuple<>, (stdx::tuple<A, int, bool>)) {
    STATIC_REQUIRE(std::is_copy_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_copy_assignable_v<TestType>);
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<TestType>);
    STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TestType>);
}

TEMPLATE_TEST_CASE("moveability", "[tuple]", (stdx::detail::element<0, int>),
                   stdx::tuple<>, (stdx::tuple<A, int, bool>)) {
    STATIC_REQUIRE(std::is_move_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_move_assignable_v<TestType>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<TestType>);
    STATIC_REQUIRE(std::is_trivially_move_constructible_v<TestType>);
    STATIC_REQUIRE(std::is_trivially_move_assignable_v<TestType>);
}

TEMPLATE_TEST_CASE("destructability", "[tuple]",
                   (stdx::detail::element<0, int>), stdx::tuple<>,
                   (stdx::tuple<A, int, bool>)) {
    STATIC_REQUIRE(std::is_nothrow_destructible_v<TestType>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<TestType>);
}

TEST_CASE("move-only types", "[tuple]") {
    STATIC_REQUIRE(std::is_default_constructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<stdx::tuple<move_only>>);

    STATIC_REQUIRE(not std::is_copy_constructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(not std::is_copy_assignable_v<stdx::tuple<move_only>>);

    STATIC_REQUIRE(std::is_move_constructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(std::is_move_assignable_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(
        std::is_trivially_move_constructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(std::is_trivially_move_assignable_v<stdx::tuple<move_only>>);

    STATIC_REQUIRE(std::is_nothrow_destructible_v<stdx::tuple<move_only>>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<stdx::tuple<move_only>>);
}

TEST_CASE("equality comparable", "[tuple]") {
    constexpr auto t = stdx::tuple{5, 10};

    REQUIRE(t == t);
    REQUIRE(t != stdx::tuple{5, 11});
    STATIC_REQUIRE(t == stdx::tuple{5, 10});
    STATIC_REQUIRE(t != stdx::tuple{5, 11});
}

TEST_CASE("equality comparable (tuple of references)", "[tuple]") {
    int x{5};
    int y{5};
    auto const t = stdx::tuple<int &>{x};
    auto const u = stdx::tuple<int &>{y};

    CHECK(t == t);
    CHECK(t == u);
}

TEST_CASE("equality comparable (references and non-references)", "[tuple]") {
    int x{5};
    auto const t = stdx::tuple<int &>{x};
    auto const u = stdx::tuple<int>{5};
    CHECK(t == u);
    CHECK(u == t);
}

TEST_CASE("equality comparable (conversions)", "[tuple]") {
    constexpr auto t = stdx::tuple{1};
    STATIC_REQUIRE(t == stdx::tuple{1.0});
}

namespace {
struct eq {
    [[nodiscard]] friend constexpr auto operator==(eq lhs, eq rhs) -> bool {
        return lhs.x == rhs.x;
    }
    int x;
};

struct eq_derived : eq {};
} // namespace

TEST_CASE("equality comparable (user-defined)", "[tuple]") {
    constexpr auto t = stdx::tuple{eq{1}};

    STATIC_REQUIRE(t == stdx::tuple{eq{1}});
    STATIC_REQUIRE(t != stdx::tuple{eq{2}});
}

TEST_CASE("equality comparable (conversions, user-defined)", "[tuple]") {
    constexpr auto t = stdx::tuple{eq{1}};
    STATIC_REQUIRE(t == stdx::tuple{eq_derived{1}});
}

TEST_CASE("order comparable", "[tuple]") {
    constexpr auto t = stdx::tuple{5, 10};

    REQUIRE(t < stdx::tuple{6, 9});
    REQUIRE(t < stdx::tuple{5, 11});
    REQUIRE(not(t < t));
    REQUIRE(not(t < stdx::tuple{4, 11}));
    STATIC_REQUIRE(t < stdx::tuple{6, 9});
    STATIC_REQUIRE(t < stdx::tuple{5, 11});
    STATIC_REQUIRE(not(t < t)); // NOLINT(misc-redundant-expression)
    STATIC_REQUIRE(not(t < stdx::tuple{4, 11}));

    REQUIRE(t <= t);
    REQUIRE(t <= stdx::tuple{6, 9});
    REQUIRE(t <= stdx::tuple{5, 11});
    REQUIRE(not(t <= stdx::tuple{5, 9}));
    REQUIRE(not(t <= stdx::tuple{4, 11}));
    STATIC_REQUIRE(t <= t); // NOLINT(misc-redundant-expression)
    STATIC_REQUIRE(t <= stdx::tuple{6, 9});
    STATIC_REQUIRE(t <= stdx::tuple{5, 11});
    STATIC_REQUIRE(not(t <= stdx::tuple{5, 9}));
    STATIC_REQUIRE(not(t <= stdx::tuple{4, 11}));

    REQUIRE(t > stdx::tuple{5, 9});
    REQUIRE(t > stdx::tuple{4, 11});
    REQUIRE(not(t > t));
    REQUIRE(not(t > stdx::tuple{6, 9}));
    STATIC_REQUIRE(t > stdx::tuple{5, 9});
    STATIC_REQUIRE(t > stdx::tuple{4, 11});
    STATIC_REQUIRE(not(t > t)); // NOLINT(misc-redundant-expression)
    STATIC_REQUIRE(not(t > stdx::tuple{6, 9}));

    REQUIRE(t >= t);
    REQUIRE(t >= stdx::tuple{5, 9});
    REQUIRE(t >= stdx::tuple{4, 11});
    REQUIRE(not(t >= stdx::tuple{5, 11}));
    REQUIRE(not(t >= stdx::tuple{6, 9}));
    STATIC_REQUIRE(t >= t); // NOLINT(misc-redundant-expression)
    STATIC_REQUIRE(t >= stdx::tuple{5, 9});
    STATIC_REQUIRE(t >= stdx::tuple{4, 11});
    STATIC_REQUIRE(not(t >= stdx::tuple{5, 11}));
    STATIC_REQUIRE(not(t >= stdx::tuple{6, 9}));
}

TEST_CASE("order comparable (references and non-references)", "[tuple]") {
    int x{5};
    int y{6};
    auto const t = stdx::tuple<int &, int &>{x, y};
    auto const u = stdx::tuple<int, int>{5, 7};
    CHECK(t < u);
    CHECK(u > t);
}

TEST_CASE("spaceship comparable", "[tuple]") {
    constexpr auto t = stdx::tuple{5, 10};

    REQUIRE(t <=> t == std::strong_ordering::equal);
    REQUIRE(t <=> stdx::tuple{6, 9} == std::strong_ordering::less);
    REQUIRE(t <=> stdx::tuple{6, 10} == std::strong_ordering::less);
    REQUIRE(t <=> stdx::tuple{5, 11} == std::strong_ordering::less);
    REQUIRE(t <=> stdx::tuple{5, 9} == std::strong_ordering::greater);
    REQUIRE(t <=> stdx::tuple{4, 10} == std::strong_ordering::greater);
    REQUIRE(t <=> stdx::tuple{4, 11} == std::strong_ordering::greater);
    STATIC_REQUIRE(t <=> t == std::strong_ordering::equal);
    STATIC_REQUIRE(t <=> stdx::tuple{6, 9} == std::strong_ordering::less);
    STATIC_REQUIRE(t <=> stdx::tuple{6, 10} == std::strong_ordering::less);
    STATIC_REQUIRE(t <=> stdx::tuple{5, 11} == std::strong_ordering::less);
    STATIC_REQUIRE(t <=> stdx::tuple{5, 9} == std::strong_ordering::greater);
    STATIC_REQUIRE(t <=> stdx::tuple{4, 10} == std::strong_ordering::greater);
    STATIC_REQUIRE(t <=> stdx::tuple{4, 11} == std::strong_ordering::greater);
}

TEST_CASE("spaceship comparable (references and non-references)", "[tuple]") {
    int x{5};
    auto const t = stdx::tuple<int &>{x};
    auto const u = stdx::tuple<int>{5};
    CHECK(t <=> u == std::strong_ordering::equal);
    CHECK(u <=> t == std::strong_ordering::equal);
}

TEST_CASE("free get is SFINAE-friendly", "[tuple]") {
    constexpr auto t = []<typename... Ts>(stdx::tuple<Ts...> const &tup) {
        return stdx::tuple{get<Ts>(tup)...};
    }(stdx::tuple{});
    STATIC_REQUIRE(t == stdx::tuple{});
}

TEST_CASE("copy/move behavior for tuple", "[tuple]") {
    auto t1 = stdx::tuple{counter{}};

    counter::reset();
    [[maybe_unused]] auto t2 = t1;
    CHECK(counter::moves == 0);
    CHECK(counter::copies == 1);

    counter::reset();
    [[maybe_unused]] auto t3 = std::move(t1);
    CHECK(counter::moves == 1);
    CHECK(counter::copies == 0);
}

auto func_no_args() -> void;
auto func_one_arg(int) -> void;

TEST_CASE("make_tuple", "[tuple]") {
    STATIC_REQUIRE(stdx::make_tuple() == stdx::tuple{});
    STATIC_REQUIRE(stdx::make_tuple(1, 2, 3) == stdx::tuple{1, 2, 3});
    STATIC_REQUIRE(
        std::is_same_v<decltype(stdx::make_tuple(func_no_args, func_one_arg)),
                       stdx::tuple<void (*)(), void (*)(int)>>);

    constexpr auto t = stdx::make_tuple(stdx::tuple{});
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(std::is_same_v<T, stdx::tuple<stdx::tuple<>>>);
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 1);
    STATIC_REQUIRE(T::size() == 1);
}

namespace detail {
template <typename, typename> struct concat;

template <template <typename...> typename L, typename... Ts>
struct concat<L<Ts...>, L<>> {
    using type = L<Ts...>;
};

template <template <typename...> typename L, typename... Ts, typename U,
          typename... Us>
struct concat<L<Ts...>, L<U, Us...>> {
    using type = typename concat<L<Ts..., U>, L<Us...>>::type;
};
} // namespace detail

TEST_CASE("tuple type-based concat", "[tuple]") {
    using T = stdx::tuple<int>;
    using U = stdx::tuple<float>;
    STATIC_REQUIRE(std::is_same_v<typename detail::concat<T, U>::type,
                                  stdx::tuple<int, float>>);
}

TEST_CASE("forward_as_tuple", "[tuple]") {
    auto const x = 17;
    auto y = 17;
    auto z = 17;
    auto t = stdx::forward_as_tuple(x, y, std::move(z));
    STATIC_REQUIRE(
        std::is_same_v<decltype(t), stdx::tuple<int const &, int &, int &&>>);
    CHECK(t == stdx::tuple{17, 17, 17});
}

TEST_CASE("one_of", "[tuple]") {
    STATIC_REQUIRE(1 == stdx::one_of{1, 2, 3});
    STATIC_REQUIRE(4 != stdx::one_of{1, 2, 3});
    STATIC_REQUIRE(stdx::one_of{1, 2, 3} == 1);
    STATIC_REQUIRE(stdx::one_of{1, 2, 3} != 4);

    STATIC_REQUIRE(stdx::one_of{1, 2, 3} == stdx::one_of{3, 4, 5});
    STATIC_REQUIRE(stdx::one_of{1, 2, 3} != stdx::one_of{4, 5, 6});
}

TEST_CASE("indexing unambiguously", "[tuple]") {
    using namespace stdx::literals;

    constexpr auto t = stdx::tuple{42, stdx::tuple{17}};
    STATIC_REQUIRE(t[0_idx] == 42);
    STATIC_REQUIRE(t[1_idx][0_idx] == 17);
}

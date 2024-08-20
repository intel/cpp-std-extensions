#include "detail/tuple_types.hpp"

#include <stdx/tuple_algorithms.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <functional>
#include <type_traits>
#include <utility>

TEST_CASE("unary transform", "[tuple_algorithms]") {
    static_assert(stdx::transform([](auto) { return 1; }, stdx::tuple{}) ==
                  stdx::tuple{});
    constexpr auto t = stdx::tuple{1, 2, 3};
    constexpr auto u = stdx::transform([](auto x) { return x + 1; }, t);
    static_assert(u == stdx::tuple{2, 3, 4});
}

TEST_CASE("unary type transform", "[tuple_algorithms]") {
    static_assert(stdx::transform([](auto) { return 1; }, stdx::tuple{}) ==
                  stdx::tuple{});
    constexpr auto t = stdx::tuple{1, 0, 3};
    constexpr auto u =
        stdx::transform([](auto x) -> bool { return x != 0; }, t);
    static_assert(u == stdx::tuple{true, false, true});
}

TEST_CASE("n-ary transform", "[tuple_algorithms]") {
    static_assert(stdx::transform([](auto, auto) { return 1; }, stdx::tuple{},
                                  stdx::tuple{}) == stdx::tuple{});
    constexpr auto t = stdx::tuple{1, 2, 3};
    constexpr auto u =
        stdx::transform([](auto x, auto y) { return x + y; }, t, t);
    static_assert(u == stdx::tuple{2, 4, 6});
}

TEST_CASE("rvalue transform", "[tuple_algorithms]") {
    auto t = stdx::tuple{1, 2, 3};
    auto const u = stdx::transform([](int &&x) { return x + 1; }, std::move(t));
    CHECK(u == stdx::tuple{2, 3, 4});
}

TEST_CASE("transform preserves references", "[tuple_algorithms]") {
    int value{1};
    auto const u = stdx::transform(
        [&](auto i) -> int & {
            value += i;
            return value;
        },
        stdx::tuple{1});
    CHECK(std::addressof(value) == std::addressof(u[stdx::index<0>]));
}

namespace {
template <typename Key, typename Value> struct map_entry {
    using key_t = Key;
    using value_t = Value;

    value_t value;
};
template <typename T> using key_for = typename T::key_t;
} // namespace

TEST_CASE("transform with index", "[tuple_algorithms]") {
    struct X;
    constexpr auto t = stdx::transform<key_for>(
        [](auto value) { return map_entry<X, int>{value}; }, stdx::tuple{42});
    static_assert(stdx::get<X>(t).value == 42);
}

TEST_CASE("apply", "[tuple_algorithms]") {
    static_assert(stdx::apply([](auto... xs) { return (0 + ... + xs); },
                              stdx::tuple{}) == 0);
    static_assert(stdx::apply([](auto... xs) { return (0 + ... + xs); },
                              stdx::tuple{1, 2, 3}) == 6);

    auto stateful = [calls = 0](auto...) mutable { return ++calls; };
    CHECK(stdx::apply(stateful, stdx::tuple{1, 2, 3}) == 1);
    CHECK(stdx::apply(stateful, stdx::tuple{1, 2, 3}) == 2);

    static_assert(stdx::apply([](auto x) { return x.value; },
                              stdx::tuple{move_only{42}}) == 42);

    auto t = stdx::tuple{1, 2, 3};
    stdx::apply([](auto &...xs) { (++xs, ...); }, t);
    CHECK(t == stdx::tuple{2, 3, 4});
}

TEST_CASE("join", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(t.join(std::plus{}) == 6);
    static_assert(stdx::tuple{1, 2, 3}.join(std::plus{}) == 6);
    static_assert(
        stdx::tuple{move_only{42}}
            .join([](auto x, auto y) { return move_only{x.value + y.value}; })
            .value == 42);
}

TEST_CASE("join (single element)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1};
    static_assert(t.join(std::plus{}) == 1);
}

TEST_CASE("join with default for empty tuple", "[tuple_algorithms]") {
    static_assert(stdx::tuple{}.join(42, std::plus{}) == 42);
    static_assert(stdx::tuple{1}.join(42, std::plus{}) == 1);
    static_assert(stdx::tuple{1, 2, 3}.join(42, std::plus{}) == 6);
}

TEST_CASE("for_each", "[tuple_algorithms]") {
    {
        auto const t = stdx::tuple{};
        auto sum = 0;
        stdx::for_each([&](auto x, auto y) { sum += x + y; }, t, t);
        CHECK(sum == 0);
    }
    {
        auto const t = stdx::tuple{1, 2, 3};
        auto sum = 0;
        stdx::for_each([&](auto x, auto y) { sum += x + y; }, t, t);
        CHECK(sum == 12);
    }
    {
        auto const t = stdx::tuple{1};
        auto sum = 0;
        stdx::for_each([&](auto x, auto &&y) { sum += x + y.value; }, t,
                       stdx::tuple{move_only{2}});
        CHECK(sum == 3);
    }
    {
        auto const t = stdx::tuple{1, 2, 3};
        auto f = stdx::for_each(
            [calls = 0](auto) mutable {
                ++calls;
                return calls;
            },
            t);
        CHECK(f(0) == 4);
    }
}

TEST_CASE("unrolled_for_each on arrays", "[tuple_algorithms]") {
    auto a = std::array{1, 2, 3};
    auto sum = 0;
    stdx::unrolled_for_each(
        [&](auto &x, auto y) {
            sum += x + y;
            x--;
        },
        a, a);
    CHECK(sum == 12);
    CHECK(a == std::array{0, 1, 2});
}

TEST_CASE("tuple_cat", "[tuple_algorithms]") {
    static_assert(stdx::tuple_cat() == stdx::tuple{});
    static_assert(stdx::tuple_cat(stdx::tuple{}, stdx::tuple{}) ==
                  stdx::tuple{});
    static_assert(stdx::tuple_cat(stdx::tuple{1, 2}, stdx::tuple{}) ==
                  stdx::tuple{1, 2});
    static_assert(stdx::tuple_cat(stdx::tuple{}, stdx::tuple{1, 2}) ==
                  stdx::tuple{1, 2});
    static_assert(stdx::tuple_cat(stdx::tuple{1, 2}, stdx::tuple{3, 4}) ==
                  stdx::tuple{1, 2, 3, 4});
    static_assert(stdx::tuple_cat(stdx::tuple{1, 2}, stdx::tuple{3, 4},
                                  stdx::tuple{5, 6}) ==
                  stdx::tuple{1, 2, 3, 4, 5, 6});
    static_assert(
        stdx::tuple_cat(stdx::tuple{1, 2}, stdx::tuple{}, stdx::tuple{3, 4}) ==
        stdx::tuple{1, 2, 3, 4});

    auto t = stdx::tuple_cat(stdx::tuple{1}, stdx::tuple{2});
    static_assert(std::is_same_v<decltype(t), stdx::tuple<int, int>>);
}

TEST_CASE("tuple_cat (move only)", "[tuple_algorithms]") {
    auto t =
        stdx::tuple_cat(stdx::tuple{move_only{5}}, stdx::tuple{move_only{10}});
    static_assert(
        std::is_same_v<decltype(t), stdx::tuple<move_only, move_only>>);
    CHECK(t == stdx::tuple{move_only{5}, move_only{10}});
}

TEST_CASE("tuple_cat (references)", "[tuple_algorithms]") {
    auto x = 1;
    auto t = stdx::tuple_cat(stdx::tuple<int &>{x}, stdx::tuple<int &>{x});
    static_assert(std::is_same_v<decltype(t), stdx::tuple<int &, int &>>);
    stdx::get<0>(t) = 2;
    CHECK(x == 2);
    stdx::get<1>(t) = 1;
    CHECK(x == 1);
}

TEST_CASE("tuple_cat (const references)", "[tuple_algorithms]") {
    auto x = 1;
    auto t = stdx::tuple_cat(stdx::tuple<int const &>{x},
                             stdx::tuple<int const &>{x});
    static_assert(
        std::is_same_v<decltype(t), stdx::tuple<int const &, int const &>>);
    x = 2;
    CHECK(stdx::get<0>(t) == 2);
    CHECK(stdx::get<1>(t) == 2);
}

TEST_CASE("tuple_cat (rvalue references)", "[tuple_algorithms]") {
    auto x = 1;
    auto y = 2;
    auto t = stdx::tuple_cat(stdx::tuple<int &&>{std::move(x)},
                             stdx::tuple<int &&>{std::move(y)});
    static_assert(std::is_same_v<decltype(t), stdx::tuple<int &&, int &&>>);
    x = 2;
    CHECK(stdx::get<0>(t) == 2);
    y = 2;
    CHECK(stdx::get<1>(t) == 2);
}

TEST_CASE("fold_left", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(t.fold_left(0, std::minus{}) == (((0 - 1) - 2) - 3));
    static_assert(stdx::tuple{move_only{1}, move_only{2}, move_only{3}}
                      .fold_left(move_only{0},
                                 [](move_only &&x, move_only &&y) {
                                     return move_only{x.value + y.value};
                                 })
                      .value == 6);

    int calls{};
    auto stateful = [&](auto x, auto y) mutable {
        ++calls;
        return x + y;
    };
    CHECK(t.fold_left(0, stateful) == 6);
    CHECK(calls == 3);
}

TEST_CASE("fold_left (heterogeneous types in fold)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(t.fold_left(stdx::tuple{}, [](auto acc, auto n) {
        return stdx::tuple_cat(acc, stdx::tuple{n});
    }) == t);

    static_assert(
        stdx::tuple{1, 2, 3}.fold_left(stdx::tuple{}, [](auto acc, auto n) {
            return stdx::tuple_cat(acc, stdx::tuple{n});
        }) == t);
}

template <auto N> struct addend {
    constexpr friend auto operator==(addend, addend) -> bool { return true; }
};
template <auto X, auto Y> constexpr auto operator+(addend<X>, addend<Y>) {
    return addend<X + Y>{};
}

TEST_CASE("fold_left (heterogeneous types in tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{addend<1>{}, addend<2>{}};
    static_assert(t.fold_left(addend<0>{}, std::plus{}) == addend<3>{});
}

TEST_CASE("fold_right", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(t.fold_right(4, std::minus{}) == (1 - (2 - (3 - 4))));
    static_assert(stdx::tuple{move_only{1}, move_only{2}, move_only{3}}
                      .fold_right(move_only{0},
                                  [](move_only &&x, move_only &&y) {
                                      return move_only{x.value + y.value};
                                  })
                      .value == 6);

    int calls{};
    auto stateful = [&](auto x, auto y) mutable {
        ++calls;
        return x + y;
    };
    CHECK(t.fold_right(0, stateful) == 6);
    CHECK(calls == 3);
}

TEST_CASE("fold_right (heterogeneous types in fold)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(t.fold_right(stdx::tuple{}, [](auto n, auto acc) {
        return stdx::tuple_cat(stdx::tuple{n}, acc);
    }) == t);

    static_assert(
        stdx::tuple{1, 2, 3}.fold_right(stdx::tuple{}, [](auto n, auto acc) {
            return stdx::tuple_cat(stdx::tuple{n}, acc);
        }) == t);
}

TEST_CASE("fold_right (heterogeneous types in tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{addend<1>{}, addend<2>{}};
    static_assert(t.fold_right(addend<0>{}, std::plus{}) == addend<3>{});
}

template <typename T> struct is_even {
    constexpr static auto value = T::value % 2 == 0;
};

TEST_CASE("filter", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{
        std::integral_constant<int, 1>{}, std::integral_constant<int, 2>{},
        std::integral_constant<int, 3>{}, std::integral_constant<int, 4>{}};
    constexpr auto u = stdx::filter<is_even>(t);
    static_assert(u == stdx::tuple{std::integral_constant<int, 2>{},
                                   std::integral_constant<int, 4>{}});
}

TEST_CASE("copy/move behavior for tuple_cat", "[tuple_algorithms]") {
    auto t1 = stdx::tuple{counter{}};
    auto t2 = stdx::tuple{counter{}};

    counter::reset();
    [[maybe_unused]] auto t3 = stdx::tuple_cat(t1, t2);
    CHECK(counter::moves == 0);
    CHECK(counter::copies == 2);

    [[maybe_unused]] auto t4 = stdx::tuple_cat(std::move(t1), std::move(t2));
    CHECK(counter::moves == 2);
    CHECK(counter::copies == 2);
}

template <typename> using always_true = std::true_type;

TEST_CASE("copy/move behavior for filter", "[tuple_algorithms]") {
    auto t1 = stdx::tuple{counter{}};

    counter::reset();
    [[maybe_unused]] auto t2 = stdx::filter<always_true>(t1);
    CHECK(counter::moves == 0);
    CHECK(counter::copies == 1);

    [[maybe_unused]] auto t3 = stdx::filter<always_true>(std::move(t1));
    CHECK(counter::moves == 1);
    CHECK(counter::copies == 1);
}

TEST_CASE("all_of", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(stdx::all_of([](auto n) { return n > 0; }, t));
    static_assert(
        stdx::all_of([](auto x, auto y) { return (x + y) % 2 == 0; }, t, t));
}

TEST_CASE("any_of", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3};
    static_assert(stdx::any_of([](auto n) { return n % 2 == 0; }, t));
    static_assert(
        stdx::any_of([](auto x, auto y) { return (x + y) % 2 == 0; }, t, t));
}

TEST_CASE("none_of", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 3, 5};
    static_assert(stdx::none_of([](auto n) { return n % 2 == 0; }, t));
    static_assert(
        stdx::none_of([](auto x, auto y) { return (x + y) % 2 != 0; }, t, t));
}

TEST_CASE("contains_type", "[tuple_algorithms]") {
    using T = stdx::tuple<int, bool, int &>;
    static_assert(stdx::contains_type<T, int>);
    static_assert(stdx::contains_type<T, bool>);
    static_assert(stdx::contains_type<T, int &>);
    static_assert(not stdx::contains_type<T, float>);
}

TEST_CASE("contains_type (indexed)", "[tuple_algorithms]") {
    struct X;
    struct Y;
    using T = stdx::indexed_tuple<stdx::detail::index_function_list<key_for>,
                                  map_entry<X, int>, map_entry<Y, int>>;

    static_assert(stdx::contains_type<T, map_entry<X, int>>);
    static_assert(stdx::contains_type<T, X>);
    static_assert(not stdx::contains_type<T, float>);
}

TEST_CASE("sort (empty tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{};
    [[maybe_unused]] constexpr auto sorted = stdx::sort(t);
    static_assert(std::is_same_v<decltype(sorted), stdx::tuple<> const>);
}

TEST_CASE("sort", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 1.0, true};
    static_assert(
        std::is_same_v<decltype(t), stdx::tuple<int, double, bool> const>);
    constexpr auto sorted = stdx::sort(t);
    static_assert(
        std::is_same_v<decltype(sorted), stdx::tuple<bool, double, int> const>);
    CHECK(sorted == stdx::tuple{true, 1.0, 1});
}

TEST_CASE("sort preserves references", "[tuple_algorithms]") {
    int x{1};
    double d{2.0};
    auto t = stdx::forward_as_tuple(x, d);
    static_assert(std::is_same_v<decltype(t), stdx::tuple<int &, double &>>);
    auto sorted = stdx::sort(t);
    static_assert(
        std::is_same_v<decltype(sorted), stdx::tuple<double &, int &>>);
    CHECK(sorted == stdx::tuple{2.0, 1});
}

TEST_CASE("sort with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{move_only{1}, 1.0};
    static_assert(std::is_same_v<decltype(t), stdx::tuple<move_only, double>>);
    auto sorted = stdx::sort(std::move(t));
    static_assert(
        std::is_same_v<decltype(sorted), stdx::tuple<double, move_only>>);
    CHECK(sorted == stdx::tuple{1, move_only{1}});
}

TEST_CASE("chunk (empty tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{};
    [[maybe_unused]] constexpr auto chunked = stdx::chunk(t);
    static_assert(std::is_same_v<decltype(chunked), stdx::tuple<> const>);
}

TEST_CASE("chunk (1-element tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1};
    constexpr auto chunked = stdx::chunk(t);
    static_assert(
        std::is_same_v<decltype(chunked), stdx::tuple<stdx::tuple<int>> const>);
    CHECK(chunked == stdx::make_tuple(stdx::tuple{1}));
}

TEST_CASE("count chunks", "[tuple_algorithms]") {
    static_assert(stdx::detail::count_chunks<stdx::tuple<int, int>>() == 1);
    static_assert(stdx::detail::count_chunks<stdx::tuple<int, float>>() == 2);
    static_assert(stdx::detail::count_chunks<stdx::tuple<int, int, float>>() ==
                  2);
    static_assert(
        stdx::detail::count_chunks<stdx::tuple<int, float, float>>() == 2);
}

TEST_CASE("create chunks", "[tuple_algorithms]") {
    static_assert(stdx::detail::create_chunks<stdx::tuple<int, int>>() ==
                  std::array{stdx::detail::chunk{0, 2}});
    static_assert(
        stdx::detail::create_chunks<stdx::tuple<int, int, float>>() ==
        std::array{stdx::detail::chunk{0, 2}, stdx::detail::chunk{2, 1}});
    static_assert(
        stdx::detail::create_chunks<
            stdx::tuple<int, int, float, int, int, float>>() ==
        std::array{stdx::detail::chunk{0, 2}, stdx::detail::chunk{2, 1},
                   stdx::detail::chunk{3, 2}, stdx::detail::chunk{5, 1}});
}

TEST_CASE("chunk (general case)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 2, 3, 1.0, 2.0, true};
    constexpr auto chunked = stdx::chunk(t);
    static_assert(
        std::is_same_v<
            decltype(chunked),
            stdx::tuple<stdx::tuple<int, int, int>, stdx::tuple<double, double>,
                        stdx::tuple<bool>> const>);
    CHECK(chunked == stdx::tuple{stdx::tuple{1, 2, 3}, stdx::tuple{1.0, 2.0},
                                 stdx::tuple{true}});
}

TEST_CASE("chunk preserves references", "[tuple_algorithms]") {
    int x{1};
    int y{2};
    auto t = stdx::tuple<int &, int &>{x, y};
    auto chunked = stdx::chunk(t);
    static_assert(std::is_same_v<decltype(chunked),
                                 stdx::tuple<stdx::tuple<int &, int &>>>);
    CHECK(get<0>(chunked) == stdx::tuple{1, 2});
}

TEST_CASE("chunk with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{move_only{1}};
    auto chunked = stdx::chunk(std::move(t));
    static_assert(
        std::is_same_v<decltype(chunked), stdx::tuple<stdx::tuple<move_only>>>);
    CHECK(get<0>(chunked) == stdx::tuple{move_only{1}});
}

TEST_CASE("cartesian product (no tuples)", "[tuple_algorithms]") {
    constexpr auto c = stdx::cartesian_product_copy();
    static_assert(
        std::is_same_v<decltype(c), stdx::tuple<stdx::tuple<>> const>);
}

TEST_CASE("cartesian product (one tuple)", "[tuple_algorithms]") {
    using namespace stdx::literals;
    constexpr auto c = stdx::cartesian_product_copy(stdx::tuple{1, 2, 3});
    static_assert(std::is_same_v<decltype(c),
                                 stdx::tuple<stdx::tuple<int>, stdx::tuple<int>,
                                             stdx::tuple<int>> const>);
    static_assert(c[0_idx][0_idx] == 1);
    static_assert(c[1_idx][0_idx] == 2);
    static_assert(c[2_idx][0_idx] == 3);
}

TEST_CASE("cartesian product (two tuples)", "[tuple_algorithms]") {
    using namespace stdx::literals;
    constexpr auto c =
        stdx::cartesian_product_copy(stdx::tuple{1}, stdx::tuple{2});
    static_assert(
        std::is_same_v<decltype(c), stdx::tuple<stdx::tuple<int, int>> const>);
    static_assert(c[0_idx][0_idx] == 1);
    static_assert(c[0_idx][1_idx] == 2);
}

TEST_CASE("cartesian product (general case)", "[tuple_algorithms]") {
    using namespace stdx::literals;
    constexpr auto c = stdx::cartesian_product_copy(
        stdx::tuple{1, 2}, stdx::tuple{'a', 'b'}, stdx::tuple{1.1f, 2.2f});
    static_assert(
        std::is_same_v<
            decltype(c),
            stdx::tuple<
                stdx::tuple<int, char, float>, stdx::tuple<int, char, float>,
                stdx::tuple<int, char, float>, stdx::tuple<int, char, float>,
                stdx::tuple<int, char, float>, stdx::tuple<int, char, float>,
                stdx::tuple<int, char, float>,
                stdx::tuple<int, char, float>> const>);
    static_assert(c[0_idx] == stdx::tuple{1, 'a', 1.1f});
    static_assert(c[1_idx] == stdx::tuple{1, 'a', 2.2f});
    static_assert(c[2_idx] == stdx::tuple{1, 'b', 1.1f});
    static_assert(c[3_idx] == stdx::tuple{1, 'b', 2.2f});
    static_assert(c[4_idx] == stdx::tuple{2, 'a', 1.1f});
    static_assert(c[5_idx] == stdx::tuple{2, 'a', 2.2f});
    static_assert(c[6_idx] == stdx::tuple{2, 'b', 1.1f});
    static_assert(c[7_idx] == stdx::tuple{2, 'b', 2.2f});
}

TEST_CASE("cartesian product of references", "[tuple_algorithms]") {
    using namespace stdx::literals;
    constexpr static auto t1 = stdx::tuple{1};
    constexpr static auto t2 = stdx::tuple{2};

    constexpr auto c = stdx::cartesian_product(t1, t2);
    static_assert(std::is_same_v<
                  decltype(c),
                  stdx::tuple<stdx::tuple<int const &, int const &>> const>);
    static_assert(c[0_idx][0_idx] == 1);
    static_assert(c[0_idx][1_idx] == 2);
}

TEST_CASE("unique", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 1.0, 2.0, true, false};
    static_assert(
        std::is_same_v<decltype(t),
                       stdx::tuple<int, double, double, bool, bool> const>);
    constexpr auto u = stdx::unique(t);
    static_assert(
        std::is_same_v<decltype(u), stdx::tuple<int, double, bool> const>);
    CHECK(u == stdx::tuple{1, 1.0, true});
}

TEST_CASE("unique preserves references", "[tuple_algorithms]") {
    int x{1};
    int y{2};
    auto t = stdx::forward_as_tuple(x, y);
    static_assert(std::is_same_v<decltype(t), stdx::tuple<int &, int &>>);
    auto u = stdx::unique(t);
    static_assert(std::is_same_v<decltype(u), stdx::tuple<int &>>);
    CHECK(u == stdx::tuple{1});
}

TEST_CASE("unique with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{move_only{1}, move_only{2}};
    static_assert(
        std::is_same_v<decltype(t), stdx::tuple<move_only, move_only>>);
    auto u = stdx::unique(std::move(t));
    static_assert(std::is_same_v<decltype(u), stdx::tuple<move_only>>);
    CHECK(u == stdx::tuple{move_only{1}});
}

TEST_CASE("to_sorted_set", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 1.0, true, 2.0, false};
    constexpr auto u = stdx::to_sorted_set(t);
    static_assert(
        std::is_same_v<decltype(u), stdx::tuple<bool, double, int> const>);
    CHECK(u == stdx::tuple{true, 1.0, 1});
}

TEST_CASE("to_sorted_set with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{1, move_only{1}, true, move_only{2}, false};
    auto u = stdx::to_sorted_set(std::move(t));
    static_assert(
        std::is_same_v<decltype(u), stdx::tuple<bool, int, move_only>>);
    CHECK(u == stdx::tuple{true, 1, move_only{1}});
}

TEST_CASE("to_sorted_set preserves references", "[tuple_algorithms]") {
    int x{1};
    int y{2};
    double a{3.0};
    double b{4.0};
    auto t = stdx::forward_as_tuple(x, y, a, b);
    static_assert(
        std::is_same_v<decltype(t),
                       stdx::tuple<int &, int &, double &, double &>>);
    auto u = stdx::to_sorted_set(t);
    static_assert(std::is_same_v<decltype(u), stdx::tuple<double &, int &>>);
    CHECK(u == stdx::tuple{3.0, 1});
}

TEST_CASE("to_unsorted_set", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 1.0, true, 2.0, false};
    constexpr auto u = stdx::to_unsorted_set(t);
    static_assert(
        std::is_same_v<decltype(u), stdx::tuple<int, double, bool> const>);
    CHECK(u == stdx::tuple{1, 1.0, true});
}

TEST_CASE("to_unsorted_set preserves references", "[tuple_algorithms]") {
    int x{1};
    int y{2};
    double a{3.0};
    double b{4.0};
    auto t = stdx::forward_as_tuple(x, y, a, b);
    static_assert(
        std::is_same_v<decltype(t),
                       stdx::tuple<int &, int &, double &, double &>>);
    auto u = stdx::to_unsorted_set(t);
    static_assert(std::is_same_v<decltype(u), stdx::tuple<int &, double &>>);
    CHECK(u == stdx::tuple{1, 3.0});
}

TEST_CASE("to_unsorted_set with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{1, move_only{1}, true, move_only{2}, false};
    auto u = stdx::to_unsorted_set(std::move(t));
    static_assert(
        std::is_same_v<decltype(u), stdx::tuple<int, move_only, bool>>);
    CHECK(u == stdx::tuple{1, move_only{1}, true});
}

TEST_CASE("enumerate", "[tuple_algorithms]") {
    auto const t = stdx::tuple{1, 2, 3};
    auto sum = 0;
    stdx::enumerate(
        [&]<auto Idx>(auto x, auto y) { sum += static_cast<int>(Idx) + x + y; },
        t, t);
    CHECK(sum == (0 + 1 + 2) + (2 + 4 + 6));
}

TEST_CASE("unrolled enumerate on arrays", "[tuple_algorithms]") {
    auto const a = std::array{1, 2, 3};
    auto sum = 0;
    stdx::unrolled_enumerate(
        [&]<auto Idx>(auto x, auto y) { sum += static_cast<int>(Idx) + x + y; },
        a, a);
    CHECK(sum == (0 + 1 + 2) + (2 + 4 + 6));
}

TEST_CASE("gather (empty tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{};
    [[maybe_unused]] constexpr auto gathered = stdx::gather(t);
    static_assert(std::is_same_v<decltype(gathered), stdx::tuple<> const>);
}

TEST_CASE("gather (1-element tuple)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1};
    constexpr auto gathered = stdx::gather(t);
    static_assert(std::is_same_v<decltype(gathered),
                                 stdx::tuple<stdx::tuple<int>> const>);
    CHECK(gathered == stdx::make_tuple(stdx::tuple{1}));
}

TEST_CASE("gather (general case)", "[tuple_algorithms]") {
    constexpr auto t = stdx::tuple{1, 1.0, 2, 1.0, 3, true};
    constexpr auto gathered = stdx::gather(t);
    static_assert(std::is_same_v<
                  decltype(gathered),
                  stdx::tuple<stdx::tuple<bool>, stdx::tuple<double, double>,
                              stdx::tuple<int, int, int>> const>);
    // NB: the subtuples are not necessarily ordered the same way as originally
    CHECK(stdx::get<0>(gathered) == stdx::tuple{true});
    CHECK(stdx::get<1>(gathered).fold_left(0.0, std::plus{}) == 2.0);
    CHECK(stdx::get<2>(gathered).fold_left(0, std::plus{}) == 6);
}

TEST_CASE("gather preserves references", "[tuple_algorithms]") {
    int x{1};
    int y{2};
    auto t = stdx::tuple<int &, int &>{x, y};
    auto gathered = stdx::gather(t);
    static_assert(std::is_same_v<decltype(gathered),
                                 stdx::tuple<stdx::tuple<int &, int &>>>);
    CHECK(get<0>(gathered) == stdx::tuple{1, 2});
}

TEST_CASE("gather with move only types", "[tuple_algorithms]") {
    auto t = stdx::tuple{move_only{1}};
    auto gathered = stdx::gather(std::move(t));
    static_assert(std::is_same_v<decltype(gathered),
                                 stdx::tuple<stdx::tuple<move_only>>>);
    CHECK(get<0>(gathered) == stdx::tuple{move_only{1}});
}

namespace {
template <typename T> struct named_int {
    using name_t = T;
    int value;
    friend constexpr auto operator==(named_int, named_int) -> bool = default;
};

template <typename T> using name_of_t = typename T::name_t;
} // namespace

TEST_CASE("gather_by with projection", "[tuple_algorithms]") {
    struct A;
    struct B;
    struct C;
    constexpr auto t = stdx::tuple{named_int<C>{3}, named_int<B>{11},
                                   named_int<A>{0}, named_int<B>{12}};
    constexpr auto gathered = stdx::gather_by<name_of_t>(t);
    static_assert(
        std::is_same_v<decltype(gathered),
                       stdx::tuple<stdx::tuple<named_int<A>>,
                                   stdx::tuple<named_int<B>, named_int<B>>,
                                   stdx::tuple<named_int<C>>> const>);
    CHECK(get<0>(gathered) == stdx::tuple{named_int<A>{0}});
    CHECK(stdx::get<1>(gathered).fold_left(
              0, [](auto x, auto y) { return x + y.value; }) == 23);
    CHECK(get<2>(gathered) == stdx::tuple{named_int<C>{3}});
}

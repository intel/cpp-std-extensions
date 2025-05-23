#include <stdx/tuple.hpp>
#include <stdx/tuple_destructure.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <type_traits>

namespace {
template <typename Key, typename Value> struct map_entry {
    using key_t = Key;
    using value_t = Value;

    value_t value;
};
template <typename T> using key_for = typename T::key_t;
} // namespace

TEST_CASE("make_indexed_tuple", "[indexed_tuple]") {
    STATIC_REQUIRE(stdx::make_indexed_tuple<>() == stdx::indexed_tuple{});
    STATIC_REQUIRE(stdx::make_indexed_tuple<>(1, 2, 3) ==
                   stdx::indexed_tuple{1, 2, 3});
}

TEST_CASE("comparable with regular tuple", "[indexed_tuple]") {
    STATIC_REQUIRE(stdx::indexed_tuple{} == stdx::tuple{});
}

TEST_CASE("indexed_tuple destructuring", "[indexed_tuple]") {
    auto const t = stdx::make_indexed_tuple<>(1, 3.14f);
    auto const [i, f] = t;
    CHECK(i == 1);
    CHECK(f == 3.14f);
}

TEST_CASE("tuple with user index", "[indexed_tuple]") {
    struct X;
    struct Y;
    constexpr auto t = stdx::make_indexed_tuple<key_for>(map_entry<X, int>{42},
                                                         map_entry<Y, int>{17});
    STATIC_REQUIRE(stdx::get<X>(t).value == 42);
    STATIC_REQUIRE(stdx::get<Y>(t).value == 17);
    using T = std::remove_const_t<decltype(t)>;
    STATIC_REQUIRE(
        std::is_same_v<
            T, stdx::indexed_tuple<stdx::detail::index_function_list<key_for>,
                                   map_entry<X, int>, map_entry<Y, int>>>);
    STATIC_REQUIRE(stdx::tuple_size_v<T> == 2);
    STATIC_REQUIRE(T::size() == 2);
}

TEST_CASE("indexed_tuple ADL get", "[indexed_tuple]") {
    struct X;
    struct Y;
    constexpr auto t = stdx::make_indexed_tuple<key_for>(map_entry<X, int>{42},
                                                         map_entry<Y, int>{17});
    STATIC_REQUIRE(get<X>(t).value == 42);
    STATIC_REQUIRE(get<Y>(t).value == 17);
}

namespace {
template <typename Key1, typename Key2, typename Value> struct multimap_entry {
    using key1_t = Key1;
    using key2_t = Key2;
    using value_t = Value;

    value_t value;
};

template <typename T> using key1_for = typename T::key1_t;
template <typename T> using key2_for = typename T::key2_t;
} // namespace

TEST_CASE("tuple with multiple user indices", "[indexed_tuple]") {
    struct M;
    struct N;
    struct X;
    struct Y;
    constexpr auto t = stdx::make_indexed_tuple<key1_for, key2_for>(
        multimap_entry<M, X, int>{42}, multimap_entry<N, Y, int>{17});
    STATIC_REQUIRE(stdx::get<M>(t).value == 42);
    STATIC_REQUIRE(stdx::get<X>(t).value == 42);
    STATIC_REQUIRE(stdx::get<N>(t).value == 17);
    STATIC_REQUIRE(stdx::get<Y>(t).value == 17);
}

TEST_CASE("apply indices", "[indexed_tuple]") {
    struct X;
    constexpr auto t = stdx::tuple{map_entry<X, int>{42}};
    constexpr auto u = stdx::apply_indices<key_for>(t);
    STATIC_REQUIRE(stdx::get<X>(u).value == 42);
}

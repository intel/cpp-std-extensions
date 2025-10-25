#include <stdx/cx_map.hpp>
#include <stdx/cx_multimap.hpp>
#include <stdx/cx_queue.hpp>
#include <stdx/cx_set.hpp>
#include <stdx/cx_vector.hpp>
#include <stdx/iterator.hpp>
#include <stdx/span.hpp>

#include <array>
#include <type_traits>

#if __has_include(<span>)
#include <span>
#endif

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time capacity (std::array)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    STATIC_REQUIRE(stdx::ct_capacity(a) == 4u);
}

#if __cpp_lib_span >= 202002L
TEST_CASE("compile-time capacity (std::span)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    std::span s{a};
    STATIC_REQUIRE(stdx::ct_capacity(s) == 4u);
}
#endif

TEST_CASE("compile-time capacity (stdx::span)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    stdx::span s{a};
    STATIC_REQUIRE(stdx::ct_capacity(s) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_map)", "[iterator]") {
    stdx::cx_map<int, int, 4> m{};
    STATIC_REQUIRE(stdx::ct_capacity(m) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_multimap)", "[iterator]") {
    stdx::cx_multimap<int, int, 4, 5> m{};
    STATIC_REQUIRE(stdx::ct_capacity(m) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_queue)", "[iterator]") {
    stdx::cx_queue<int, 4> q{};
    STATIC_REQUIRE(stdx::ct_capacity(q) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_set)", "[iterator]") {
    stdx::cx_set<int, 4> s{};
    STATIC_REQUIRE(stdx::ct_capacity(s) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_vector)", "[iterator]") {
    stdx::cx_vector<int, 4> v{};
    STATIC_REQUIRE(stdx::ct_capacity(v) == 4u);
}

TEST_CASE("compile-time capacity variable template", "[iterator]") {
    std::array a{1, 2, 3, 4};
    STATIC_REQUIRE(stdx::ct_capacity_v<decltype(a)> == 4u);
}

TEST_CASE("compile-time capacity variable template (const)", "[iterator]") {
    std::array const a{1, 2, 3, 4};
    STATIC_REQUIRE(stdx::ct_capacity_v<decltype(a)> == 4u);
}

TEST_CASE("default counting_iterator traits", "[iterator]") {
    using T = std::iterator_traits<stdx::counting_iterator<>>;
    STATIC_REQUIRE(std::is_same_v<typename T::difference_type, int>);
    STATIC_REQUIRE(std::is_same_v<typename T::value_type, int>);
    STATIC_REQUIRE(std::is_same_v<typename T::pointer, int *>);
    STATIC_REQUIRE(std::is_same_v<typename T::reference, int &>);

#if __cplusplus >= 202002L
    STATIC_REQUIRE(std::is_same_v<typename T::iterator_category,
                                  std::contiguous_iterator_tag>);
#else
    STATIC_REQUIRE(std::is_same_v<typename T::iterator_category,
                                  std::random_access_iterator_tag>);
#endif
}

TEST_CASE("default counting_iterator value is 0, increment is 1",
          "[iterator]") {
    auto i = stdx::counting_iterator{};
    CHECK(*i == 0);
    ++i;
    CHECK(*i == 1);
}

TEST_CASE("counting_iterator is an input iterator", "[iterator]") {
    auto i = stdx::counting_iterator{};
    auto v = *i;
    CHECK(v == 0);
    ++i;
    CHECK(*i != v);
}

TEST_CASE("counting_iterator is a forward iterator", "[iterator]") {
    auto i = stdx::counting_iterator{};
    auto v = *i;
    auto j = i++;
    CHECK(*j == v);
    CHECK(*i - v == 1);
    ++j;
    CHECK(*i == *j);
}

TEST_CASE("counting_iterator is a bidi iterator", "[iterator]") {
    auto i = stdx::counting_iterator{};
    auto v = *i;
    auto j = i--;
    CHECK(*j == v);
    CHECK(v - *i == 1);
    --j;
    CHECK(*i == *j);
}

TEST_CASE("counting_iterator is a random access iterator", "[iterator]") {
    auto i = stdx::counting_iterator{};
    CHECK(*(i + 1) - *i == 1);
    CHECK(*i - *(i - 1) == 1);

    i += 1;
    CHECK(*i == 1);
    CHECK(i - stdx::counting_iterator{} == 1);
    i -= 1;
    CHECK(*i == 0);
}

TEST_CASE("counting_iterator equality", "[iterator]") {
    auto i = stdx::counting_iterator{};
    CHECK(i == stdx::counting_iterator{});
}

TEST_CASE("counting_iterator comparison", "[iterator]") {
    auto i = stdx::counting_iterator{};
    auto j = i++;
    CHECK(j < i);
    CHECK(j <= i);
    CHECK(i > j);
    CHECK(i >= j);
}

#if __cpp_impl_three_way_comparison >= 201907L
TEST_CASE("counting_iterator spaceship comparison", "[iterator]") {
    auto i = stdx::counting_iterator{};
    auto j = i++;
    CHECK(i <=> i == std::strong_ordering::equal);
    CHECK(j <=> i == std::strong_ordering::less);
}
#endif

TEST_CASE("counting_iterator can be given a starting value", "[iterator]") {
    auto i = stdx::counting_iterator{17};
    CHECK(*i == 17);
    ++i;
    CHECK(*i == 18);
}

TEST_CASE("counting_iterator can be given a different type", "[iterator]") {
    auto i = stdx::counting_iterator{'A'};
    CHECK(*i == 'A');
    ++i;
    CHECK(*i == 'B');

    using T = std::iterator_traits<decltype(i)>;
    STATIC_REQUIRE(std::is_same_v<typename T::difference_type, int>);
    STATIC_REQUIRE(std::is_same_v<typename T::value_type, char>);
    STATIC_REQUIRE(std::is_same_v<typename T::pointer, char *>);
    STATIC_REQUIRE(std::is_same_v<typename T::reference, char &>);
}

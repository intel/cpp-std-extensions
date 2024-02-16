#include <stdx/cx_map.hpp>
#include <stdx/cx_multimap.hpp>
#include <stdx/cx_queue.hpp>
#include <stdx/cx_set.hpp>
#include <stdx/cx_vector.hpp>
#include <stdx/iterator.hpp>
#include <stdx/span.hpp>

#include <array>

#if __has_include(<span>)
#include <span>
#endif

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time capacity (std::array)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    static_assert(stdx::ct_capacity(a) == 4u);
}

#if __cpp_lib_span >= 202002L
TEST_CASE("compile-time capacity (std::span)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    std::span s{a};
    static_assert(stdx::ct_capacity(s) == 4u);
}
#endif

TEST_CASE("compile-time capacity (stdx::span)", "[iterator]") {
    std::array a{1, 2, 3, 4};
    stdx::span s{a};
    static_assert(stdx::ct_capacity(s) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_map)", "[iterator]") {
    stdx::cx_map<int, int, 4> m{};
    static_assert(stdx::ct_capacity(m) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_multimap)", "[iterator]") {
    stdx::cx_multimap<int, int, 4, 5> m{};
    static_assert(stdx::ct_capacity(m) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_queue)", "[iterator]") {
    stdx::cx_queue<int, 4> q{};
    static_assert(stdx::ct_capacity(q) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_set)", "[iterator]") {
    stdx::cx_set<int, 4> s{};
    static_assert(stdx::ct_capacity(s) == 4u);
}

TEST_CASE("compile-time capacity (stdx::cx_vector)", "[iterator]") {
    stdx::cx_vector<int, 4> v{};
    static_assert(stdx::ct_capacity(v) == 4u);
}

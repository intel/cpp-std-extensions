#include <stdx/cx_multimap.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("empty and size", "[cx_multimap]") {
    stdx::cx_multimap<int, int, 64> t;

    CHECK(t.size() == 0);
    CHECK(t.empty());
}

TEST_CASE("put and contains", "[cx_multimap]") {
    stdx::cx_multimap<int, int, 64> t;

    t.put(60, 40);

    CHECK(t.size() == 1);
    CHECK(not t.empty());
    CHECK(t.contains(60));
    CHECK(not t.contains(40));
    CHECK(t.contains(60, 40));
    CHECK(not t.contains(60, 60));
    CHECK(not t.contains(40, 40));
}

TEST_CASE("constexpr empty", "[cx_multimap]") {
    constexpr auto t = stdx::cx_multimap<int, int, 64>{};

    STATIC_REQUIRE(t.empty());
    STATIC_REQUIRE(not t.contains(10));
    STATIC_REQUIRE(not t.contains(10, 10));
}

TEST_CASE("put multiple values", "[cx_multimap]") {
    stdx::cx_multimap<int, int, 64> t;

    t.put(60, 1);
    t.put(60, 2);
    t.put(60, 3);

    CHECK(t.size() == 1);
    CHECK(t.contains(60, 1));
    CHECK(t.contains(60, 2));
    CHECK(t.contains(60, 3));
    CHECK(not t.contains(60, 0));
}

TEST_CASE("erase values", "[cx_multimap]") {
    stdx::cx_multimap<int, int, 64> t;

    t.put(60, 1);
    t.put(60, 2);
    t.put(61, 3);
    t.put(62, 4);
    CHECK(t.size() == 3);

    CHECK(t.erase(60, 1));
    CHECK(t.size() == 3);
    CHECK(not t.contains(60, 1));
    CHECK(t.contains(60, 2));

    CHECK(t.erase(60, 2));
    CHECK(t.size() == 2);
    CHECK(not t.contains(60, 2));
    CHECK(t.contains(61, 3));
    CHECK(t.contains(62, 4));

    t.erase(61);
    CHECK(t.size() == 1);
    CHECK(t.contains(62, 4));
    t.erase(62);
    CHECK(t.empty());
}

TEST_CASE("constexpr populated map", "[cx_multimap]") {
    constexpr auto m = [] {
        stdx::cx_multimap<int, int, 64> t;
        t.put(10, 100);
        t.put(10, 101);
        t.put(10, 110);
        t.put(50, 1);
        return t;
    }();

    STATIC_REQUIRE(not m.empty());
    STATIC_REQUIRE(m.size() == 2);
    STATIC_REQUIRE(m.contains(10));
    STATIC_REQUIRE(not m.get(10).empty());
    STATIC_REQUIRE(m.get(10).size() == 3);
    STATIC_REQUIRE(m.contains(10, 100));
    STATIC_REQUIRE(m.contains(10, 101));
    STATIC_REQUIRE(m.contains(10, 110));
    STATIC_REQUIRE(not m.contains(10, 50));
    STATIC_REQUIRE(not m.get(50).empty());
    STATIC_REQUIRE(m.get(50).size() == 1);
    STATIC_REQUIRE(m.contains(50, 1));
    STATIC_REQUIRE(not m.contains(50, 2));
}

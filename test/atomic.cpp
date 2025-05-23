#include <stdx/atomic.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <type_traits>

TEMPLATE_TEST_CASE("atomic size and alignment is the same as the data",
                   "[atomic]", bool, char, signed char, unsigned char,
                   short int, unsigned short int, int, unsigned int, long int,
                   unsigned long int) {
    STATIC_REQUIRE(sizeof(stdx::atomic<TestType>) == sizeof(TestType));
    STATIC_REQUIRE(alignof(stdx::atomic<TestType>) == alignof(TestType));
}

TEMPLATE_TEST_CASE("atomic is default constructible when data is", "[atomic]",
                   bool, char, signed char, unsigned char, short int,
                   unsigned short int, int, unsigned int, long int,
                   unsigned long int) {
    STATIC_REQUIRE(std::is_default_constructible_v<stdx::atomic<TestType>>);
}

namespace {
struct non_dc {
    non_dc(int) {}
};
} // namespace

#if __cplusplus >= 202002L
TEST_CASE("atomic is not default constructible when data is not", "[atomic]") {
    STATIC_REQUIRE(not std::is_default_constructible_v<stdx::atomic<non_dc>>);
}
#endif

TEST_CASE("atomic is not copyable or movable", "[atomic]") {
    STATIC_REQUIRE(not std::is_copy_constructible_v<stdx::atomic<int>>);
    STATIC_REQUIRE(not std::is_move_constructible_v<stdx::atomic<int>>);
    STATIC_REQUIRE(not std::is_copy_assignable_v<stdx::atomic<int>>);
    STATIC_REQUIRE(not std::is_move_assignable_v<stdx::atomic<int>>);
}

TEMPLATE_TEST_CASE("atomic supports value initialization", "[atomic]", bool,
                   char, signed char, unsigned char, short int,
                   unsigned short int, int, unsigned int, long int,
                   unsigned long int) {
    STATIC_REQUIRE(std::is_constructible_v<stdx::atomic<TestType>, TestType>);
    [[maybe_unused]] auto x = stdx::atomic<TestType>{TestType{}};
}

TEST_CASE("load", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val.load() == 17);
}

TEST_CASE("store", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    val.store(1337);
    CHECK(val.load() == 1337);
}

TEST_CASE("implicit conversion to T", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val == 17);
}

TEST_CASE("assignment from T", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    val = 1337;
    CHECK(val == 1337);
}

TEST_CASE("exchange", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val.exchange(1337) == 17);
    CHECK(val.load() == 1337);
}

TEST_CASE("fetch_add", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val.fetch_add(42) == 17);
    CHECK(val.load() == 59);
}

TEST_CASE("fetch_sub", "[atomic]") {
    stdx::atomic<std::uint32_t> val{59};
    CHECK(val.fetch_sub(42) == 59);
    CHECK(val.load() == 17);
}

TEST_CASE("operator +=", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK((val += 42) == 59);
    CHECK(val.load() == 59);
}

TEST_CASE("operator -=", "[atomic]") {
    stdx::atomic<std::uint32_t> val{59};
    CHECK((val -= 42) == 17);
    CHECK(val.load() == 17);
}

TEST_CASE("pre-increment", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(++val == 18);
    CHECK(val.load() == 18);
}

TEST_CASE("post-increment", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val++ == 17);
    CHECK(val.load() == 18);
}

TEST_CASE("pre-decrement", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(--val == 16);
    CHECK(val.load() == 16);
}

TEST_CASE("post-decrement", "[atomic]") {
    stdx::atomic<std::uint32_t> val{17};
    CHECK(val-- == 17);
    CHECK(val.load() == 16);
}

TEST_CASE("fetch_and", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b101};
    CHECK(val.fetch_and(0b100) == 0b101);
    CHECK(val.load() == 0b100);
}

TEST_CASE("fetch_or", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b1};
    CHECK(val.fetch_or(0b100) == 0b1);
    CHECK(val.load() == 0b101);
}

TEST_CASE("fetch_xor", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b101};
    CHECK(val.fetch_xor(0b1) == 0b101);
    CHECK(val.load() == 0b100);
}

TEST_CASE("operator &=", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b101};
    CHECK((val &= 0b100) == 0b100);
    CHECK(val.load() == 0b100);
}

TEST_CASE("operator |=", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b1};
    CHECK((val |= 0b100) == 0b101);
    CHECK(val.load() == 0b101);
}

TEST_CASE("operator ^=", "[atomic]") {
    stdx::atomic<std::uint32_t> val{0b101};
    CHECK((val ^= 0b1) == 0b100);
    CHECK(val.load() == 0b100);
}

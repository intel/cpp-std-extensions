#include <stdx/bit.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>

TEST_CASE("byteswap", "[bit]") {
    static_assert(stdx::byteswap(std::uint8_t{1u}) == 1u);
    static_assert(stdx::byteswap(std::uint16_t{0x0102u}) == 0x0201u);
    static_assert(stdx::byteswap(std::uint32_t{0x01020304ul}) == 0x04030201ul);
    static_assert(stdx::byteswap(std::uint64_t{0x01020304'05060708ull}) ==
                  0x08070605'04030201ull);
}

TEST_CASE("to little endian", "[bit]") {
    static_assert(stdx::to_le(std::uint8_t{1u}) == 1u);

    [[maybe_unused]] constexpr std::uint16_t u16{0x1234};
    [[maybe_unused]] constexpr std::uint32_t u32{0x1234'5678};
    [[maybe_unused]] constexpr std::uint64_t u64{0x1234'5678'9abc'def0};

    if constexpr (stdx::endian::native == stdx::endian::little) {
        CHECK(stdx::to_le(u16) == u16);
        CHECK(stdx::to_le(u32) == u32);
        CHECK(stdx::to_le(u64) == u64);
    } else if constexpr (stdx::endian::native == stdx::endian::big) {
        CHECK(stdx::to_le(u16) == stdx::byteswap(u16));
        CHECK(stdx::to_le(u32) == stdx::byteswap(u32));
        CHECK(stdx::to_le(u64) == stdx::byteswap(u64));
    }
}

TEST_CASE("to big endian", "[bit]") {
    static_assert(stdx::to_be(std::uint8_t{1u}) == 1u);

    [[maybe_unused]] constexpr std::uint16_t u16{0x1234};
    [[maybe_unused]] constexpr std::uint32_t u32{0x1234'5678};
    [[maybe_unused]] constexpr std::uint64_t u64{0x1234'5678'9abc'def0};

    if constexpr (stdx::endian::native == stdx::endian::big) {
        CHECK(stdx::to_be(u16) == u16);
        CHECK(stdx::to_be(u32) == u32);
        CHECK(stdx::to_be(u64) == u64);
    } else if constexpr (stdx::endian::native == stdx::endian::little) {
        CHECK(stdx::to_be(u16) == stdx::byteswap(u16));
        CHECK(stdx::to_be(u32) == stdx::byteswap(u32));
        CHECK(stdx::to_be(u64) == stdx::byteswap(u64));
    }
}

TEMPLATE_TEST_CASE("popcount", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::popcount(TestType{}) == 0);

    constexpr TestType x = 0b10101;
    static_assert(stdx::popcount(x) == 3);

    constexpr TestType max = std::numeric_limits<TestType>::max();
    static_assert(stdx::popcount(max) == std::numeric_limits<TestType>::digits);
}

TEMPLATE_TEST_CASE("has_single_bit", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(not stdx::has_single_bit(TestType{}));
    static_assert(stdx::has_single_bit(TestType{1u}));

    constexpr TestType x = 0b10101;
    static_assert(not stdx::has_single_bit(x));
}

TEMPLATE_TEST_CASE("countl_zero", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    static_assert(stdx::countl_zero(TestType{}) == d);
    static_assert(stdx::countl_zero(TestType{1u}) == d - 1);
    static_assert(stdx::countl_zero(TestType{2u}) == d - 2);
}

TEMPLATE_TEST_CASE("countr_zero", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    static_assert(stdx::countr_zero(TestType{}) == d);
    static_assert(stdx::countr_zero(TestType{1u}) == 0);
    static_assert(stdx::countr_zero(TestType{2u}) == 1);
}

TEMPLATE_TEST_CASE("countl_one", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    constexpr auto max = std::numeric_limits<TestType>::max();
    static_assert(stdx::countl_one(TestType{}) == 0);
    static_assert(stdx::countl_one(TestType{1u}) == 0);
    static_assert(stdx::countl_one(max) == d);
    static_assert(stdx::countl_one(TestType(max - 1)) == d - 1);
}

TEMPLATE_TEST_CASE("countr_one", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    constexpr auto max = std::numeric_limits<TestType>::max();
    static_assert(stdx::countr_one(TestType{}) == 0);
    static_assert(stdx::countr_one(TestType{1u}) == 1);
    static_assert(stdx::countr_one(max) == d);
    static_assert(stdx::countr_one(TestType(max - 1)) == 0);
}

TEMPLATE_TEST_CASE("bit_width", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::bit_width(TestType{}) == 0);
    static_assert(stdx::bit_width(TestType{1u}) == 1);
    static_assert(stdx::bit_width(TestType{3u}) == 2);

    constexpr TestType max = std::numeric_limits<TestType>::max();
    static_assert(stdx::bit_width(TestType{max}) ==
                  std::numeric_limits<TestType>::digits);
}

TEMPLATE_TEST_CASE("bit_ceil", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::bit_ceil(TestType{}) == 1);
    static_assert(stdx::bit_ceil(TestType{1u}) == 1);
    static_assert(stdx::bit_ceil(TestType{45u}) == 64);
}

TEMPLATE_TEST_CASE("bit_floor", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::bit_floor(TestType{}) == 0);
    static_assert(stdx::bit_floor(TestType{1u}) == 1);
    static_assert(stdx::bit_floor(TestType{45u}) == 32);
}

TEST_CASE("bit_cast", "[bit]") {
    constexpr float f = 1.0f;
    constexpr auto x = stdx::bit_cast<std::uint32_t>(f);
    static_assert(x == 0x3f80'0000);
}

TEMPLATE_TEST_CASE("rotl", "[bit]", std::uint8_t, std::uint16_t, std::uint32_t,
                   std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    static_assert(stdx::rotl(TestType{1u}, 1) == TestType{2u});
    static_assert(stdx::rotl(TestType(TestType{1u} << (d - 1)), 1) ==
                  TestType{1u});
}

TEMPLATE_TEST_CASE("rotr", "[bit]", std::uint8_t, std::uint16_t, std::uint32_t,
                   std::uint64_t) {
    constexpr auto d = std::numeric_limits<TestType>::digits;
    static_assert(stdx::rotr(TestType{2u}, 1) == TestType{1u});
    static_assert(stdx::rotr(TestType{1u}, 1) == TestType{1u} << (d - 1));
}

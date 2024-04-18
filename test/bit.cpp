#include <stdx/bit.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

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

TEST_CASE("bit_pack 2x8 -> 16", "[bit]") {
    static_assert(stdx::bit_pack<std::uint16_t>(0x12, 0x34) == 0x1234);
}

TEST_CASE("bit_pack 2x16 -> 32", "[bit]") {
    static_assert(stdx::bit_pack<std::uint32_t>(0x1234, 0x5678) == 0x1234'5678);
}

TEST_CASE("bit_pack 4x8 -> 32", "[bit]") {
    static_assert(stdx::bit_pack<std::uint32_t>(0x12, 0x34, 0x56, 0x78) ==
                  0x1234'5678);
}

TEST_CASE("bit_pack 2x32 -> 64", "[bit]") {
    static_assert(stdx::bit_pack<std::uint64_t>(0x1234'5678, 0x9abc'def0) ==
                  0x1234'5678'9abc'def0);
}

TEST_CASE("bit_pack 4x16 -> 64", "[bit]") {
    static_assert(stdx::bit_pack<std::uint64_t>(
                      0x1234, 0x5678, 0x9abc, 0xdef0) == 0x1234'5678'9abc'def0);
}

TEST_CASE("bit_pack 8x8 -> 64", "[bit]") {
    static_assert(stdx::bit_pack<std::uint64_t>(0x12, 0x34, 0x56, 0x78, 0x9a,
                                                0xbc, 0xde,
                                                0xf0) == 0x1234'5678'9abc'def0);
}

TEST_CASE("template bit_mask (whole range)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint64_t>();
    static_assert(m == std::numeric_limits<std::uint64_t>::max());
    static_assert(m == stdx::bit_mask<std::uint64_t, 63>());
    static_assert(m == stdx::bit_mask<std::uint64_t, 63, 0>());
}

TEST_CASE("template bit_mask (low bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t, 1, 0>();
    static_assert(m == 0b0000'0011);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("template bit_mask (mid bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t, 4, 3>();
    static_assert(m == 0b0001'1000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("template bit_mask (high bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t, 7, 6>();
    static_assert(m == 0b1100'0000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("template bit_mask (single bit)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t, 5, 5>();
    static_assert(m == 0b0010'0000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("arg bit_mask (whole range)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint64_t>(63);
    static_assert(m == std::numeric_limits<std::uint64_t>::max());
    static_assert(m == stdx::bit_mask<std::uint64_t>(63, 0));
}

TEST_CASE("arg bit_mask (low bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t>(1, 0);
    static_assert(m == 0b0000'0011);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("arg bit_mask (mid bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t>(4, 3);
    static_assert(m == 0b0001'1000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("arg bit_mask (high bits)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t>(7, 6);
    static_assert(m == 0b1100'0000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEST_CASE("arg bit_mask (single bit)", "[bit]") {
    constexpr auto m = stdx::bit_mask<std::uint8_t>(5, 5);
    static_assert(m == 0b0010'0000);
    static_assert(std::is_same_v<decltype(m), std::uint8_t const>);
}

TEMPLATE_TEST_CASE("bit_size", "[bit]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t, std::int8_t, std::int16_t,
                   std::int32_t, std::int64_t) {
    static_assert(stdx::bit_size<TestType>() ==
                  std::numeric_limits<std::make_unsigned_t<TestType>>::digits);
}

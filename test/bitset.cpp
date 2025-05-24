#include <stdx/bitset.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

TEST_CASE("bitset storage rounds up to nearest element size", "[bitset]") {
    STATIC_REQUIRE(sizeof(stdx::bitset<1, std::uint8_t>) == 1);
    STATIC_REQUIRE(sizeof(stdx::bitset<8, std::uint8_t>) == 1);
    STATIC_REQUIRE(sizeof(stdx::bitset<9, std::uint8_t>) == 2);

    STATIC_REQUIRE(sizeof(stdx::bitset<1, std::uint16_t>) == 2);
    STATIC_REQUIRE(sizeof(stdx::bitset<16, std::uint16_t>) == 2);
    STATIC_REQUIRE(sizeof(stdx::bitset<17, std::uint16_t>) == 4);
}

TEST_CASE("bitset with implicit storage element type", "[bitset]") {
    STATIC_REQUIRE(sizeof(stdx::bitset<1>) == 1);
    STATIC_REQUIRE(sizeof(stdx::bitset<8>) == 1);
    STATIC_REQUIRE(sizeof(stdx::bitset<9>) == 2);
    STATIC_REQUIRE(sizeof(stdx::bitset<16>) == 2);
    STATIC_REQUIRE(sizeof(stdx::bitset<17>) == 4);
    STATIC_REQUIRE(sizeof(stdx::bitset<32>) == 4);
    STATIC_REQUIRE(sizeof(stdx::bitset<33>) == 8);
    STATIC_REQUIRE(sizeof(stdx::bitset<64>) == 8);
}

TEMPLATE_TEST_CASE("bitset size", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(stdx::bitset<1, TestType>{}.size() == 1);
    STATIC_REQUIRE(stdx::bitset<8, TestType>{}.size() == 8);
}

TEMPLATE_TEST_CASE("index operation", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(not stdx::bitset<1, TestType>{}[0]);
}

TEMPLATE_TEST_CASE("set single bit", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<1, TestType>{};
    CHECK(not bs[0]);
    bs.set(0);
    CHECK(bs[0]);
    bs.set(0, false);
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("set all bits", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<1, TestType>{};
    CHECK(not bs[0]);
    bs.set();
    CHECK(bs[0]);
}

TEMPLATE_TEST_CASE("reset single bit", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<1, TestType>{1ul};
    CHECK(bs[0]);
    bs.reset(0);
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("reset all bits", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<1, TestType>{1ul};
    CHECK(bs[0]);
    bs.reset();
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("flip single bit", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<3, TestType>{0b101ul};
    CHECK(bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
    bs.flip(0);
    CHECK(not bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
}

TEMPLATE_TEST_CASE("flip all bits", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::bitset<3, TestType>{0b101ul};
    CHECK(bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
    bs.flip();
    CHECK(not bs[0]);
    CHECK(bs[1]);
    CHECK(not bs[2]);
}

TEMPLATE_TEST_CASE("construct with a value", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<1, TestType>{1ul};
    STATIC_REQUIRE(bs1[0]);

    constexpr auto bs2 = stdx::bitset<3, TestType>{255ul};
    STATIC_REQUIRE(bs2[0]);
    STATIC_REQUIRE(bs2[1]);
}

TEMPLATE_TEST_CASE("construct with values for bits", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<8, TestType>{stdx::place_bits, 1, 3, 5};
    STATIC_REQUIRE(not bs[0]);
    STATIC_REQUIRE(bs[1]);
    STATIC_REQUIRE(bs[3]);
    STATIC_REQUIRE(bs[5]);
}

TEMPLATE_TEST_CASE("construct with a string_view", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::bitset<4, TestType>{"1010"sv} ==
                   stdx::bitset<4, TestType>{0b1010ul});
}

TEMPLATE_TEST_CASE("construct with a substring", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::bitset<4, TestType>{"XOXOXO"sv, 2, 4, 'X'} ==
                   stdx::bitset<4, TestType>{0b1010ul});
}

TEMPLATE_TEST_CASE("convert to unsigned integral type (same underlying type)",
                   "[bitset]", std::uint8_t, std::uint16_t, std::uint32_t,
                   std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{255ul};
    constexpr auto val = bs.template to<TestType>();
    STATIC_REQUIRE(std::is_same_v<decltype(val), TestType const>);
    STATIC_REQUIRE(val == 7u);
}

TEMPLATE_TEST_CASE(
    "convert to unsigned integral type (different underlying type)", "[bitset]",
    std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs =
        stdx::bitset<11, std::uint8_t>{stdx::place_bits, 3, 7, 10};
    constexpr auto val = bs.to<TestType>();
    STATIC_REQUIRE(std::is_same_v<decltype(val), TestType const>);
    STATIC_REQUIRE(val == 0b100'1000'1000u);
}

TEST_CASE("convert to type that fits", "[bitset]") {
    constexpr auto bs =
        stdx::bitset<11, std::uint8_t>{stdx::place_bits, 3, 7, 10};
    constexpr auto val = bs.to_natural();
    STATIC_REQUIRE(std::is_same_v<decltype(val), std::uint16_t const>);
    STATIC_REQUIRE(val == 0b100'1000'1000u);
}

TEMPLATE_TEST_CASE("all", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 =
        stdx::bitset<8, TestType>{std::numeric_limits<TestType>::max()};
    STATIC_REQUIRE(bs1.all());

    constexpr auto bs2 = stdx::bitset<9, TestType>{0x1fful};
    STATIC_REQUIRE(bs2.all());

    constexpr auto bs3 = stdx::bitset<8, TestType>{0xf7ul};
    STATIC_REQUIRE(not bs3.all());
}

TEMPLATE_TEST_CASE("any", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<8, TestType>{8ul};
    STATIC_REQUIRE(bs1.any());

    constexpr auto bs2 = stdx::bitset<9, TestType>{0xfful};
    STATIC_REQUIRE(bs2.any());

    constexpr auto bs3 = stdx::bitset<8, TestType>{};
    STATIC_REQUIRE(not bs3.any());
}

TEMPLATE_TEST_CASE("none", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<8, TestType>{};
    STATIC_REQUIRE(bs1.none());

    constexpr auto bs2 = stdx::bitset<8, TestType>{8ul};
    STATIC_REQUIRE(not bs2.none());
}

TEMPLATE_TEST_CASE("count", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<8, TestType>{};
    STATIC_REQUIRE(bs1.count() == 0u);

    constexpr auto bs2 = stdx::bitset<8, TestType>{0b10101ul};
    STATIC_REQUIRE(bs2.count() == 3u);
}

TEMPLATE_TEST_CASE("or", "[bitset]", std::uint8_t, std::uint16_t, std::uint32_t,
                   std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<3, TestType>{0b101ul};
    constexpr auto bs2 = stdx::bitset<3, TestType>{0b010ul};
    STATIC_REQUIRE((bs1 | bs2) == stdx::bitset<3, TestType>{0b111ul});
}

TEMPLATE_TEST_CASE("and", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<3, TestType>{0b101ul};
    constexpr auto bs2 = stdx::bitset<3, TestType>{0b100ul};
    STATIC_REQUIRE((bs1 & bs2) == stdx::bitset<3, TestType>{0b100ul});
}

TEMPLATE_TEST_CASE("xor", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<3, TestType>{0b101ul};
    constexpr auto bs2 = stdx::bitset<3, TestType>{0b010ul};
    STATIC_REQUIRE((bs1 ^ bs2) == stdx::bitset<3, TestType>{0b111ul});
}

TEMPLATE_TEST_CASE("not", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{0b101ul};
    STATIC_REQUIRE(~bs == stdx::bitset<3, TestType>{0b10ul});
}

TEMPLATE_TEST_CASE("difference", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::bitset<3, TestType>{0b101ul};
    constexpr auto bs2 = stdx::bitset<3, TestType>{0b011ul};
    STATIC_REQUIRE(bs1 - bs2 == stdx::bitset<3, TestType>{0b100ul});
}

TEMPLATE_TEST_CASE("left shift", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{0b101ul};
    STATIC_REQUIRE(bs << 1u == stdx::bitset<3, TestType>{0b10ul});
    STATIC_REQUIRE(bs << 2u == stdx::bitset<3, TestType>{0b100ul});
}

TEMPLATE_TEST_CASE("left shift (equal to type size)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto type_size = std::numeric_limits<TestType>::digits;
    constexpr auto sz = type_size * 3;
    auto bs = stdx::bitset<sz, TestType>{};
    bs.set();
    bs <<= type_size;
    CHECK(bs[type_size]);
    CHECK(not bs[type_size - 1]);
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("left shift (off end)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{0b101ul};
    STATIC_REQUIRE(bs << 3u == stdx::bitset<3, TestType>{});
}

TEMPLATE_TEST_CASE("left shift (large shift)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto sz = std::numeric_limits<TestType>::digits * 2;
    constexpr auto shift_amount = std::numeric_limits<TestType>::digits + 1;
    constexpr auto bs = stdx::bitset<sz, TestType>{0b101ul} << shift_amount;
    STATIC_REQUIRE(not bs[0]);
    STATIC_REQUIRE(bs[shift_amount]);
    STATIC_REQUIRE(not bs[shift_amount + 1]);
    STATIC_REQUIRE(bs[shift_amount + 2]);
}

TEMPLATE_TEST_CASE("right shift", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{0b101ul};
    STATIC_REQUIRE(bs >> 1u == stdx::bitset<3, TestType>{0b10ul});
    STATIC_REQUIRE(bs >> 2u == stdx::bitset<3, TestType>{0b1ul});
}

TEMPLATE_TEST_CASE("right shift (equal to type size)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto type_size = std::numeric_limits<TestType>::digits;
    constexpr auto sz = type_size * 3;
    auto bs = stdx::bitset<sz, TestType>{};
    bs.set();
    bs >>= type_size;
    CHECK(not bs[type_size * 2]);
    CHECK(bs[type_size * 2 - 1]);
    CHECK(bs[0]);
}

TEMPLATE_TEST_CASE("right shift (off end)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<3, TestType>{0b101ul};
    STATIC_REQUIRE(bs >> 3u == stdx::bitset<3, TestType>{});
}

TEMPLATE_TEST_CASE("right shift (large shift)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto sz = std::numeric_limits<TestType>::digits * 2;
    constexpr auto shift_amount = std::numeric_limits<TestType>::digits + 1;
    auto bs = stdx::bitset<sz, TestType>{};
    bs.set();
    bs >>= shift_amount;
    CHECK(not bs[std::numeric_limits<TestType>::digits]);
    CHECK(not bs[std::numeric_limits<TestType>::digits - 1]);
    CHECK(bs[std::numeric_limits<TestType>::digits - 2]);
    CHECK(bs[0]);
}

TEMPLATE_TEST_CASE("for_each", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<64, TestType>{0x01020304'05060708ul};
    auto result = decltype(bs){};
    for_each([&](auto i) { result.set(i); }, bs);
    CHECK(result == bs);
}

TEMPLATE_TEST_CASE("for_each iterates in order lsb to msb", "[bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<5, TestType>{0b10101ul};
    auto result = decltype(bs){};
    std::size_t expected{};
    for_each(
        [&](auto i) {
            CHECK(i == expected);
            expected += 2;
            result.set(i);
        },
        bs);
    CHECK(result == bs);
}

TEMPLATE_TEST_CASE("transform_reduce", "[bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<8, TestType>{0b10101010ul};
    int calls{};
    auto const result = transform_reduce(
        [&](auto i) {
            ++calls;
            return i == 3 and calls == 2;
        },
        std::logical_or{}, false, bs);
    CHECK(result);
    CHECK(calls == bs.count());
}

TEMPLATE_TEST_CASE("set range of bits (lsb, length)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::bitset<64, TestType>{};
    bs.set(6_lsb, 4_len);
    CHECK(bs[6]);
    CHECK(bs[7]);
    CHECK(bs[8]);
    CHECK(bs[9]);
    CHECK(bs.count() == 4u);
}

TEMPLATE_TEST_CASE("set range of bits (lsb, msb)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::bitset<64, TestType>{};
    bs.set(6_lsb, 9_msb);
    CHECK(bs[6]);
    CHECK(bs[7]);
    CHECK(bs[8]);
    CHECK(bs[9]);
    CHECK(bs.count() == 4u);
}

TEMPLATE_TEST_CASE("construct with all bits set", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<9, TestType>{stdx::all_bits};
    STATIC_REQUIRE(bs.all());
}

TEMPLATE_TEST_CASE("reset range of bits (lsb, length)", "[bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::bitset<64, TestType>{stdx::all_bits};
    bs.reset(6_lsb, 4_len);
    CHECK(not bs[6]);
    CHECK(not bs[7]);
    CHECK(not bs[8]);
    CHECK(not bs[9]);
    CHECK(bs.count() == 60);
}

TEMPLATE_TEST_CASE("reset range of bits (lsb, msb)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::bitset<64, TestType>{stdx::all_bits};
    bs.reset(6_lsb, 9_msb);
    CHECK(not bs[6]);
    CHECK(not bs[7]);
    CHECK(not bs[8]);
    CHECK(not bs[9]);
    CHECK(bs.count() == 60);
}

TEMPLATE_TEST_CASE("set/reset all bits with size at type capacity", "[bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto sz = std::numeric_limits<TestType>::digits;
    constexpr auto expected = std::numeric_limits<TestType>::max();

    constexpr auto bs1 = stdx::bitset<sz, TestType>{stdx::all_bits};
    STATIC_REQUIRE(bs1.all());
    STATIC_REQUIRE(bs1.template to<TestType>() == expected);

    auto bs2 = stdx::bitset<sz, TestType>{};
    bs2.set();
    CHECK(bs2.all());
    CHECK(bs2.template to<TestType>() == expected);
    bs2.reset();
    CHECK(bs2.none());
}

TEMPLATE_TEST_CASE("find lowest unset bit (element 0)", "[bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::bitset<4, TestType>{stdx::place_bits, 0, 1, 3};
    STATIC_REQUIRE(bs.lowest_unset() == 2);
}

TEMPLATE_TEST_CASE("find lowest unset bit (element > 0)", "[bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto sz = std::numeric_limits<TestType>::digits * 2;
    auto bs = stdx::bitset<sz, TestType>{stdx::all_bits};
    bs.reset(sz - 3);
    CHECK(bs.lowest_unset() == sz - 3);
}

TEMPLATE_TEST_CASE("find lowest unset bit (full)", "[bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto sz = std::numeric_limits<TestType>::digits;
    constexpr auto bs = stdx::bitset<sz, TestType>{stdx::all_bits};
    STATIC_REQUIRE(bs.lowest_unset() == sz);
}

namespace {
enum struct Bits : std::uint8_t { ZERO, ONE, TWO, THREE, MAX };
}

TEST_CASE("use bitset with enum struct (default construct)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{};
    STATIC_REQUIRE(bs.size() == stdx::to_underlying(Bits::MAX));
}

TEST_CASE("use bitset with enum struct (to)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::all_bits};
    STATIC_REQUIRE(bs.to<Bits>() == static_cast<Bits>(0b1111));
}

TEST_CASE("use bitset with enum struct (set/flip)", "[bitset]") {
    auto bs = stdx::bitset<Bits::MAX>{};
    bs.set(Bits::ZERO);
    CHECK(bs.to_natural() == 1);
    bs.reset(Bits::ZERO);
    CHECK(bs.to_natural() == 0);
    bs.flip(Bits::ZERO);
    CHECK(bs.to_natural() == 1);
}

TEST_CASE("use bitset with enum struct (read index)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::all_bits};
    STATIC_REQUIRE(bs[Bits::ZERO]);
    STATIC_REQUIRE(bs[Bits::ONE]);
    STATIC_REQUIRE(bs[Bits::TWO]);
    STATIC_REQUIRE(bs[Bits::THREE]);
}

TEST_CASE("use bitset with enum struct (place_bits construct)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::place_bits, Bits::ZERO};
    STATIC_REQUIRE(bs.to_natural() == 1);
}

TEST_CASE("use bitset with enum struct (for_each)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::all_bits};
    for_each([](Bits) {}, bs);
}

TEST_CASE("use bitset with enum struct (transform_reduce)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::all_bits};
    CHECK(transform_reduce([](Bits) { return true; }, std::logical_or{}, false,
                           bs));
}

TEST_CASE("use bitset with enum struct (lowest_unset)", "[bitset]") {
    constexpr auto bs = stdx::bitset<Bits::MAX>{stdx::place_bits, Bits::ZERO};
    CHECK(bs.lowest_unset() == Bits::ONE);
}

#if __cplusplus >= 202002L
TEST_CASE("construct with a ct_string", "[bitset]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(stdx::bitset{"1010"_cts} ==
                   stdx::bitset<4ul, std::uint8_t>{0b1010ul});
    STATIC_REQUIRE(stdx::bitset{"101010101"_cts} ==
                   stdx::bitset<9ul, std::uint16_t>{0b101010101ul});
}
#endif

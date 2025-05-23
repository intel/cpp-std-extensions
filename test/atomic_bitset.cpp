#include <stdx/atomic_bitset.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <thread>
#include <type_traits>

TEST_CASE("atomic_bitset with explicit storage element type",
          "[atomic_bitset]") {
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<1, std::uint8_t>) ==
                   sizeof(std::uint8_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<8, std::uint8_t>) ==
                   sizeof(std::uint8_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<1, std::uint16_t>) ==
                   sizeof(std::uint16_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<16, std::uint16_t>) ==
                   sizeof(std::uint16_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<1, std::uint32_t>) ==
                   sizeof(std::uint32_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<32, std::uint32_t>) ==
                   sizeof(std::uint32_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<1, std::uint64_t>) ==
                   sizeof(std::uint64_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<64, std::uint64_t>) ==
                   sizeof(std::uint64_t));
}

TEST_CASE("atomic_bitset with implicit storage element type",
          "[atomic_bitset]") {
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<1>) == sizeof(std::uint8_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<8>) == sizeof(std::uint8_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<9>) == sizeof(std::uint16_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<16>) == sizeof(std::uint16_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<17>) == sizeof(std::uint32_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<32>) == sizeof(std::uint32_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<33>) == sizeof(std::uint64_t));
    STATIC_REQUIRE(sizeof(stdx::atomic_bitset<64>) == sizeof(std::uint64_t));
}

TEMPLATE_TEST_CASE("index operation", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    CHECK(not stdx::atomic_bitset<1, TestType>{}[0]);
}

TEMPLATE_TEST_CASE("set single bit", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<1, TestType>{};
    CHECK(not bs[0]);
    bs.set(0);
    CHECK(bs[0]);
    bs.set(0, false);
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("set all bits", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<1, TestType>{};
    CHECK(not bs[0]);
    bs.set();
    CHECK(bs[0]);
}

TEMPLATE_TEST_CASE("reset single bit", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<1, TestType>{1ul};
    CHECK(bs[0]);
    bs.reset(0);
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("reset all bits", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<1, TestType>{1ul};
    CHECK(bs[0]);
    bs.reset();
    CHECK(not bs[0]);
}

TEMPLATE_TEST_CASE("flip single bit", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<3, TestType>{0b101ul};
    CHECK(bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
    bs.flip(0);
    CHECK(not bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
}

TEMPLATE_TEST_CASE("flip all bits", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<3, TestType>{0b101ul};
    CHECK(bs[0]);
    CHECK(not bs[1]);
    CHECK(bs[2]);
    bs.flip();
    CHECK(not bs[0]);
    CHECK(bs[1]);
    CHECK(not bs[2]);
}

TEMPLATE_TEST_CASE("default construct", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    [[maybe_unused]] constexpr auto bs1 = stdx::atomic_bitset<1, TestType>{};
}

TEMPLATE_TEST_CASE("construct with a value", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::atomic_bitset<1, TestType>{1ul};
    CHECK(bs1[0]);

    constexpr auto bs2 = stdx::atomic_bitset<3, TestType>{255ul};
    CHECK(bs2[0]);
    CHECK(bs2[1]);
}

TEMPLATE_TEST_CASE("construct with values for bits", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs =
        stdx::atomic_bitset<8, TestType>{stdx::place_bits, 1, 3, 5};
    CHECK(not bs[0]);
    CHECK(bs[1]);
    CHECK(bs[3]);
    CHECK(bs[5]);
}

TEMPLATE_TEST_CASE("convert to unsigned integral type (same underlying type)",
                   "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::atomic_bitset<3, TestType>{255ul};
    auto const val = bs.template to<TestType>();
    CHECK(std::is_same_v<decltype(val), TestType const>);
    CHECK(val == 7u);
    CHECK(bs.template to<TestType>(std::memory_order_acquire) == 7u);
}

TEMPLATE_TEST_CASE(
    "convert to unsigned integral type (different underlying type)",
    "[atomic_bitset]", std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs =
        stdx::atomic_bitset<8, std::uint8_t>{stdx::place_bits, 3, 7};
    auto const val = bs.to<TestType>();
    CHECK(std::is_same_v<decltype(val), TestType const>);
    CHECK(val == 0b1000'1000u);
    CHECK(bs.template to<TestType>(std::memory_order_acquire) == 0b1000'1000u);
}

TEMPLATE_TEST_CASE("convert to natural type", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs =
        stdx::atomic_bitset<8, TestType>{stdx::place_bits, 3, 7};
    auto const val = bs.to_natural();
    CHECK(std::is_same_v<decltype(val), TestType const>);
    CHECK(val == 0b1000'1000u);
    CHECK(bs.to_natural(std::memory_order_acquire) == 0b1000'1000u);
}

TEST_CASE("to_natural returns smallest_uint", "[atomic_bitset]") {
    auto bs = stdx::atomic_bitset<4>{stdx::all_bits};
    auto value = bs.to_natural();
    CHECK(value == 0b1111);
    STATIC_REQUIRE(std::is_same_v<decltype(value), std::uint8_t>);
}

TEMPLATE_TEST_CASE("construct with a string_view", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace std::string_view_literals;
    CHECK(stdx::atomic_bitset<4, TestType>{"1010"sv}.to_natural() == 0b1010ul);
}

TEMPLATE_TEST_CASE("construct with a substring", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace std::string_view_literals;
    CHECK(
        stdx::atomic_bitset<4, TestType>{"XOXOXO"sv, 2, 4, 'X'}.to_natural() ==
        0b1010ul);
}

TEMPLATE_TEST_CASE("all", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::atomic_bitset<8, TestType>{0b1111'1111u};
    CHECK(bs1.all());

    constexpr auto bs2 = stdx::atomic_bitset<8, TestType>{0xb1111'1110u};
    CHECK(not bs2.all());
}

TEMPLATE_TEST_CASE("any", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::atomic_bitset<8, TestType>{8ul};
    CHECK(bs1.any());

    constexpr auto bs2 = stdx::atomic_bitset<8, TestType>{};
    CHECK(not bs2.any());
}

TEMPLATE_TEST_CASE("none", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::atomic_bitset<8, TestType>{};
    CHECK(bs1.none());

    constexpr auto bs2 = stdx::atomic_bitset<8, TestType>{8ul};
    CHECK(not bs2.none());
}

TEMPLATE_TEST_CASE("count", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs1 = stdx::atomic_bitset<8, TestType>{};
    CHECK(bs1.count() == 0u);

    constexpr auto bs2 = stdx::atomic_bitset<8, TestType>{0b10101ul};
    CHECK(bs2.count() == 3u);
}

TEMPLATE_TEST_CASE("conversion to bitset", "[atomic_bitset]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::atomic_bitset<8, TestType>{0b10101ul};
    CHECK(bs == stdx::bitset<8, TestType>{0b10101ul});
}

TEMPLATE_TEST_CASE("load", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::atomic_bitset<8, TestType>{0b10101ul};
    CHECK(bs.load() == stdx::bitset<8, TestType>{0b10101ul});
    CHECK(bs.load(std::memory_order_acquire) ==
          stdx::bitset<8, TestType>{0b10101ul});
}

TEMPLATE_TEST_CASE("store", "[atomic_bitset]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    auto bs = stdx::atomic_bitset<8, TestType>{};
    bs.store(stdx::bitset<8, TestType>{0b10101ul});
    CHECK(bs.to_natural() == 0b10101ul);

    bs.store(stdx::bitset<8, TestType>{0b1010ul}, std::memory_order_release);
    CHECK(bs.to_natural() == 0b1010ul);
}

TEMPLATE_TEST_CASE("set range of bits (lsb, length)", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<CHAR_BIT * sizeof(TestType), TestType>{};
    bs.set(2_lsb, 4_len);
    CHECK(bs[2]);
    CHECK(bs[3]);
    CHECK(bs[4]);
    CHECK(bs[5]);
    CHECK(bs.count() == 4u);
}

TEMPLATE_TEST_CASE("set range of bits (lsb, msb)", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<CHAR_BIT * sizeof(TestType), TestType>{};
    bs.set(2_lsb, 5_msb);
    CHECK(bs[2]);
    CHECK(bs[3]);
    CHECK(bs[4]);
    CHECK(bs[5]);
    CHECK(bs.count() == 4u);
}

TEMPLATE_TEST_CASE("construct with all bits set", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::atomic_bitset<8, TestType>{stdx::all_bits};
    CHECK(bs.all());
}

TEMPLATE_TEST_CASE("reset range of bits (lsb, length)", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<CHAR_BIT * sizeof(TestType), TestType>{
        stdx::all_bits};
    bs.reset(2_lsb, 4_len);
    CHECK(not bs[2]);
    CHECK(not bs[3]);
    CHECK(not bs[4]);
    CHECK(not bs[5]);
    CHECK(bs.count() == bs.size() - 4);
}

TEMPLATE_TEST_CASE("reset range of bits (lsb, msb)", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<CHAR_BIT * sizeof(TestType), TestType>{
        stdx::all_bits};
    bs.reset(2_lsb, 5_msb);
    CHECK(not bs[2]);
    CHECK(not bs[3]);
    CHECK(not bs[4]);
    CHECK(not bs[5]);
    CHECK(bs.count() == bs.size() - 4);
}

namespace {
enum struct Bits : std::uint8_t { ZERO, ONE, TWO, THREE, MAX };
}

TEST_CASE("use atomic_bitset with enum struct (construct)", "[atomic_bitset]") {
    constexpr auto bs = stdx::atomic_bitset<Bits::MAX>{};
    STATIC_REQUIRE(bs.size() == stdx::to_underlying(Bits::MAX));
}

TEST_CASE("use atomic_bitset with enum struct (to)", "[atomic_bitset]") {
    constexpr auto bs = stdx::atomic_bitset<Bits::MAX>{stdx::all_bits};
    CHECK(bs.to<Bits>() == static_cast<Bits>(0b1111));
}

TEST_CASE("use atomic_bitset with enum struct (set/flip)", "[atomic_bitset]") {
    auto bs = stdx::atomic_bitset<Bits::MAX>{};
    bs.set(Bits::ZERO);
    CHECK(bs.to_natural() == 1);
    bs.reset(Bits::ZERO);
    CHECK(bs.to_natural() == 0);
    bs.flip(Bits::ZERO);
    CHECK(bs.to_natural() == 1);
}

TEST_CASE("use atomic_bitset with enum struct (read index)",
          "[atomic_bitset]") {
    constexpr auto bs = stdx::atomic_bitset<Bits::MAX>{stdx::all_bits};
    CHECK(bs[Bits::ZERO]);
    CHECK(bs[Bits::ONE]);
    CHECK(bs[Bits::TWO]);
    CHECK(bs[Bits::THREE]);
}

TEMPLATE_TEST_CASE("set returns the previous bitset", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<8, TestType>{};
    CHECK(bs.set(0) == stdx::bitset<8, TestType>{});
    CHECK(bs[0]);
    CHECK(bs.set(1_lsb, 1_len) == stdx::bitset<8, TestType>{0b1ul});
    CHECK(bs[1]);
    CHECK(bs.set(2_lsb, 2_msb) == stdx::bitset<8, TestType>{0b11ul});
    CHECK(bs[2]);
}

TEMPLATE_TEST_CASE("reset returns the previous bitset", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<8, TestType>{0b111ul};
    CHECK(bs.reset(0) == stdx::bitset<8, TestType>{0b111ul});
    CHECK(not bs[0]);
    CHECK(bs.reset(1_lsb, 1_len) == stdx::bitset<8, TestType>{0b110ul});
    CHECK(not bs[1]);
    CHECK(bs.reset(2_lsb, 2_msb) == stdx::bitset<8, TestType>{0b100ul});
    CHECK(not bs[2]);
}

TEMPLATE_TEST_CASE("flip returns the previous bitset", "[atomic_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    using namespace stdx::literals;
    auto bs = stdx::atomic_bitset<8, TestType>{0b111ul};
    CHECK(bs.flip(0) == stdx::bitset<8, TestType>{0b111ul});
    CHECK(not bs[0]);
    CHECK(bs.flip(1_lsb, 1_len) == stdx::bitset<8, TestType>{0b110ul});
    CHECK(not bs[1]);
    CHECK(bs.flip(2_lsb, 2_msb) == stdx::bitset<8, TestType>{0b100ul});
    CHECK(not bs[2]);
}

TEST_CASE("atomic bitset is thread-safe", "[atomic_bitset]") {
    auto bs = stdx::atomic_bitset<1, std::uint8_t>{};
    auto t1 = std::thread([&] { bs.set(0); });
    auto t2 = std::thread([&] { bs.set(0); });

    t1.join();
    t2.join();
}

#if __cplusplus >= 202002L
TEST_CASE("construct with a ct_string", "[atomic_bitset]") {
    using namespace stdx::literals;
    CHECK(stdx::atomic_bitset{"1010"_cts} ==
          stdx::bitset<4ul, std::uint8_t>{0b1010ul});
    CHECK(stdx::atomic_bitset{"101010101"_cts} ==
          stdx::bitset<9ul, std::uint16_t>{0b101010101ul});
}
#endif

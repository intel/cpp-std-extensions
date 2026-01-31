#include <stdx/bitset.hpp>
#include <stdx/ct_conversions.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

TEST_CASE("bitset size", "[type_bitset]") {
    STATIC_CHECK(stdx::type_bitset<int>{}.size() == 1u);
    STATIC_CHECK(stdx::type_bitset<int, float>{}.size() == 2u);
}

TEST_CASE("index operation", "[type_bitset]") {
    STATIC_CHECK(not stdx::type_bitset<int>{}[stdx::type_identity_v<int>]);
}

TEST_CASE("set single bit", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{};
    CHECK(not bs[stdx::type_identity_v<int>]);
    bs.set<int>();
    CHECK(bs[stdx::type_identity_v<int>]);
    bs.set<int>(false);
    CHECK(not bs[stdx::type_identity_v<int>]);
}

TEST_CASE("reset single bit", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    CHECK(bs[stdx::type_identity_v<int>]);
    bs.reset<int>();
    CHECK(not bs[stdx::type_identity_v<int>]);
}

TEST_CASE("flip single bit", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    CHECK(bs[stdx::type_identity_v<int>]);
    bs.flip<int>();
    CHECK(not bs[stdx::type_identity_v<int>]);
}

TEST_CASE("construct with a value", "[type bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float>{1ul};
    STATIC_CHECK(bs1[stdx::type_identity_v<int>]);

    constexpr auto bs2 = stdx::type_bitset<int, float>{255ul};
    STATIC_CHECK(bs2[stdx::type_identity_v<int>]);
    STATIC_CHECK(bs2[stdx::type_identity_v<float>]);
}

TEST_CASE("construct with values for bits", "[type_bitset]") {
    constexpr auto bs =
        stdx::type_bitset<int, float, bool>{stdx::type_list<int, bool>{}};
    STATIC_CHECK(bs[stdx::type_identity_v<int>]);
    STATIC_CHECK(not bs[stdx::type_identity_v<float>]);
    STATIC_CHECK(bs[stdx::type_identity_v<bool>]);
}

TEMPLATE_TEST_CASE("convert to unsigned integral type", "[type_bitset]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr auto bs = stdx::type_bitset<int, float, bool>{255ul};
    constexpr auto val = bs.template to<TestType>();
    STATIC_CHECK(std::is_same_v<decltype(val), TestType const>);
    STATIC_CHECK(val == 7u);
}

TEST_CASE("convert to type that fits", "[type_bitset]") {
    constexpr auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    constexpr auto val = bs.to_natural();
    STATIC_CHECK(std::is_same_v<decltype(val), std::uint8_t const>);
    STATIC_CHECK(val == 7u);
}

TEST_CASE("all", "[type bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    STATIC_CHECK(bs1.all());

    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{};
    STATIC_CHECK(not bs2.all());
}

TEST_CASE("any", "[type bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    STATIC_CHECK(bs1.any());

    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{};
    STATIC_CHECK(not bs2.any());
}

TEST_CASE("none", "[type bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    STATIC_CHECK(not bs1.none());

    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{};
    STATIC_CHECK(bs2.none());
}

TEST_CASE("count", "[type_bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{};
    STATIC_CHECK(bs1.count() == 0u);

    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    STATIC_CHECK(bs2.count() == 3u);
}

TEST_CASE("set all bits", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{};
    bs.set();
    CHECK(bs == stdx::type_bitset<int, float, bool>{stdx::all_bits});
    bs.set();
    CHECK(bs == stdx::type_bitset<int, float, bool>{stdx::all_bits});
}

TEST_CASE("reset all bits", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    bs.reset();
    CHECK(bs == stdx::type_bitset<int, float, bool>{});
    bs.reset();
    CHECK(bs == stdx::type_bitset<int, float, bool>{});
}

TEST_CASE("flip all bits", "[type_bitset]") {
    auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    bs.flip();
    CHECK(bs == stdx::type_bitset<int, float, bool>{});
    bs.flip();
    CHECK(bs == stdx::type_bitset<int, float, bool>{stdx::all_bits});
}

TEST_CASE("or", "[type_bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{0b101ul};
    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{0b010ul};
    STATIC_CHECK((bs1 | bs2) ==
                 stdx::type_bitset<int, float, bool>{stdx::all_bits});
}

TEST_CASE("and", "[type_bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{0b101ul};
    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{0b100ul};
    STATIC_CHECK((bs1 & bs2) == stdx::type_bitset<int, float, bool>{0b100ul});
}

TEST_CASE("xor", "[type_bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{0b101ul};
    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{0b010ul};
    STATIC_CHECK((bs1 ^ bs2) ==
                 stdx::type_bitset<int, float, bool>{stdx::all_bits});
}

TEST_CASE("not", "[type_bitset]") {
    constexpr auto bs = stdx::type_bitset<int, float, bool>{0b101ul};
    STATIC_CHECK(~bs == stdx::type_bitset<int, float, bool>{0b10ul});
}

TEST_CASE("difference", "[type_bitset]") {
    constexpr auto bs1 = stdx::type_bitset<int, float, bool>{0b101ul};
    constexpr auto bs2 = stdx::type_bitset<int, float, bool>{0b011ul};
    STATIC_CHECK((bs1 - bs2) == stdx::type_bitset<int, float, bool>{0b100ul});
}

#if __cplusplus >= 202002L
TEST_CASE("for_each", "[type_bitset]") {
    constexpr auto bs = stdx::type_bitset<int, float, bool>{stdx::all_bits};
    auto result = std::string{};
    bs.for_each([&]<typename T, std::size_t I>() -> void {
        result += std::string{stdx::type_as_string<T>()} + std::to_string(I);
    });
    CHECK(result == "int0float1bool2");
}
#endif

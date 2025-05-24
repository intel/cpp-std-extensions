#include <stdx/udls.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time named bools", "[utility]") {
    using namespace stdx::literals;
    STATIC_REQUIRE("variable"_b);
    STATIC_REQUIRE(not(not "variable"_b));

    STATIC_REQUIRE("variable"_true);
    STATIC_REQUIRE(not "variable"_false);
}

TEST_CASE("decimal units", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(1_k == 1'000ull);
    STATIC_REQUIRE(1_M == 1'000'000ull);
    STATIC_REQUIRE(1_G == 1'000'000'000ull);
}

TEST_CASE("binary units", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(1_ki == 1'024ull);
    STATIC_REQUIRE(1_Mi == 1'024ull * 1'024ull);
    STATIC_REQUIRE(1_Gi == 1'024ull * 1'024ull * 1'024ull);
}

TEST_CASE("compile-time named small indices", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(std::is_same_v<decltype("index"_0),
                                  std::integral_constant<size_t, 0u>>);
    STATIC_REQUIRE("index"_0 == 0u);
    STATIC_REQUIRE("index"_1 == 1u);
    STATIC_REQUIRE("index"_2 == 2u);
    STATIC_REQUIRE("index"_3 == 3u);
    STATIC_REQUIRE("index"_4 == 4u);
    STATIC_REQUIRE("index"_5 == 5u);
    STATIC_REQUIRE("index"_6 == 6u);
    STATIC_REQUIRE("index"_7 == 7u);
    STATIC_REQUIRE("index"_8 == 8u);
    STATIC_REQUIRE("index"_9 == 9u);
}

TEST_CASE("compile-time constant", "[units]") {
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::_c<0>),
                                  std::integral_constant<int, 0> const>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(stdx::_c<0u>),
                       std::integral_constant<unsigned int, 0> const>);
}

TEST_CASE("compile-time literal (decimal)", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(std::is_same_v<decltype(0_c),
                                  std::integral_constant<std::uint32_t, 0>>);
}

TEST_CASE("compile-time literal supports digit separators", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(
        std::is_same_v<decltype(123'456_c),
                       std::integral_constant<std::uint32_t, 123'456>>);
}

TEST_CASE("compile-time literal (octal)", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(std::is_same_v<decltype(010_c),
                                  std::integral_constant<std::uint32_t, 8>>);
    STATIC_REQUIRE(std::is_same_v<decltype(0'10_c),
                                  std::integral_constant<std::uint32_t, 8>>);
}

TEST_CASE("compile-time literal (binary)", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(std::is_same_v<decltype(0b11_c),
                                  std::integral_constant<std::uint32_t, 3>>);
    STATIC_REQUIRE(std::is_same_v<decltype(0b1'1_c),
                                  std::integral_constant<std::uint32_t, 3>>);
}

TEST_CASE("compile-time literal (hex)", "[units]") {
    using namespace stdx::literals;
    STATIC_REQUIRE(std::is_same_v<decltype(0xaa_c),
                                  std::integral_constant<std::uint32_t, 170>>);
    STATIC_REQUIRE(std::is_same_v<decltype(0xAA_c),
                                  std::integral_constant<std::uint32_t, 170>>);
    STATIC_REQUIRE(std::is_same_v<decltype(0xA'a_c),
                                  std::integral_constant<std::uint32_t, 170>>);
}

namespace {
enum UnscopedEnum { Value3 = 3 };
enum struct ScopedEnum : char { Value5 = 5 };
} // namespace

TEST_CASE("compile-time enum constant", "[units]") {
    using namespace stdx;
    STATIC_REQUIRE(
        std::is_same_v<decltype(_c<Value3>),
                       std::integral_constant<UnscopedEnum, Value3> const>);
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(_c<ScopedEnum::Value5>),
            std::integral_constant<ScopedEnum, ScopedEnum::Value5> const>);
}

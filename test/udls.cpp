#include <stdx/udls.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time named bools", "[utility]") {
    using namespace stdx::literals;
    static_assert("variable"_b);
    static_assert(not(not "variable"_b));

    static_assert("variable"_true);
    static_assert(not "variable"_false);
}

TEST_CASE("decimal units", "[units]") {
    using namespace stdx::literals;
    static_assert(1_k == 1'000ull);
    static_assert(1_M == 1'000'000ull);
    static_assert(1_G == 1'000'000'000ull);
}

TEST_CASE("binary units", "[units]") {
    using namespace stdx::literals;
    static_assert(1_ki == 1'024ull);
    static_assert(1_Mi == 1'024ull * 1'024ull);
    static_assert(1_Gi == 1'024ull * 1'024ull * 1'024ull);
}

TEST_CASE("compile-time named small indices", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype("index"_0),
                                 std::integral_constant<size_t, 0u>>);
    static_assert("index"_0 == 0u);
    static_assert("index"_1 == 1u);
    static_assert("index"_2 == 2u);
    static_assert("index"_3 == 3u);
    static_assert("index"_4 == 4u);
    static_assert("index"_5 == 5u);
    static_assert("index"_6 == 6u);
    static_assert("index"_7 == 7u);
    static_assert("index"_8 == 8u);
    static_assert("index"_9 == 9u);
}

TEST_CASE("compile-time constant", "[units]") {
    static_assert(std::is_same_v<decltype(stdx::_c<0>),
                                 std::integral_constant<int, 0> const>);
    static_assert(
        std::is_same_v<decltype(stdx::_c<0u>),
                       std::integral_constant<unsigned int, 0> const>);
}

TEST_CASE("compile-time literal (decimal)", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype(0_c),
                                 std::integral_constant<std::uint32_t, 0>>);
}

TEST_CASE("compile-time literal supports digit separators", "[units]") {
    using namespace stdx::literals;
    static_assert(
        std::is_same_v<decltype(123'456_c),
                       std::integral_constant<std::uint32_t, 123'456>>);
}

TEST_CASE("compile-time literal (octal)", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype(010_c),
                                 std::integral_constant<std::uint32_t, 8>>);
    static_assert(std::is_same_v<decltype(0'10_c),
                                 std::integral_constant<std::uint32_t, 8>>);
}

TEST_CASE("compile-time literal (binary)", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype(0b11_c),
                                 std::integral_constant<std::uint32_t, 3>>);
    static_assert(std::is_same_v<decltype(0b1'1_c),
                                 std::integral_constant<std::uint32_t, 3>>);
}

TEST_CASE("compile-time literal (hex)", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype(0xaa_c),
                                 std::integral_constant<std::uint32_t, 170>>);
    static_assert(std::is_same_v<decltype(0xAA_c),
                                 std::integral_constant<std::uint32_t, 170>>);
    static_assert(std::is_same_v<decltype(0xA'a_c),
                                 std::integral_constant<std::uint32_t, 170>>);
}

namespace {
enum UnscopedEnum { Value3 = 3 };
enum struct ScopedEnum : char { Value5 = 5 };
} // namespace

TEST_CASE("compile-time enum constant", "[units]") {
    using namespace stdx;
    static_assert(
        std::is_same_v<decltype(_c<Value3>),
                       std::integral_constant<UnscopedEnum, Value3> const>);
    static_assert(
        std::is_same_v<
            decltype(_c<ScopedEnum::Value5>),
            std::integral_constant<ScopedEnum, ScopedEnum::Value5> const>);
}

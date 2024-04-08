#include <stdx/utility.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("forward_like", "[utility]") {
    static_assert(std::is_same_v<stdx::forward_like_t<int, float>, float &&>);
    static_assert(
        std::is_same_v<stdx::forward_like_t<int const, float>, float const &&>);

    static_assert(
        std::is_same_v<stdx::forward_like_t<int &&, float>, float &&>);
    static_assert(std::is_same_v<stdx::forward_like_t<int const &&, float>,
                                 float const &&>);

    static_assert(std::is_same_v<stdx::forward_like_t<int &, float>, float &>);
    static_assert(std::is_same_v<stdx::forward_like_t<int const &, float>,
                                 float const &>);
}

TEST_CASE("as_unsigned (changed)", "[utility]") {
    static_assert(std::is_same_v<decltype(stdx::as_unsigned(std::int8_t{})),
                                 std::uint8_t>);
    static_assert(std::is_same_v<decltype(stdx::as_unsigned(std::int16_t{})),
                                 std::uint16_t>);
    static_assert(std::is_same_v<decltype(stdx::as_unsigned(std::int32_t{})),
                                 std::uint32_t>);
    static_assert(std::is_same_v<decltype(stdx::as_unsigned(std::int64_t{})),
                                 std::uint64_t>);
}

TEMPLATE_TEST_CASE("as_unsigned (unchanged)", "[utility]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    static_assert(
        std::is_same_v<decltype(stdx::as_unsigned(TestType{})), TestType>);
}

TEST_CASE("as_signed (changed)", "[utility]") {
    static_assert(
        std::is_same_v<decltype(stdx::as_signed(std::uint8_t{})), std::int8_t>);
    static_assert(std::is_same_v<decltype(stdx::as_signed(std::uint16_t{})),
                                 std::int16_t>);
    static_assert(std::is_same_v<decltype(stdx::as_signed(std::uint32_t{})),
                                 std::int32_t>);
    static_assert(std::is_same_v<decltype(stdx::as_signed(std::uint64_t{})),
                                 std::int64_t>);
}

TEMPLATE_TEST_CASE("as_signed (unchanged)", "[utility]", std::int8_t,
                   std::int16_t, std::int32_t, std::int64_t) {
    static_assert(
        std::is_same_v<decltype(stdx::as_signed(TestType{})), TestType>);
}

TEMPLATE_TEST_CASE("sized<T> in (uint8_t zero/one/two case)", "[bit]",
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    static_assert(stdx::sized<std::uint8_t>{0}.in<TestType>() == 0);
    static_assert(stdx::sized<std::uint8_t>{1}.in<TestType>() == 1);
    static_assert(stdx::sized<std::uint8_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (uint16_t zero/one/two case)", "[bit]",
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::sized<std::uint16_t>{0}.in<TestType>() == 0);
    static_assert(stdx::sized<std::uint16_t>{1}.in<TestType>() == 1);
    static_assert(stdx::sized<std::uint16_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (uint32_t zero/one/two case)", "[bit]",
                   std::uint64_t) {
    static_assert(stdx::sized<std::uint32_t>{0}.in<TestType>() == 0);
    static_assert(stdx::sized<std::uint32_t>{1}.in<TestType>() == 1);
    static_assert(stdx::sized<std::uint32_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (same type)", "[bit]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    static_assert(stdx::sized<TestType>{3}.template in<TestType>() == 3);
}

TEMPLATE_TEST_CASE("sized<T> in (exact case)", "[bit]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(
        stdx::sized<std::uint8_t>{3 * sizeof(TestType)}.in<TestType>() == 3);
}

TEMPLATE_TEST_CASE("sized<T> in defaults to bytes", "[bit]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    static_assert(stdx::sized<TestType>{3}.in() == 3 * sizeof(TestType));
}

TEMPLATE_TEST_CASE("sized<T> in (downsize)", "[bit]", std::uint8_t,
                   std::uint16_t, std::uint32_t) {
    static_assert(stdx::sized<std::uint64_t>{3}.in<TestType>() ==
                  3 * sizeof(std::uint64_t) / sizeof(TestType));
}

TEST_CASE("sized<T> in (upsize uint8_t)", "[bit]") {
    static_assert(stdx::sized<std::uint8_t>{3}.in<std::uint16_t>() == 2);
    static_assert(stdx::sized<std::uint8_t>{17}.in<std::uint16_t>() == 9);

    static_assert(stdx::sized<std::uint8_t>{5}.in<std::uint32_t>() == 2);
    static_assert(stdx::sized<std::uint8_t>{17}.in<std::uint32_t>() == 5);

    static_assert(stdx::sized<std::uint8_t>{9}.in<std::uint64_t>() == 2);
    static_assert(stdx::sized<std::uint8_t>{17}.in<std::uint64_t>() == 3);
}

TEST_CASE("sized<T> in (upsize uint16_t)", "[bit]") {
    static_assert(stdx::sized<std::uint16_t>{3}.in<std::uint32_t>() == 2);
    static_assert(stdx::sized<std::uint16_t>{17}.in<std::uint32_t>() == 9);

    static_assert(stdx::sized<std::uint16_t>{5}.in<std::uint64_t>() == 2);
    static_assert(stdx::sized<std::uint16_t>{17}.in<std::uint64_t>() == 5);
}

TEST_CASE("sized<T> in (upsize uint32_t)", "[bit]") {
    static_assert(stdx::sized<std::uint32_t>{3}.in<std::uint64_t>() == 2);
    static_assert(stdx::sized<std::uint32_t>{17}.in<std::uint64_t>() == 9);
}

TEST_CASE("sized<T> aliases", "[bit]") {
    static_assert(stdx::sized8{1}.in<std::uint64_t>() == 1);
    static_assert(stdx::sized16{1}.in<std::uint64_t>() == 1);
    static_assert(stdx::sized32{1}.in<std::uint64_t>() == 1);
    static_assert(stdx::sized64{1}.in<std::uint64_t>() == 1);
}

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

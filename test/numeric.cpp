#include <stdx/numeric.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <functional>
#include <iterator>

TEST_CASE("unary transform_reduce", "[numeric]") {
    auto const input = std::array{1, 2, 3, 4};
    auto const v =
        stdx::transform_reduce(std::cbegin(input), std::cend(input), 0,
                               std::plus{}, [](auto n) { return n * 2; });
    CHECK(v == 20);
}

TEST_CASE("n-ary transform_reduce", "[numeric]") {
    auto const input = std::array{1, 2, 3, 4};
    auto const v = stdx::transform_reduce(
        std::cbegin(input), std::cend(input), 0, std::plus{},
        [](auto... ns) { return (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input), std::cbegin(input));
    CHECK(v == 40);
}

TEST_CASE("unary transform_reduce_n", "[numeric]") {
    auto const input = std::array{1, 2, 3, 4};
    auto const v =
        stdx::transform_reduce_n(std::cbegin(input), std::size(input), 0,
                                 std::plus{}, [](auto n) { return n * 2; });
    CHECK(v == 20);
}

TEST_CASE("n-ary transform_reduce_n", "[numeric]") {
    auto const input = std::array{1, 2, 3, 4};
    auto const v = stdx::transform_reduce_n(
        std::cbegin(input), std::size(input), 0, std::plus{},
        [](auto... ns) { return (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input), std::cbegin(input));
    CHECK(v == 40);
}

TEST_CASE("transform_reduce with output fixed by template argument",
          "[numeric]") {
    auto const input = std::array{1.5, 2.5, 3.5, 4.5};
    auto v =
        stdx::transform_reduce<double>(std::cbegin(input), std::cend(input), 0,
                                       std::plus{}, [](auto n) { return n; });
    static_assert(std::is_same_v<decltype(v), double>);
    CHECK(v == 12);
}

TEST_CASE("saturate_cast cppreference example", "[numeric]") {
    constexpr std::int16_t x1{696};

    constexpr std::int8_t x2 = stdx::saturate_cast<std::int8_t>(x1);
    CHECK(x2 == std::numeric_limits<std::int8_t>::max());

    constexpr std::uint8_t x3 = stdx::saturate_cast<std::uint8_t>(x1);
    CHECK(x3 == std::numeric_limits<std::uint8_t>::max());

    constexpr std::int16_t y1{-696};

    constexpr std::int8_t y2 = stdx::saturate_cast<std::int8_t>(y1);
    CHECK(y2 == std::numeric_limits<std::int8_t>::min());

    constexpr std::uint8_t y3 = stdx::saturate_cast<std::uint8_t>(y1);
    CHECK(y3 == 0);
}

TEST_CASE("saturate_cast signed large From, small To", "[numeric]") {
    static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{42}) == 42);
    static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{1000}) == 127);
    static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{-200}) == -128);
}

TEST_CASE("saturate_cast signed small From, large To", "[numeric]") {
    static_assert(stdx::saturate_cast<std::int32_t>(std::int8_t{42}) == 42);
}

TEST_CASE("saturate_cast unsigned From, signed To", "[numeric]") {
    static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{255u}) == 127);
    static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{42u}) == 42);
    static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{0u}) == 0);
}

TEST_CASE("saturate_cast signed From, unsigned To", "[numeric]") {
    static_assert(stdx::saturate_cast<std::uint8_t>(std::int8_t{-1}) == 0);
    static_assert(stdx::saturate_cast<std::uint8_t>(
                      std::numeric_limits<std::int8_t>::min()) == 0);
    static_assert(stdx::saturate_cast<std::uint8_t>(
                      std::numeric_limits<std::int8_t>::max()) ==
                  std::numeric_limits<std::int8_t>::max());
}

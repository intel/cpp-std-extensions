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

TEST_CASE("transform_reduce with output fixed by template arumgent",
          "[numeric]") {
    auto const input = std::array{1.5, 2.5, 3.5, 4.5};
    auto const v = stdx::transform_reduce<double>(
        std::cbegin(input), std::cend(input), 0, std::plus{},
        [](auto n) { return n * 2; });
    CHECK(v == 24);
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
    SECTION("std::int8_t from std::int32_t") {
        static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{42}) == 42);
        static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{1000}) ==
                      127);
        static_assert(stdx::saturate_cast<std::int8_t>(std::int32_t{-200}) ==
                      -128);
    }

    SECTION("Conversions from uint and int to smaller types") {
        static_assert(stdx::saturate_cast<std::uint8_t>(999) == 255);
        static_assert(stdx::saturate_cast<std::int8_t>(999) == 127);
        static_assert(stdx::saturate_cast<std::uint16_t>(999) == 999);
        static_assert(stdx::saturate_cast<std::int16_t>(999) == 999);
    }

    SECTION("Max value conversions") {
        static_assert(stdx::saturate_cast<std::int16_t>(
                          std::numeric_limits<std::int32_t>::max()) ==
                      std::numeric_limits<std::int16_t>::max());
        static_assert(stdx::saturate_cast<std::int32_t>(
                          std::numeric_limits<std::int32_t>::max()) ==
                      std::numeric_limits<std::int32_t>::max());
    }
}

TEST_CASE("saturate_cast signed small From, large To", "[numeric]") {
    static_assert(stdx::saturate_cast<std::int32_t>(std::int8_t{42}) == 42);
}

TEST_CASE("saturate_cast unsigned From, signed To", "[numeric]") {
    SECTION("Conversions from uint to int") {
        static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{255u}) ==
                      127);
        static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{42u}) ==
                      42);
        static_assert(stdx::saturate_cast<std::int8_t>(std::uint8_t{0u}) == 0);
    }

    SECTION("Max value conversions") {
        static_assert(stdx::saturate_cast<std::int16_t>(
                          std::numeric_limits<std::uint32_t>::max()) ==
                      std::numeric_limits<std::int16_t>::max());
        static_assert(stdx::saturate_cast<std::int32_t>(
                          std::numeric_limits<std::uint32_t>::max()) ==
                      std::numeric_limits<std::int32_t>::max());
    }

    SECTION("Min and max value conversions") {
        static_assert(stdx::saturate_cast<std::uint32_t>(
                          std::numeric_limits<std::uint32_t>::min()) == 0);
        static_assert(stdx::saturate_cast<std::uint32_t>(
                          std::numeric_limits<std::uint32_t>::max()) ==
                      std::numeric_limits<std::uint32_t>::max());
        static_assert(stdx::saturate_cast<std::uint32_t>(
                          std::numeric_limits<std::uint64_t>::max()) ==
                      std::numeric_limits<std::uint32_t>::max());
    }
}

TEST_CASE("saturate_cast signed From, unsigned To", "[numeric]") {
    SECTION("Negative value conversion") {
        static_assert(stdx::saturate_cast<std::uint8_t>(std::int8_t{-1}) == 0);
        static_assert(stdx::saturate_cast<std::uint8_t>(
                          std::numeric_limits<std::int8_t>::min()) == 0);
        static_assert(stdx::saturate_cast<std::uint8_t>(
                          std::numeric_limits<std::int8_t>::max()) ==
                      std::numeric_limits<std::int8_t>::max());
    }

    SECTION("Max value conversions") {
        static_assert(stdx::saturate_cast<std::uint32_t>(-1) == 0);
        static_assert(stdx::saturate_cast<std::uint32_t>(
                          std::numeric_limits<std::int64_t>::max()) ==
                      std::numeric_limits<std::uint32_t>::max());
    }
}

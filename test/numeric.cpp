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

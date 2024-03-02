#include <stdx/algorithm.hpp>
#include <stdx/tuple_destructure.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <iterator>

TEST_CASE("unary transform", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    auto [o, i] =
        stdx::transform(std::cbegin(input), std::cend(input),
                        std::begin(output), [](auto n) { return n + 1; });
    CHECK(o == std::end(output));
    CHECK(i == std::cend(input));
    CHECK(output == std::array{2, 3, 4, 5});
}

TEST_CASE("n-ary transform", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    stdx::transform(
        std::cbegin(input), std::cend(input), std::begin(output),
        [](auto... ns) { return (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input), std::cbegin(input));
    CHECK(output == std::array{4, 8, 12, 16});
}

TEST_CASE("unary transform_n", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    stdx::transform_n(std::cbegin(input), std::size(input), std::begin(output),
                      [](auto n) { return n + 1; });
    CHECK(output == std::array{2, 3, 4, 5});
}

TEST_CASE("n-ary transform_n", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    stdx::transform_n(
        std::cbegin(input), std::size(input), std::begin(output),
        [](auto... ns) { return (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input), std::cbegin(input));
    CHECK(output == std::array{4, 8, 12, 16});
}

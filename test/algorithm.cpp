#include <stdx/algorithm.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/tuple_destructure.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <forward_list>
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

TEST_CASE("n-ary for_each", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    auto [op, i1, i2] = stdx::for_each(
        std::cbegin(input), std::cend(input),
        [it = std::begin(output)](auto... ns) mutable {
            *it++ = (0 + ... + ns);
        },
        std::cbegin(input), std::cbegin(input));
    CHECK(i1 == std::cend(input));
    CHECK(i2 == std::cend(input));
    CHECK(output == std::array{3, 6, 9, 12});
}

TEST_CASE("unary for_each_n", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    auto f = [&sum, i = 0](auto n) mutable {
        sum += n;
        return ++i;
    };
    auto [op, i] = stdx::for_each_n(std::cbegin(input), 2, f);
    CHECK(i == std::next(std::cbegin(input), 2));
    CHECK(sum == 3);
    CHECK(op(42) == 3);
}

TEST_CASE("n-ary for_each_n", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto output = decltype(input){};
    stdx::for_each_n(
        std::cbegin(input), std::size(input),
        [it = std::begin(output)](auto... ns) mutable {
            *it++ = (0 + ... + ns);
        },
        std::cbegin(input), std::cbegin(input));
    CHECK(output == std::array{3, 6, 9, 12});
}

TEST_CASE("for_each_butlastn (random access)", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    auto [_, it] = stdx::for_each_butlastn(std::cbegin(input), std::cend(input),
                                           2, [&](auto n) { sum += n; });
    CHECK(sum == 3);
    CHECK(it == std::next(std::cbegin(input), 2));
}

TEST_CASE("for_each_butlastn (forward)", "[algorithm]") {
    auto const input = std::forward_list{1, 2, 3, 4};
    auto sum = 0;
    auto [_, it] = stdx::for_each_butlastn(std::cbegin(input), std::cend(input),
                                           2, [&](auto n) { sum += n; });
    CHECK(sum == 3);
    CHECK(it == std::next(std::cbegin(input), 2));
}

TEST_CASE("for_each_butlast (random access)", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    auto [_, it] = stdx::for_each_butlast(std::cbegin(input), std::cend(input),
                                          [&](auto n) { sum += n; });
    CHECK(sum == 6);
    CHECK(it == std::next(std::cbegin(input), 3));
}

TEST_CASE("for_each_butlast (forward)", "[algorithm]") {
    auto const input = std::forward_list{1, 2, 3, 4};
    auto sum = 0;
    auto [_, it] = stdx::for_each_butlast(std::cbegin(input), std::cend(input),
                                          [&](auto n) { sum += n; });
    CHECK(sum == 6);
    CHECK(it == std::next(std::cbegin(input), 3));
}

TEST_CASE("n-ary for_each_butlastn", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlastn(
        std::cbegin(input), std::cend(input), 2,
        [&](auto... ns) { sum += (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input));
    CHECK(sum == 9);
}

TEST_CASE("n-ary for_each_butlast", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlast(
        std::cbegin(input), std::cend(input),
        [&](auto... ns) { sum += (0 + ... + ns); }, std::cbegin(input),
        std::cbegin(input));
    CHECK(sum == 18);
}

TEST_CASE("for_each_butlast (zero size, random access)", "[algorithm]") {
    auto const input = std::array<int, 0>{};
    auto sum = 0;
    stdx::for_each_butlast(std::cbegin(input), std::cend(input),
                           [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("for_each_butlast (zero size, forward)", "[algorithm]") {
    auto const input = std::forward_list<int>{};
    auto sum = 0;
    stdx::for_each_butlast(std::cbegin(input), std::cend(input),
                           [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("for_each_butlastn (limit == n, random access)", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlastn(std::cbegin(input), std::cend(input), 4,
                            [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("for_each_butlastn (limit == n, forward)", "[algorithm]") {
    auto const input = std::forward_list{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlastn(std::cbegin(input), std::cend(input), 4,
                            [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("for_each_butlastn (limit > n, random access)", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlastn(std::cbegin(input), std::cend(input), 5,
                            [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("for_each_butlastn (limit > n, forward)", "[algorithm]") {
    auto const input = std::forward_list{1, 2, 3, 4};
    auto sum = 0;
    stdx::for_each_butlastn(std::cbegin(input), std::cend(input), 5,
                            [&](auto n) { sum += n; });
    CHECK(sum == 0);
}

TEST_CASE("initial_medial_final", "[algorithm]") {
    auto const input = std::array{1, 2, 3, 4};
    auto first = 0;
    auto sum = 0;
    auto last = 0;
    stdx::initial_medial_final(
        std::cbegin(input), std::cend(input), [&](auto n) { first += n; },
        [&](auto n) { sum += n; }, [&](auto n) { last += n; });
    CHECK(first == 1);
    CHECK(sum == 5);
    CHECK(last == 4);
}

TEST_CASE("initial_medial_final (no body)", "[algorithm]") {
    auto const input = std::array{1, 4};
    auto first = 0;
    auto sum = 0;
    auto last = 0;
    stdx::initial_medial_final(
        std::cbegin(input), std::cend(input), [&](auto n) { first += n; },
        [&](auto n) { sum += n; }, [&](auto n) { last += n; });
    CHECK(first == 1);
    CHECK(sum == 0);
    CHECK(last == 4);
}

TEST_CASE("initial_medial_final (single element)", "[algorithm]") {
    auto const input = std::array{1};
    auto first = 0;
    auto sum = 0;
    auto last = 0;
    stdx::initial_medial_final(
        std::cbegin(input), std::cend(input), [&](auto n) { first += n; },
        [&](auto n) { sum += n; }, [&](auto n) { last += n; });
    CHECK(first == 1);
    CHECK(sum == 0);
    CHECK(last == 0);
}

TEST_CASE("initial_medial_final (empty range)", "[algorithm]") {
    auto const input = std::array<int, 0>{};
    auto first = 0;
    auto sum = 0;
    auto last = 0;
    stdx::initial_medial_final(
        std::cbegin(input), std::cend(input), [&](auto n) { first += n; },
        [&](auto n) { sum += n; }, [&](auto n) { last += n; });
    CHECK(first == 0);
    CHECK(sum == 0);
    CHECK(last == 0);
}

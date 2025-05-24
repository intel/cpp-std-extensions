#include <stdx/rollover.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <type_traits>

TEMPLATE_TEST_CASE("default construction", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    constexpr rollover_t x;
    STATIC_REQUIRE(x == rollover_t{TestType{}});
    CHECK(x == rollover_t{TestType{}});
}

TEMPLATE_TEST_CASE("value construction", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    constexpr rollover_t x{};
    STATIC_REQUIRE(x == rollover_t{TestType{}});
    CHECK(x == rollover_t{TestType{}});
}

TEMPLATE_TEST_CASE("access to underlying value", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    constexpr rollover_t x;
    STATIC_REQUIRE(x.as_underlying() == TestType{});
    CHECK(x.as_underlying() == TestType{});
}

TEMPLATE_TEST_CASE("cast to underlying type", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    constexpr rollover_t x;
    STATIC_REQUIRE(static_cast<typename rollover_t::underlying_t>(x) ==
                   TestType{});
    CHECK(static_cast<typename rollover_t::underlying_t>(x) == TestType{});
}

TEMPLATE_TEST_CASE("construction from convertible integral value", "[rollover]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr stdx::rollover_t<std::uint32_t> x{std::uint8_t{16}};
    STATIC_REQUIRE(x.as_underlying() == TestType{16});
    CHECK(x.as_underlying() == TestType{16});
}

TEMPLATE_TEST_CASE("construction from convertible rollover_t", "[rollover]",
                   std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t) {
    constexpr stdx::rollover_t<std::uint32_t> x{
        stdx::rollover_t<std::uint8_t>{16}};
    STATIC_REQUIRE(x.as_underlying() == TestType{16});
    CHECK(x.as_underlying() == TestType{16});
}

TEST_CASE("conversion with deduction guide", "[rollover]") {
    constexpr auto x = stdx::rollover_t{16u};
    STATIC_REQUIRE(
        std::is_same_v<decltype(x), stdx::rollover_t<unsigned int> const>);
}

TEMPLATE_TEST_CASE("equality", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{1};
    CHECK(x == rollover_t{1});
    CHECK(x != rollover_t{2});
}

TEMPLATE_TEST_CASE("unary plus", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    STATIC_REQUIRE(+rollover_t{1} == rollover_t{1});
    CHECK(+rollover_t{1} == rollover_t{1});
}

TEMPLATE_TEST_CASE("unary minus", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    STATIC_REQUIRE(-rollover_t{1} == rollover_t{-1});
    CHECK(-rollover_t{1} == rollover_t{-1});
}

TEMPLATE_TEST_CASE("increment", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{1};
    CHECK(++x == rollover_t{2});
    CHECK(x++ == rollover_t{2});
    CHECK(x == rollover_t{3});
}

TEMPLATE_TEST_CASE("decrement", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{3};
    CHECK(--x == rollover_t{2});
    CHECK(x-- == rollover_t{2});
    CHECK(x == rollover_t{1});
}

TEMPLATE_TEST_CASE("addition", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{1};
    rollover_t y{1};
    x += y;
    CHECK(x == rollover_t{2});
    CHECK(x + y == rollover_t{3});
}

TEMPLATE_TEST_CASE("subtraction", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{2};
    rollover_t y{1};
    x -= y;
    CHECK(x == rollover_t{1});
    CHECK(x + y == rollover_t{2});
}

TEMPLATE_TEST_CASE("multiplication", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{2};
    rollover_t y{3};
    x *= y;
    CHECK(x == rollover_t{6});
    CHECK(x * y == rollover_t{18});
}

TEMPLATE_TEST_CASE("division", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{12};
    rollover_t y{3};
    x /= y;
    CHECK(x == rollover_t{4});
    CHECK(rollover_t{12} / y == rollover_t{4});
}

TEMPLATE_TEST_CASE("mod operation", "[rollover]", std::uint8_t, std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    rollover_t x{13};
    rollover_t y{3};
    x %= y;
    CHECK(x == rollover_t{1});
    CHECK(rollover_t{13} % y == rollover_t{1});
}

TEMPLATE_TEST_CASE("increment overflow", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    auto x = rollover_t{std::numeric_limits<TestType>::max()};
    ++x;
    CHECK(x == rollover_t{});
}

TEMPLATE_TEST_CASE("decrement underflow", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    auto x = rollover_t{};
    --x;
    CHECK(x == rollover_t{std::numeric_limits<TestType>::max()});
}

TEMPLATE_TEST_CASE("simple comparison", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    CHECK(cmp_less(rollover_t{1}, rollover_t{2}));
}

TEMPLATE_TEST_CASE("comparison at rollover point", "[rollover]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    CHECK(cmp_less(rollover_t{std::numeric_limits<TestType>::max()},
                   rollover_t{}));
    constexpr auto mid =
        rollover_t{std::numeric_limits<TestType>::max() / 2 + 1};
    CHECK(cmp_less(mid, rollover_t{}));
    CHECK(cmp_less(rollover_t{}, mid));
}

TEMPLATE_TEST_CASE("exactly half the values are less", "[rollover]",
                   std::uint8_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    using count_t = std::uint64_t;
    constexpr auto limit = count_t{std::numeric_limits<TestType>::max()} + 1;
    constexpr auto expected = limit / 2;

    auto x = std::array<rollover_t, limit>{};
    std::iota(std::begin(x), std::end(x), rollover_t{});

    for (auto i : x) {
        CHECK(std::count_if(std::begin(x), std::end(x), [&](auto val) {
                  return cmp_less(val, i);
              }) == expected);
    }
}

TEMPLATE_TEST_CASE("chrono duration rep", "[rollover]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    using duration_sec_t = std::chrono::duration<rollover_t>;
    using duration_millisec_t = std::chrono::duration<rollover_t, std::milli>;
    auto d1 = duration_sec_t{rollover_t{3u}};
    CHECK(d1.count() == rollover_t{3u});
    auto d2 = duration_millisec_t{d1};
    CHECK(d2.count() == rollover_t{3000u});
    auto d3 = std::chrono::duration_cast<duration_sec_t>(d2);
    CHECK(d3.count() == rollover_t{3u});
    CHECK((d1 == d2));
    CHECK((d2 == d3));
    CHECK((d2 - d1).count() == rollover_t{});
}

namespace {
struct local_clock {};
} // namespace

TEMPLATE_TEST_CASE("chrono time_point rep", "[rollover]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    using rollover_t = stdx::rollover_t<TestType>;
    using duration_sec_t = std::chrono::duration<rollover_t>;
    using duration_millisec_t = std::chrono::duration<rollover_t, std::milli>;
    using tp_sec_t = std::chrono::time_point<local_clock, duration_sec_t>;
    using tp_millisec_t =
        std::chrono::time_point<local_clock, duration_millisec_t>;
    auto tp1 = tp_sec_t{duration_sec_t{rollover_t{3u}}};
    CHECK(tp1.time_since_epoch().count() == rollover_t{3u});
    auto tp2 = tp_millisec_t{tp1};
    CHECK(tp2.time_since_epoch().count() == rollover_t{3000u});
    CHECK((tp2 - tp1 == duration_sec_t{}));
}

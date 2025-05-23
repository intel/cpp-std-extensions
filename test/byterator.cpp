#include <stdx/byterator.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <iterator>

TEST_CASE("constructible from an iterator", "[byterator]") {
    auto const a = std::array{1, 2, 3, 4};
    auto const b = stdx::byterator{std::begin(a)};
    CHECK(static_cast<void const *>(stdx::to_address(std::begin(a))) ==
          static_cast<void const *>(stdx::to_address(b)));
}

TEST_CASE("equality comparable to iterator", "[byterator]") {
    auto const a = std::array{1, 2, 3, 4};
    auto const b = stdx::byterator{std::begin(a)};
    CHECK((b == std::begin(a)));
    CHECK((std::begin(a) == b));
    CHECK((b != std::end(a)));
    CHECK((std::end(a) != b));
}

TEST_CASE("comparable to iterator", "[byterator]") {
    auto const a = std::array{1, 2, 3, 4};
    auto const b = stdx::byterator{std::begin(a)};
    auto const c = std::next(b);

    CHECK((b < std::end(a)));
    CHECK((std::begin(a) < c));
    CHECK((b <= std::end(a)));
    CHECK((std::begin(a) <= c));
    CHECK((b <= std::begin(a)));
    CHECK((std::begin(a) <= b));

    CHECK((std::end(a) > b));
    CHECK((c > std::begin(a)));
    CHECK((std::end(a) >= b));
    CHECK((c >= std::begin(a)));
    CHECK((b >= std::begin(a)));
    CHECK((std::begin(a) >= b));
}

TEST_CASE("copy constructible", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto const b = stdx::byterator{std::begin(a)};
    auto i = b;
    CHECK((i == b));
}

TEST_CASE("increment and decrement", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto const b = stdx::byterator{std::begin(a)};
    auto i = b;
    CHECK((++i != b));
    CHECK((--i == b));
    auto j = i++;
    CHECK((j == b));
    auto k = i--;
    CHECK((i == j));
    CHECK((k != j));
}

TEST_CASE("random access arithmetic", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto const b = stdx::byterator{std::begin(a)};
    auto i = b;
    CHECK((i + 1 != b));
    i += 1;
    CHECK((i == b + 1));
    CHECK(i - b == 1);
    CHECK((i - 1 == b));
    i -= 1;
    CHECK((i == b));
}

TEST_CASE("advance", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto const b = stdx::byterator{std::begin(a)};
    auto i = b;
    CHECK((i + 1 != b));
    i.advance();
    CHECK((i == b + 1));
    CHECK(i - b == 1);
    CHECK((i - 1 == b));
    i.advance(-1);
    CHECK((i == b));
    STATIC_REQUIRE(std::is_same_v<decltype(i.advance()),
                                  stdx::byterator<std::uint16_t const> &>);
}

TEST_CASE("equality comparable", "[byterator]") {
    auto const a = std::array{1, 2, 3, 4};
    auto x = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(stdx::equality_comparable<decltype(x)>);
    auto y = x;
    CHECK(x == y);
    ++y;
    CHECK(x != y);
}

TEST_CASE("totally ordered", "[byterator]") {
    auto const a = std::array{1, 2, 3, 4};
    auto x = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(stdx::totally_ordered<decltype(x)>);
    auto y = std::next(x);
    CHECK(x < y);
    CHECK(x <= y);
    CHECK(y > x);
    CHECK(y >= x);
}

TEST_CASE("dereference to byte const& (read)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    CHECK(*i++ == std::byte{1});
    CHECK(*i++ == std::byte{2});
    CHECK(*i++ == std::byte{3});
    CHECK(*i++ == std::byte{4});
    CHECK((i == std::end(a)));
}

TEST_CASE("dereference to byte & (write)", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    *i = std::byte{3};
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0302));
}

TEST_CASE("random access indexing (read)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto const b = stdx::byterator{std::begin(a)};
    auto i = b;
    CHECK(i[2] == std::byte{3});
}

TEST_CASE("random access indexing (write)", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto b = stdx::byterator{std::begin(a)};
    b[2] = std::byte{5};
    CHECK(a[1] == stdx::to_be<std::uint16_t>(0x0504));
}

TEST_CASE("peek uint8_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu8()), std::uint8_t>);
    CHECK(i.peeku8() == 1);
    CHECK((i == std::begin(a)));
}

TEST_CASE("advance uint8_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    i.advanceu8();
    CHECK((i == j));
}

TEST_CASE("read uint8_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu8()), std::uint8_t>);
    CHECK(i.readu8() == 1);
    CHECK((i == j));
}

TEST_CASE("write uint8_t", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    i.writeu8(3);
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0302));
    CHECK((i == j));
}

TEST_CASE("peek uint16_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu16()), std::uint16_t>);
    CHECK(i.peeku16() == stdx::to_be<std::uint16_t>(0x0102));
    CHECK((i == std::begin(a)));
}

TEST_CASE("advance uint16_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = i + 2;
    i.advanceu16();
    CHECK((i == j));
}

TEST_CASE("read uint16_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = i + 2;
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu16()), std::uint16_t>);
    CHECK(i.readu16() == stdx::to_be<std::uint16_t>(0x0102));
    CHECK((i == j));
}

TEST_CASE("write uint16_t", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = i + 2;
    i.writeu16(3);
    CHECK(a[0] == 3);
    CHECK((i == j));
}

TEST_CASE("peek uint32_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.peeku32() == stdx::to_be<std::uint32_t>(0x01020304));
    CHECK((i == std::begin(a)));
}

TEST_CASE("advance uint32_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    i.advanceu32();
    CHECK((i == std::end(a)));
}

TEST_CASE("read uint32_t", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.readu32() == stdx::to_be<std::uint32_t>(0x01020304));
    CHECK((i == std::end(a)));
}

TEST_CASE("write uint32_t", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    i.writeu32(stdx::to_be<std::uint32_t>(0x05060708));
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0506));
    CHECK(a[1] == stdx::to_be<std::uint16_t>(0x0708));
    CHECK((i == std::end(a)));
}

TEST_CASE("peek uint64_t", "[byterator]") {
    auto const a = std::array{
        stdx::to_be<std::uint16_t>(0x0102), stdx::to_be<std::uint16_t>(0x0304),
        stdx::to_be<std::uint16_t>(0x0506), stdx::to_be<std::uint16_t>(0x0708)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu64()), std::uint64_t>);
    CHECK(i.peeku64() == stdx::to_be<std::uint64_t>(0x0102030405060708));
    CHECK((i == std::begin(a)));
}

TEST_CASE("advance uint64_t", "[byterator]") {
    auto const a = std::array{
        stdx::to_be<std::uint16_t>(0x0102), stdx::to_be<std::uint16_t>(0x0304),
        stdx::to_be<std::uint16_t>(0x0506), stdx::to_be<std::uint16_t>(0x0708)};
    auto i = stdx::byterator{std::begin(a)};
    i.advanceu64();
    CHECK((i == std::end(a)));
}

TEST_CASE("read uint64_t", "[byterator]") {
    auto const a = std::array{
        stdx::to_be<std::uint16_t>(0x0102), stdx::to_be<std::uint16_t>(0x0304),
        stdx::to_be<std::uint16_t>(0x0506), stdx::to_be<std::uint16_t>(0x0708)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu64()), std::uint64_t>);
    CHECK(i.readu64() == stdx::to_be<std::uint64_t>(0x0102030405060708));
    CHECK((i == std::end(a)));
}

TEST_CASE("write uint64_t", "[byterator]") {
    auto a = std::array{
        stdx::to_be<std::uint16_t>(0x0102), stdx::to_be<std::uint16_t>(0x0304),
        stdx::to_be<std::uint16_t>(0x0506), stdx::to_be<std::uint16_t>(0x0708)};
    auto i = stdx::byterator{std::begin(a)};
    i.writeu64(stdx::to_be<std::uint64_t>(0x05060708090A0B0C));
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0506));
    CHECK(a[1] == stdx::to_be<std::uint16_t>(0x0708));
    CHECK(a[2] == stdx::to_be<std::uint16_t>(0x090A));
    CHECK(a[3] == stdx::to_be<std::uint16_t>(0x0B0C));
    CHECK((i == std::end(a)));
}

namespace {
enum struct E : std::uint8_t { A = 1, B = 2, C = 3 };
}

TEST_CASE("peek enum", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.peek<E>() == E::A);
}

TEST_CASE("advance enum", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    i.advance<E>();
    CHECK(i == j);
}

TEST_CASE("read enum", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.read<E>() == E::A);
}

TEST_CASE("write enum", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    i.write(E::C);
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0302));
}

namespace {
enum struct E2 : std::uint32_t { A = 1, B = 2, C = 3 };
}

TEST_CASE("peek enum (constrained size)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.peek<std::uint8_t, E2>() == E2::A);
}

TEST_CASE("read enum (constrained size)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.read<std::uint8_t, E2>() == E2::A);
    CHECK((i == j));
}

TEST_CASE("write enum (constrained size)", "[byterator]") {
    auto a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                        stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    i.writeu8(E::C);
    CHECK(a[0] == stdx::to_be<std::uint16_t>(0x0302));
    CHECK((i == j));
}

TEST_CASE("peek enum (constrained size alias)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.peeku8<E2>() == E2::A);
}

TEST_CASE("read enum (constrained size alias)", "[byterator]") {
    auto const a = std::array{stdx::to_be<std::uint16_t>(0x0102),
                              stdx::to_be<std::uint16_t>(0x0304)};
    auto i = stdx::byterator{std::begin(a)};
    auto j = std::next(i);
    STATIC_REQUIRE(std::is_same_v<decltype(i.readu32()), std::uint32_t>);
    CHECK(i.readu8<E2>() == E2::A);
    CHECK((i == j));
}

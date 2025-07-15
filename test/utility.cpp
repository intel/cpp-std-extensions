#include <stdx/utility.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <string_view>
#include <type_traits>

TEST_CASE("forward_like", "[utility]") {
    STATIC_REQUIRE(std::is_same_v<stdx::forward_like_t<int, float>, float &&>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::forward_like_t<int const, float>, float const &&>);

    STATIC_REQUIRE(
        std::is_same_v<stdx::forward_like_t<int &&, float>, float &&>);
    STATIC_REQUIRE(std::is_same_v<stdx::forward_like_t<int const &&, float>,
                                  float const &&>);

    STATIC_REQUIRE(std::is_same_v<stdx::forward_like_t<int &, float>, float &>);
    STATIC_REQUIRE(std::is_same_v<stdx::forward_like_t<int const &, float>,
                                  float const &>);
}

TEST_CASE("as_unsigned (changed)", "[utility]") {
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_unsigned(std::int8_t{})),
                                  std::uint8_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_unsigned(std::int16_t{})),
                                  std::uint16_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_unsigned(std::int32_t{})),
                                  std::uint32_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_unsigned(std::int64_t{})),
                                  std::uint64_t>);
    STATIC_REQUIRE(stdx::as_unsigned(std::int8_t{17}) == 17);
}

TEMPLATE_TEST_CASE("as_unsigned (unchanged)", "[utility]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(
        std::is_same_v<decltype(stdx::as_unsigned(TestType{})), TestType>);
    STATIC_REQUIRE(stdx::as_unsigned(TestType{17}) == 17);
}

TEST_CASE("as_signed (changed)", "[utility]") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(stdx::as_signed(std::uint8_t{})), std::int8_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_signed(std::uint16_t{})),
                                  std::int16_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_signed(std::uint32_t{})),
                                  std::int32_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(stdx::as_signed(std::uint64_t{})),
                                  std::int64_t>);
    STATIC_REQUIRE(stdx::as_signed(std::uint8_t{17}) == 17);
}

TEMPLATE_TEST_CASE("as_signed (unchanged)", "[utility]", std::int8_t,
                   std::int16_t, std::int32_t, std::int64_t) {
    STATIC_REQUIRE(
        std::is_same_v<decltype(stdx::as_signed(TestType{})), TestType>);
    STATIC_REQUIRE(stdx::as_signed(TestType{17}) == 17);
}

TEMPLATE_TEST_CASE("sized<T> in (uint8_t zero/one/two case)", "[utility]",
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{0}.in<TestType>() == 0);
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{1}.in<TestType>() == 1);
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (uint16_t zero/one/two case)", "[utility]",
                   std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{0}.in<TestType>() == 0);
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{1}.in<TestType>() == 1);
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (uint32_t zero/one/two case)", "[utility]",
                   std::uint64_t) {
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{0}.in<TestType>() == 0);
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{1}.in<TestType>() == 1);
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{2}.in<TestType>() == 1);
}

TEMPLATE_TEST_CASE("sized<T> in (same type)", "[utility]", std::uint8_t,
                   std::uint16_t, std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(stdx::sized<TestType>{3}.template in<TestType>() == 3);
}

TEMPLATE_TEST_CASE("sized<T> in (exact case)", "[utility]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(
        stdx::sized<std::uint8_t>{3 * sizeof(TestType)}.in<TestType>() == 3);
}

TEMPLATE_TEST_CASE("sized<T> in defaults to bytes", "[utility]", std::uint16_t,
                   std::uint32_t, std::uint64_t) {
    STATIC_REQUIRE(stdx::sized<TestType>{3}.in() == 3 * sizeof(TestType));
}

TEMPLATE_TEST_CASE("sized<T> in (downsize)", "[utility]", std::uint8_t,
                   std::uint16_t, std::uint32_t) {
    STATIC_REQUIRE(stdx::sized<std::uint64_t>{3}.in<TestType>() ==
                   3 * sizeof(std::uint64_t) / sizeof(TestType));
}

TEST_CASE("sized<T> in (upsize uint8_t)", "[utility]") {
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{3}.in<std::uint16_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{17}.in<std::uint16_t>() == 9);

    STATIC_REQUIRE(stdx::sized<std::uint8_t>{5}.in<std::uint32_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{17}.in<std::uint32_t>() == 5);

    STATIC_REQUIRE(stdx::sized<std::uint8_t>{9}.in<std::uint64_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint8_t>{17}.in<std::uint64_t>() == 3);
}

TEST_CASE("sized<T> in (upsize uint16_t)", "[utility]") {
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{3}.in<std::uint32_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{17}.in<std::uint32_t>() == 9);

    STATIC_REQUIRE(stdx::sized<std::uint16_t>{5}.in<std::uint64_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint16_t>{17}.in<std::uint64_t>() == 5);
}

TEST_CASE("sized<T> in (upsize uint32_t)", "[utility]") {
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{3}.in<std::uint64_t>() == 2);
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{17}.in<std::uint64_t>() == 9);
}

TEST_CASE("sized<T> aliases", "[utility]") {
    STATIC_REQUIRE(stdx::sized8{1}.in<std::uint64_t>() == 1);
    STATIC_REQUIRE(stdx::sized16{1}.in<std::uint64_t>() == 1);
    STATIC_REQUIRE(stdx::sized32{1}.in<std::uint64_t>() == 1);
    STATIC_REQUIRE(stdx::sized64{1}.in<std::uint64_t>() == 1);
}

TEST_CASE("sized<T> in (downsize not divisible)", "[utility]") {
    using T = std::array<char, 3>;
    STATIC_REQUIRE(sizeof(T) == 3);
    STATIC_REQUIRE(stdx::sized<std::uint32_t>{2}.in<T>() == 3);
}

TEST_CASE("sized<T> in (upsize not divisible)", "[utility]") {
    using T = std::array<char, 3>;
    STATIC_REQUIRE(sizeof(T) == 3);
    STATIC_REQUIRE(stdx::sized<T>{3}.in<std::uint32_t>() == 3);
}

TEST_CASE("sized<T> in (downsize, mod > 1)", "[utility]") {
    using T = std::array<char, 6>;
    using U = std::array<char, 4>;
    STATIC_REQUIRE(stdx::sized<T>{1}.in<U>() == 2);
}

TEST_CASE("sized<T> in (upsize, mod > 1)", "[utility]") {
    using T = std::array<char, 6>;
    using U = std::array<char, 4>;
    STATIC_REQUIRE(stdx::sized<U>{2}.in<T>() == 2);
}

TEST_CASE("CX_VALUE structural value", "[utility]") {
    auto x = CX_VALUE(42);
    STATIC_REQUIRE(x() == 42);
    auto y = CX_VALUE(17, 42);
    STATIC_REQUIRE(y() == 42);
}

TEST_CASE("CX_VALUE non-structural value", "[utility]") {
    auto x = CX_VALUE(std::string_view{"Hello"});
    STATIC_REQUIRE(x() == std::string_view{"Hello"});
}

TEST_CASE("CX_VALUE type", "[utility]") {
    auto x = CX_VALUE(int);
    STATIC_REQUIRE(std::is_same_v<decltype(x()), stdx::type_identity<int>>);
    auto y = CX_VALUE(std::pair<int, int>);
    STATIC_REQUIRE(std::is_same_v<decltype(y()),
                                  stdx::type_identity<std::pair<int, int>>>);
}

namespace {
template <std::size_t S> constexpr auto cx_value_test_nttp() {
    return CX_VALUE(S);
}

template <typename T> constexpr auto cx_value_test_type() {
    return CX_VALUE(T);
}
} // namespace

TEST_CASE("CX_VALUE on NTTP", "[utility]") {
    auto x = cx_value_test_nttp<42>();
    STATIC_REQUIRE(x() == 42);
}

TEST_CASE("CX_VALUE on type template argument", "[utility]") {
    auto x = cx_value_test_type<int>();
    STATIC_REQUIRE(std::is_same_v<decltype(x()), stdx::type_identity<int>>);
}

namespace {
struct alignas(16) over_aligned {};
} // namespace

TEST_CASE("is_aligned_with (integral)", "[utility]") {
    STATIC_REQUIRE(stdx::is_aligned_with<std::uint8_t>(0b1111));

    STATIC_REQUIRE(stdx::is_aligned_with<std::uint16_t>(0b1110));
    STATIC_REQUIRE(not stdx::is_aligned_with<std::uint16_t>(0b1111));

    STATIC_REQUIRE(stdx::is_aligned_with<std::uint32_t>(0b1100));
    STATIC_REQUIRE(not stdx::is_aligned_with<std::uint32_t>(0b1110));

    STATIC_REQUIRE(stdx::is_aligned_with<std::uint64_t>(0b1000));
    STATIC_REQUIRE(not stdx::is_aligned_with<std::uint64_t>(0b1100));

    STATIC_REQUIRE(stdx::is_aligned_with<over_aligned>(0b1'0000));
    STATIC_REQUIRE(not stdx::is_aligned_with<over_aligned>(0b1000));
}

TEST_CASE("is_aligned_with (pointer)", "[utility]") {
    std::int32_t i;
    auto p = &i;
    auto pc = reinterpret_cast<std::int8_t *>(p);
    ++pc;

    CHECK(stdx::is_aligned_with<std::uint8_t>(pc));
    CHECK(not stdx::is_aligned_with<std::uint16_t>(pc));

    CHECK(stdx::is_aligned_with<std::uint8_t>(p));
    CHECK(stdx::is_aligned_with<std::uint16_t>(p));
    CHECK(stdx::is_aligned_with<std::uint32_t>(p));
}

#if __cplusplus >= 202002L

TEST_CASE("ct (integral)", "[utility]") {
    constexpr auto vs = stdx::ct<42>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(vs), std::integral_constant<int, 42> const>);
    constexpr auto vu = stdx::ct<42u>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(vu),
                       std::integral_constant<unsigned int, 42> const>);
}

TEST_CASE("ct (bool)", "[utility]") {
    constexpr auto v = stdx::ct<true>();
    STATIC_REQUIRE(std::is_same_v<decltype(v), std::bool_constant<true> const>);
}

TEST_CASE("ct (char)", "[utility]") {
    constexpr auto v = stdx::ct<'A'>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(v), std::integral_constant<char, 'A'> const>);
}

namespace {
enum struct E { A, B, C };
}

TEST_CASE("ct (enum)", "[utility]") {
    constexpr auto v = stdx::ct<E::A>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(v), std::integral_constant<E, E::A> const>);
}

TEST_CASE("ct (type)", "[utility]") {
    constexpr auto v = stdx::ct<int>();
    STATIC_REQUIRE(std::is_same_v<decltype(v), stdx::type_identity<int> const>);
}

TEST_CASE("is_ct", "[utility]") {
    constexpr auto x1 = stdx::ct<42>();
    STATIC_REQUIRE(stdx::is_ct_v<decltype(x1)>);
    constexpr auto x2 = stdx::ct<int>();
    STATIC_REQUIRE(stdx::is_ct_v<decltype(x2)>);
}

TEST_CASE("CT_WRAP", "[utility]") {
    auto x1 = 17;
    STATIC_REQUIRE(std::is_same_v<decltype(CT_WRAP(x1)), int>);
    CHECK(CT_WRAP(x1) == 17);

    auto x2 = stdx::ct<17>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(CT_WRAP(x2)), std::integral_constant<int, 17>>);
    STATIC_REQUIRE(CT_WRAP(x2).value == 17);

    auto const x3 = 17;
    STATIC_REQUIRE(
        std::is_same_v<decltype(CT_WRAP(x3)), std::integral_constant<int, 17>>);
    STATIC_REQUIRE(CT_WRAP(x3).value == 17);

    constexpr static auto x4 = 17;
    STATIC_REQUIRE(
        std::is_same_v<decltype(CT_WRAP(x4)), std::integral_constant<int, 17>>);
    STATIC_REQUIRE(CT_WRAP(x4).value == 17);

    []<auto X>() {
        STATIC_REQUIRE(std::is_same_v<decltype(CT_WRAP(X)),
                                      std::integral_constant<int, 17>>);
        STATIC_REQUIRE(CT_WRAP(X).value == 17);
    }.template operator()<17>();
}

TEST_CASE("CX_WRAP integer runtime arg", "[utility]") {
    auto x = 17;
    STATIC_REQUIRE(std::is_same_v<decltype(CX_WRAP(x)), int>);
    CHECK(CX_WRAP(x) == 17);
}

TEST_CASE("CX_WRAP string_view runtime arg", "[utility]") {
    auto x = std::string_view{"hello"};
    STATIC_REQUIRE(std::is_same_v<decltype(CX_WRAP(x)), std::string_view>);
    CHECK(CX_WRAP(x) == std::string_view{"hello"});
}

TEST_CASE("CX_WRAP const integral type", "[utility]") {
    auto const x = 17;
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(x))>);
    STATIC_REQUIRE(CX_WRAP(x)() == 17);
}

TEST_CASE("CX_WRAP constexpr integral type", "[utility]") {
    constexpr auto x = 17;
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(x))>);
    STATIC_REQUIRE(CX_WRAP(x)() == 17);
}

TEST_CASE("CX_WRAP constexpr non-structural type", "[utility]") {
    constexpr static auto x = std::string_view{"hello"};
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(x))>);
    STATIC_REQUIRE(CX_WRAP(x)() == std::string_view{"hello"});
}

TEST_CASE("CX_WRAP integer literal", "[utility]") {
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(17))>);
    STATIC_REQUIRE(CX_WRAP(17)() == 17);
}

TEST_CASE("CX_WRAP string literal", "[utility]") {
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP("hello"))>);
    STATIC_REQUIRE(CX_WRAP("hello")() == std::string_view{"hello"});
}

TEST_CASE("CX_WRAP existing CX_VALUE", "[utility]") {
    auto x = CX_VALUE(17);
    STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(x))>);
    STATIC_REQUIRE(CX_WRAP(x)() == 17);
}

TEST_CASE("CX_WRAP template argument", "[utility]") {
    []<int x> {
        STATIC_REQUIRE(stdx::is_cx_value_v<decltype(CX_WRAP(x))>);
        STATIC_REQUIRE(CX_WRAP(x)() == 17);
    }.template operator()<17>();
}

TEST_CASE("CX_WRAP type argument", "[utility]") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(CX_WRAP(int)), stdx::type_identity<int>>);
}

TEST_CASE("CX_WRAP empty type argument", "[utility]") {
    using X = std::integral_constant<int, 17>;
    STATIC_REQUIRE(
        std::is_same_v<decltype(CX_WRAP(X)), stdx::type_identity<X>>);
}

TEST_CASE("CX_WRAP integral_constant arg", "[utility]") {
    auto x = std::integral_constant<int, 17>{};
    STATIC_REQUIRE(std::is_same_v<decltype(CX_WRAP(x)), decltype(x)>);
    CHECK(CX_WRAP(x)() == 17);
}

#ifdef __clang__
namespace {
struct expression_test {
    int f(int x) { return x; }
};
} // namespace

TEST_CASE("CX_WRAP non-constexpr expression", "[utility]") {
    auto x = 17;
    STATIC_REQUIRE(
        std::is_same_v<decltype(CX_WRAP(expression_test{}.f(x))), int>);
    CHECK(CX_WRAP(expression_test{}.f(x)) == 17);
}
#endif

#endif

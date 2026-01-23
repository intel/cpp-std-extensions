#include "detail/tuple_types.hpp"

#include <stdx/ct_format.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <limits>
#include <string_view>
#include <type_traits>

using namespace stdx::ct_string_literals;

TEST_CASE("detect string format specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_CHECK(stdx::detail::count_specifiers("{}"sv) == 1u);
    STATIC_CHECK(stdx::detail::count_specifiers("{} {}"sv) == 2u);
    STATIC_CHECK(stdx::detail::count_specifiers("{"sv) == 0u);
    STATIC_CHECK(stdx::detail::count_specifiers("{{"sv) == 0u);
    STATIC_CHECK(stdx::detail::count_specifiers("{{{}"sv) == 1u);
    STATIC_CHECK(stdx::detail::count_specifiers("{{}}"sv) == 0u);
    STATIC_CHECK(stdx::detail::count_specifiers("{{{{"sv) == 0u);
}

TEST_CASE("split format string by specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    using SS = stdx::detail::split_spec;
    STATIC_CHECK(stdx::detail::split_specifiers<1>("hello"sv) ==
                 std::array{SS{"hello"sv, 5}});
    STATIC_CHECK(stdx::detail::split_specifiers<2>("{}"sv) ==
                 std::array{SS{"{}"sv, 0}, SS{""sv, 0}});
    STATIC_CHECK(stdx::detail::split_specifiers<3>("{} {}"sv) ==
                 std::array{SS{"{}"sv, 0}, SS{" {}"sv, 1}, SS{""sv, 0}});
    STATIC_CHECK(stdx::detail::split_specifiers<2>("{{{}"sv) ==
                 std::array{SS{"{{{}"sv, 2}, SS{""sv, 0}});
    STATIC_CHECK(stdx::detail::split_specifiers<2>("{} hello"sv) ==
                 std::array{SS{"{}"sv, 0}, SS{" hello"sv, 6}});
}

TEST_CASE("extract name and format string", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_CHECK(stdx::detail::extract_format1_str<"{}", 0>() ==
                 std::pair{"{}"_cts, ""_cts});
    STATIC_CHECK(stdx::detail::extract_format1_str<"{:}", 0>() ==
                 std::pair{"{:}"_cts, ""_cts});
    STATIC_CHECK(stdx::detail::extract_format1_str<"abc{}", 3>() ==
                 std::pair{"abc{}"_cts, ""_cts});
    STATIC_CHECK(stdx::detail::extract_format1_str<"abc{xyz:}", 3>() ==
                 std::pair{"abc{:}"_cts, "xyz"_cts});
    STATIC_CHECK(stdx::detail::extract_format1_str<"abc{xyz}", 3>() ==
                 std::pair{"abc{}"_cts, "xyz"_cts});
}

TEST_CASE("format a static string", "[ct_format]") {
    STATIC_CHECK(stdx::ct_format<"Hello">() == "Hello"_fmt_res);
}

TEST_CASE("format a compile-time stringish argument (CX_VALUE)",
          "[ct_format]") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE("world"sv)) ==
                   "Hello world"_fmt_res);
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE("world"_cts)) ==
                   "Hello world"_fmt_res);
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE("world")) ==
                   "Hello world"_fmt_res);
}

TEST_CASE("format a compile-time stringish argument (ct)", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">("world"_ctst) ==
                   "Hello world"_fmt_res);
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(stdx::ct<"world">()) ==
                   "Hello world"_fmt_res);
}

TEST_CASE("format a compile-time integral argument (CX_VALUE)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE(42)) ==
                   "Hello 42"_fmt_res);
}

TEST_CASE("format a negative compile-time integral argument (CX_VALUE)",
          "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE(-42)) ==
                   "Hello -42"_fmt_res);
}

TEST_CASE("format most negative compile-time integral argument (CX_VALUE)",
          "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(
                       CX_VALUE(std::numeric_limits<int>::min())) ==
                   "Hello -2147483648"_fmt_res);
}

TEST_CASE("format zero (CX_VALUE)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE(0)) ==
                   "Hello 0"_fmt_res);
}

TEST_CASE("format a char (CX_VALUE)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}orld">(CX_VALUE('w')) ==
                   "Hello world"_fmt_res);
}

#ifndef STDX_FREESTANDING
namespace move_test {
struct move_only {
    constexpr move_only() = default;
    constexpr move_only(int x) : value{x} {}
    constexpr move_only(move_only &&) = default;
    constexpr auto operator=(move_only &&) noexcept -> move_only & = default;

    friend constexpr auto operator==(move_only const &, move_only const &)
        -> bool = default;

    int value;
};

[[nodiscard]] constexpr auto format_as(move_only const &) -> std::string_view {
    return "17";
}
} // namespace move_test

TEST_CASE("format a move-only argument (CX_VALUE)", "[ct_format]") {
    STATIC_CHECK(stdx::ct_format<"Hello {}">(
                     CX_VALUE(move_test::move_only{17})) == "Hello 17"_fmt_res);
}
#endif

TEST_CASE("format a compile-time integral argument (ct)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(stdx::ct<42>()) ==
                   "Hello 42"_fmt_res);
}

TEST_CASE("format a type argument (CX_VALUE)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE(int)) ==
                   "Hello int"_fmt_res);
}

TEST_CASE("format a type argument (ct)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(stdx::ct<int>()) ==
                   "Hello int"_fmt_res);
}

TEST_CASE("format a compile-time argument with different base", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello 0x{:x}">(CX_VALUE(42)) ==
                   "Hello 0x2a"_fmt_res);
}

#ifndef STDX_FREESTANDING
TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
                   "Hello **0x2a"_fmt_res);
}
#endif

namespace {
enum struct E { A };
}

TEST_CASE("format a compile-time enum argument (CX_VALUE)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(CX_VALUE(E::A)) ==
                   "Hello A"_fmt_res);
}

TEST_CASE("format a compile-time enum argument (ct)", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(stdx::ct<E::A>()) ==
                   "Hello A"_fmt_res);
}

TEST_CASE("format multiple compile-time arguments", "[ct_format]") {
    STATIC_CHECK(
        stdx::ct_format<"Hello {} {}">(CX_VALUE(E::A), CX_VALUE("world")) ==
        "Hello A world"_fmt_res);
}

TEST_CASE("format a runtime argument", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::make_format_result("Hello {}"_ctst, stdx::make_tuple(x));
    CHECK(stdx::ct_format<"Hello {}">(x) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {}">(x) == expected);
}

TEST_CASE("format a move-only runtime argument", "[ct_format]") {
    constexpr auto expected =
        stdx::make_format_result("Hello {}"_ctst, stdx::tuple{move_only{17}});
    CHECK(stdx::ct_format<"Hello {}">(move_only{17}) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {}">(move_only{17}) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::make_format_result("Hello int {}"_ctst, stdx::make_tuple(x));

    CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    STATIC_REQUIRE(
        stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) ==
        stdx::make_format_result("Hello {} int"_ctst, stdx::make_tuple(42)));
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    STATIC_CHECK(
        stdx::ct_format<"Hello {} {}">(42, 17) ==
        stdx::make_format_result("Hello {} {}"_ctst, stdx::make_tuple(42, 17)));
}

TEST_CASE("format multiple mixed arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;
    constexpr auto expected = stdx::make_format_result(
        "Hello {} A {} int world"_ctst, stdx::make_tuple(42, "B"sv));

    CHECK(stdx::ct_format<"Hello {} {} {} {} world">(
              42, CX_VALUE("A"sv), b, CX_VALUE(int)) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {} {} {} world">(
                     42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) == expected);
}

TEST_CASE("format a format result", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"The value is {}.">(
                       stdx::ct_format<"(year={})">(2022)) ==
                   stdx::make_format_result("The value is (year={})."_ctst,
                                            stdx::make_tuple(2022)));
}

TEST_CASE("format an empty format result", "[ct_format]") {
    STATIC_CHECK(stdx::ct_format<"The value is {}.">("2022"_fmt_res) ==
                 "The value is 2022."_fmt_res);
}

namespace {
template <stdx::ct_string> constexpr auto conversion_success = true;
} // namespace

TEST_CASE("empty format_result can implicitly convert to ct_string",
          "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_CHECK(
        stdx::detail::format_convertible<decltype(stdx::ct_format<"Hello">())>);
    STATIC_CHECK(stdx::detail::format_convertible<
                 decltype(stdx::ct_format<"Hello {}">("world"_ctst))>);
    STATIC_CHECK(not stdx::detail::format_convertible<
                 decltype(stdx::ct_format<"Hello {}">(42))>);

    STATIC_CHECK(conversion_success<stdx::ct_format<"Hello">()>);
}

TEST_CASE("empty format_result can explicitly convert to ct_string",
          "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_CHECK(+stdx::ct_format<"Hello">() == "Hello"_cts);
}

namespace {
template <typename T, T...> struct string_constant {
  private:
    friend constexpr auto operator==(string_constant const &,
                                     string_constant const &) -> bool = default;
};

template <class T, T... Ls, T... Rs>
[[nodiscard]] constexpr auto operator+(string_constant<T, Ls...>,
                                       string_constant<T, Rs...>) noexcept
    -> string_constant<T, Ls..., Rs...> {
    return {};
}
} // namespace

TEST_CASE("format_to a different type (compile-time value)", "[ct_format]") {
    using namespace std::string_view_literals;
    constexpr auto expected =
        stdx::make_format_result(string_constant<char, 'A'>{});
    STATIC_CHECK(stdx::ct_format<"{}", string_constant>(CX_VALUE("A"sv)) ==
                 expected);
}

TEST_CASE("format_to a different type (runtime value)", "[ct_format]") {
    using namespace std::string_view_literals;

    constexpr auto expected = stdx::make_format_result(
        string_constant<char, '{', '}'>{}, stdx::tuple{17});

    CHECK(stdx::ct_format<"{}", string_constant>(17) == expected);
    STATIC_CHECK(stdx::ct_format<"{}", string_constant>(17) == expected);
}

TEST_CASE("format a string-type argument", "[ct_format]") {
    STATIC_CHECK(stdx::ct_format<"Hello {}!">(string_constant<char, 'A'>{}) ==
                 "Hello A!"_fmt_res);
}

TEST_CASE("format a format result with different type", "[ct_format]") {
    constexpr auto expected = stdx::make_format_result(
        string_constant<char, 'A', 'B', '{', '}', 'C', 'D'>{},
        stdx::tuple{2022});

    STATIC_CHECK(stdx::ct_format<"A{}D", string_constant>(
                     stdx::ct_format<"B{}C", string_constant>(2022)) ==
                 expected);
}

TEST_CASE("format multiple mixed arguments with different type",
          "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;

    constexpr auto expected = stdx::make_format_result(
        stdx::ct_string_to_type<"Hello {} A {} int world"_cts,
                                string_constant>(),
        stdx::tuple{42, "B"sv});

    CHECK(stdx::ct_format<"Hello {} {} {} {} world", string_constant>(
              42, CX_VALUE("A"sv), b, CX_VALUE(int)) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {} {} {} world", string_constant>(
                     42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) == expected);
}

TEST_CASE("num fmt specifiers", "[ct_format]") {
    STATIC_REQUIRE(stdx::num_fmt_specifiers<"{}"> == 1u);
    STATIC_REQUIRE(stdx::num_fmt_specifiers<"{} {}"> == 2u);
}

namespace user {
struct S {
    int x;
};

constexpr auto ct_format_as(S const &s) {
    return stdx::ct_format<"S: {}">(s.x);
}
} // namespace user

TEST_CASE("user-defined formatting", "[ct_format]") {
    constexpr auto expected =
        stdx::make_format_result("Hello S: {}"_ctst, stdx::tuple{17});

    auto r = stdx::ct_format<"Hello {}">(user::S{17});
    CHECK(r == expected);
}

TEST_CASE("FORMAT with no arguments", "[ct_format]") {
    STATIC_CHECK(STDX_CT_FORMAT("Hello") == "Hello"_fmt_res);
}

TEST_CASE("FORMAT a compile-time string argument", "[ct_format]") {
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", "world") == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT a compile-time int argument", "[ct_format]") {
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", 17) == "Hello 17"_fmt_res);
}

TEST_CASE("FORMAT a type argument", "[ct_format]") {
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", int) == "Hello int"_fmt_res);
}

TEST_CASE("FORMAT a constexpr ct_string argument", "[ct_format]") {
    constexpr static auto S = "world"_cts;
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT a cts_t argument", "[ct_format]") {
    auto S = "world"_ctst;
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT a format_result argument", "[ct_format]") {
    constexpr auto expected = stdx::make_format_result(
        "The value is (year={})."_ctst, stdx::tuple{2022});

    auto S = stdx::ct_format<"(year={})">(2022);
    CHECK(STDX_CT_FORMAT("The value is {}.", S) == expected);
}

TEST_CASE("FORMAT an empty format_result argument", "[ct_format]") {
    auto S = "world"_fmt_res;
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT a constexpr int argument", "[ct_format]") {
    constexpr static auto I = 17;
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", I) == "Hello 17"_fmt_res);
}

#ifdef __clang__
TEST_CASE("FORMAT a constexpr nonstatic string_view argument", "[ct_format]") {
    constexpr auto S = std::string_view{"world"};
    constexpr auto expected =
        stdx::make_format_result("Hello {}"_ctst, stdx::make_tuple(S));
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == expected);
}
#endif

TEST_CASE("FORMAT a constexpr string_view argument", "[ct_format]") {
    constexpr static auto S = std::string_view{"world"};
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT an integral_constant argument", "[ct_format]") {
    auto I = std::integral_constant<unsigned int, 17u>{};
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", I) == "Hello 17"_fmt_res);
}

#ifdef __clang__
namespace {
struct expression_test {
    int f(int x) { return x; }
};
} // namespace

TEST_CASE("FORMAT non-constexpr expression", "[utility]") {
    auto x = 17;
    constexpr auto expected =
        stdx::make_format_result("Hello {}"_ctst, stdx::make_tuple(17));
    CHECK(STDX_CT_FORMAT("Hello {}", expression_test{}.f(x)) == expected);
}
#endif

TEST_CASE("format a named runtime argument", "[ct_format]") {
    constexpr auto x = 17;

    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 0>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {}"_ctst, stdx::make_tuple(x));

    CHECK(stdx::ct_format<"Hello {x}">(x) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {x}">(x) == expected);
}

TEST_CASE("format two named runtime arguments", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto y = 18;

    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 0>,
                                             stdx::named_arg<"y", int, 1>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {} {}"_ctst, stdx::make_tuple(x, y));

    CHECK(stdx::ct_format<"Hello {x} {y}">(x, y) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {x} {y}">(x, y) == expected);
}

TEST_CASE("format two runtime arguments, only one named", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto y = 18;

    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 1>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {} {}"_ctst, stdx::make_tuple(x, y));

    CHECK(stdx::ct_format<"Hello {} {y}">(x, y) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {y}">(x, y) == expected);
}

TEST_CASE("format a named compile-time argument", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 6, 8>>;
    constexpr auto expected =
        stdx::make_format_result<expected_named_t>("Hello 17"_ctst);

    STATIC_CHECK(STDX_CT_FORMAT("Hello {x}", 17) == expected);
}

TEST_CASE("format two named compile-time arguments", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 6, 8>,
                                             stdx::named_arg<"y", int, 9, 11>>;
    constexpr auto expected =
        stdx::make_format_result<expected_named_t>("Hello 17 18"_ctst);

    STATIC_CHECK(STDX_CT_FORMAT("Hello {x} {y}", 17, 18) == expected);
}

TEST_CASE("format two compile-time arguments, only one named", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 9, 11>>;
    constexpr auto expected =
        stdx::make_format_result<expected_named_t>("Hello 17 18"_ctst);

    STATIC_CHECK(STDX_CT_FORMAT("Hello {} {y}", 17, 18) == expected);
}

TEST_CASE("format mixed named arguments (1)", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 6, 8>,
                                             stdx::named_arg<"y", int, 0>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello 17 {}"_ctst, stdx::tuple{18});

    auto y = 18;
    CHECK(STDX_CT_FORMAT("Hello {x} {y}", 17, y) == expected);
}

TEST_CASE("format mixed named arguments (2)", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"x", int, 0>,
                                             stdx::named_arg<"y", int, 9, 11>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {} 18"_ctst, stdx::tuple{17});

    auto x = 17;
    CHECK(STDX_CT_FORMAT("Hello {x} {y}", x, 18) == expected);
}

TEST_CASE("format mixed named arguments (3)", "[ct_format]") {
    using expected_named_t = stdx::type_list<
        stdx::named_arg<"a", int, 0>, stdx::named_arg<"b", int, 9, 11>,
        stdx::named_arg<"c", int, 1>, stdx::named_arg<"d", int, 15, 17>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {} 18 {} 20"_ctst, stdx::tuple{17, 19});

    auto x = 17;
    auto y = 19;
    CHECK(STDX_CT_FORMAT("Hello {a} {b} {c} {d}", x, 18, y, 20) == expected);
}

TEST_CASE("format mixed named and unnamed arguments", "[ct_format]") {
    using expected_named_t = stdx::type_list<
        stdx::named_arg<"a", int, 0>, stdx::named_arg<"b", int, 9, 11>,
        stdx::named_arg<"c", int, 2>, stdx::named_arg<"d", int, 21, 23>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "Hello {} 18 {} 20 {} 22"_ctst, stdx::tuple{17, 19, 21});

    auto x = 17;
    auto y = 19;
    auto z = 21;
    CHECK(STDX_CT_FORMAT("Hello {a} {b} {} {} {c} {d}", x, 18, y, 20, z, 22) ==
          expected);
}

TEST_CASE("format a named argument with format spec", "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"a", int, 6, 7>>;
    constexpr auto expected =
        stdx::make_format_result<expected_named_t>("Hello a"_ctst);

    STATIC_CHECK(STDX_CT_FORMAT("Hello {a:x}", 10) == expected);
}

TEST_CASE("format a format_result with named rt argument (1/normal)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 0>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is (year={})."_ctst, stdx::tuple{2022});

    auto S = stdx::ct_format<"(year={y})">(2022);
    CHECK(STDX_CT_FORMAT("The value is {}.", S) == expected);
}

TEST_CASE("format a format_result with named rt argument (2/after rt arg)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 1>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is {} (year={})."_ctst, stdx::tuple{17, 2022});

    auto S = stdx::ct_format<"(year={y})">(2022);
    auto x = 17;
    CHECK(STDX_CT_FORMAT("The value is {} {}.", x, S) == expected);
}

TEST_CASE("format a format_result with named rt argument (3/after ct arg)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 0>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is 17 (year={})."_ctst, stdx::tuple{2022});

    auto S = stdx::ct_format<"(year={y})">(2022);
    CHECK(STDX_CT_FORMAT("The value is {} {}.", 17, S) == expected);
}

TEST_CASE("format a format_result with named ct argument (1/normal)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 19, 23>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is (year=2022)."_ctst);

    STDX_PRAGMA(diagnostic push)
    STDX_PRAGMA(diagnostic ignored "-Wshadow")
    CHECK(STDX_CT_FORMAT("The value is {}.",
                         STDX_CT_FORMAT("(year={y})", 2022)) == expected);
    STDX_PRAGMA(diagnostic pop)
}

TEST_CASE("format a format_result with named ct argument (2/after ct arg)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 22, 26>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is 17 (year=2022)."_ctst);

    STDX_PRAGMA(diagnostic push)
    STDX_PRAGMA(diagnostic ignored "-Wshadow")
    CHECK(STDX_CT_FORMAT("The value is {} {}.", 17,
                         STDX_CT_FORMAT("(year={y})", 2022)) == expected);
    STDX_PRAGMA(diagnostic pop)
}

TEST_CASE("format a format_result with named ct argument (2/after rt arg)",
          "[ct_format]") {
    using expected_named_t = stdx::type_list<stdx::named_arg<"y", int, 22, 26>>;
    constexpr auto expected = stdx::make_format_result<expected_named_t>(
        "The value is {} (year=2022)."_ctst, stdx::tuple{17});

    auto x = 17;
    STDX_PRAGMA(diagnostic push)
    STDX_PRAGMA(diagnostic ignored "-Wshadow")
    CHECK(STDX_CT_FORMAT("The value is {} {}.", x,
                         STDX_CT_FORMAT("(year={y})", 2022)) == expected);
    STDX_PRAGMA(diagnostic pop)
}

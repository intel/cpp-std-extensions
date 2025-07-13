#include <stdx/ct_format.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace stdx::ct_string_literals;

TEST_CASE("detect string format specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::detail::count_specifiers("{}"sv) == 1u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{} {}"sv) == 2u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{"sv) == 0u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{{"sv) == 0u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{{{}"sv) == 1u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{{}}"sv) == 0u);
    STATIC_REQUIRE(stdx::detail::count_specifiers("{{{{"sv) == 0u);
}

TEST_CASE("split format string by specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::detail::split_specifiers<1>("hello"sv) ==
                   std::array{"hello"sv});
    STATIC_REQUIRE(stdx::detail::split_specifiers<2>("{}"sv) ==
                   std::array{"{}"sv, ""sv});
    STATIC_REQUIRE(stdx::detail::split_specifiers<3>("{} {}"sv) ==
                   std::array{"{}"sv, " {}"sv, ""sv});
    STATIC_REQUIRE(stdx::detail::split_specifiers<2>("{{{}"sv) ==
                   std::array{"{{{}"sv, ""sv});
    STATIC_REQUIRE(stdx::detail::split_specifiers<2>("{} hello"sv) ==
                   std::array{"{}"sv, " hello"sv});
}

TEST_CASE("format a static string", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello">() == "Hello"_fmt_res);
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

TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
                   "Hello **0x2a"_fmt_res);
}

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

TEST_CASE("format a runtime argument", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::format_result{"Hello {}"_ctst, stdx::make_tuple(x)};

    CHECK(stdx::ct_format<"Hello {}">(x) == expected);
    STATIC_REQUIRE(stdx::ct_format<"Hello {}">(x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::format_result{"Hello int {}"_ctst, stdx::make_tuple(x)};

    CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
    STATIC_REQUIRE(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) ==
                   expected);
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    STATIC_REQUIRE(
        stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) ==
        stdx::format_result{"Hello {} int"_ctst, stdx::make_tuple(42)});
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    STATIC_REQUIRE(
        stdx::ct_format<"Hello {} {}">(42, 17) ==
        stdx::format_result{"Hello {} {}"_ctst, stdx::make_tuple(42, 17)});
}

TEST_CASE("format multiple mixed arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;
    CHECK(stdx::ct_format<"Hello {} {} {} {} world">(42, CX_VALUE("A"sv), b,
                                                     CX_VALUE(int)) ==
          stdx::format_result{"Hello {} A {} int world"_ctst,
                              stdx::make_tuple(42, "B"sv)});
    STATIC_REQUIRE(stdx::ct_format<"Hello {} {} {} {} world">(
                       42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                   stdx::format_result{"Hello {} A {} int world"_ctst,
                                       stdx::make_tuple(42, "B"sv)});
}

TEST_CASE("format a format result", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"The value is {}.">(
                       stdx::ct_format<"(year={})">(2022)) ==
                   stdx::format_result{"The value is (year={})."_ctst,
                                       stdx::make_tuple(2022)});
}

namespace {
template <stdx::ct_string> constexpr auto conversion_success = true;
} // namespace

TEST_CASE("empty format_result can implicitly convert to ct_string",
          "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(
        stdx::detail::format_convertible<decltype(stdx::ct_format<"Hello">())>);
    STATIC_REQUIRE(stdx::detail::format_convertible<
                   decltype(stdx::ct_format<"Hello {}">("world"_ctst))>);
    STATIC_REQUIRE(not stdx::detail::format_convertible<
                   decltype(stdx::ct_format<"Hello {}">(42))>);

    STATIC_REQUIRE(conversion_success<stdx::ct_format<"Hello">()>);
}

TEST_CASE("empty format_result can explicitly convert to ct_string",
          "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(+stdx::ct_format<"Hello">() == "Hello"_cts);
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

TEST_CASE("format_to a different type", "[ct_format]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::ct_format<"{}", string_constant>(CX_VALUE("A"sv)) ==
                   stdx::format_result{string_constant<char, 'A'>{}});

    auto x = 17;
    CHECK(stdx::ct_format<"{}", string_constant>(x) ==
          stdx::format_result{string_constant<char, '{', '}'>{},
                              stdx::make_tuple(17)});
    STATIC_REQUIRE(stdx::ct_format<"{}", string_constant>(17) ==
                   stdx::format_result{string_constant<char, '{', '}'>{},
                                       stdx::make_tuple(17)});
}

TEST_CASE("format a string-type argument", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"Hello {}!">(string_constant<char, 'A'>{}) ==
                   "Hello A!"_fmt_res);
}

TEST_CASE("format a format result with different type", "[ct_format]") {
    STATIC_REQUIRE(stdx::ct_format<"A{}D", string_constant>(
                       stdx::ct_format<"B{}C", string_constant>(2022)) ==
                   stdx::format_result{
                       string_constant<char, 'A', 'B', '{', '}', 'C', 'D'>{},
                       stdx::make_tuple(2022)});
}

TEST_CASE("format multiple mixed arguments with different type",
          "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;
    CHECK(stdx::ct_format<"Hello {} {} {} {} world", string_constant>(
              42, CX_VALUE("A"sv), b, CX_VALUE(int)) ==
          stdx::format_result{
              stdx::ct_string_to_type<"Hello {} A {} int world"_cts,
                                      string_constant>(),
              stdx::make_tuple(42, "B"sv)});
    STATIC_REQUIRE(stdx::ct_format<"Hello {} {} {} {} world", string_constant>(
                       42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                   stdx::format_result{
                       stdx::ct_string_to_type<"Hello {} A {} int world"_cts,
                                               string_constant>(),
                       stdx::make_tuple(42, "B"sv)});
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
    auto r = stdx::ct_format<"Hello {}">(user::S{17});
    CHECK(r == stdx::format_result{"Hello S: {}"_ctst, stdx::make_tuple(17)});
}

TEST_CASE("FORMAT with no arguments", "[ct_format]") {
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello") == "Hello"_fmt_res);
}

TEST_CASE("FORMAT a compile-time string argument", "[ct_format]") {
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", "world") ==
                   "Hello world"_fmt_res);
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
        stdx::format_result{"Hello {}"_ctst, stdx::make_tuple(S)};
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == expected);
}
#endif

TEST_CASE("FORMAT a constexpr string_view argument", "[ct_format]") {
    constexpr static auto S = std::string_view{"world"};
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT an integral_constant argument", "[ct_format]") {
    auto I = std::integral_constant<int, 17>{};
    STATIC_REQUIRE(STDX_CT_FORMAT("Hello {}", I) == "Hello 17"_fmt_res);
}

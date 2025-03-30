#include <stdx/ct_format.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace stdx::ct_string_literals;

TEST_CASE("detect string format specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::detail::count_specifiers("{}"sv) == 1u);
    static_assert(stdx::detail::count_specifiers("{} {}"sv) == 2u);
    static_assert(stdx::detail::count_specifiers("{"sv) == 0u);
    static_assert(stdx::detail::count_specifiers("{{"sv) == 0u);
    static_assert(stdx::detail::count_specifiers("{{{}"sv) == 1u);
    static_assert(stdx::detail::count_specifiers("{{}}"sv) == 0u);
    static_assert(stdx::detail::count_specifiers("{{{{"sv) == 0u);
}

TEST_CASE("split format string by specifiers", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::detail::split_specifiers<1>("hello"sv) ==
                  std::array{"hello"sv});
    static_assert(stdx::detail::split_specifiers<2>("{}"sv) ==
                  std::array{"{}"sv, ""sv});
    static_assert(stdx::detail::split_specifiers<3>("{} {}"sv) ==
                  std::array{"{}"sv, " {}"sv, ""sv});
    static_assert(stdx::detail::split_specifiers<2>("{{{}"sv) ==
                  std::array{"{{{}"sv, ""sv});
    static_assert(stdx::detail::split_specifiers<2>("{} hello"sv) ==
                  std::array{"{}"sv, " hello"sv});
}

TEST_CASE("format a static string", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello">() == "Hello"_fmt_res);
}

TEST_CASE("format a compile-time stringish argument (CX_VALUE)",
          "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world"sv)) ==
                  "Hello world"_fmt_res);
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world"_cts)) ==
                  "Hello world"_fmt_res);
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world")) ==
                  "Hello world"_fmt_res);
}

TEST_CASE("format a compile-time stringish argument (ct)", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">("world"_ctst) ==
                  "Hello world"_fmt_res);
    static_assert(stdx::ct_format<"Hello {}">(stdx::ct<"world">()) ==
                  "Hello world"_fmt_res);
}

TEST_CASE("format a compile-time integral argument (CX_VALUE)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(42)) ==
                  "Hello 42"_fmt_res);
}

TEST_CASE("format a compile-time integral argument (ct)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(stdx::ct<42>()) ==
                  "Hello 42"_fmt_res);
}

TEST_CASE("format a type argument (CX_VALUE)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(int)) ==
                  "Hello int"_fmt_res);
}

TEST_CASE("format a type argument (ct)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(stdx::ct<int>()) ==
                  "Hello int"_fmt_res);
}

TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
                  "Hello **0x2a"_fmt_res);
}

namespace {
enum struct E { A };
}

TEST_CASE("format a compile-time enum argument (CX_VALUE)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(E::A)) ==
                  "Hello A"_fmt_res);
}

TEST_CASE("format a compile-time enum argument (ct)", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(stdx::ct<E::A>()) ==
                  "Hello A"_fmt_res);
}

TEST_CASE("format a runtime argument", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::format_result{"Hello {}"_ctst, stdx::make_tuple(x)};

    CHECK(stdx::ct_format<"Hello {}">(x) == expected);
    static_assert(stdx::ct_format<"Hello {}">(x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    constexpr auto x = 17;
    constexpr auto expected =
        stdx::format_result{"Hello int {}"_ctst, stdx::make_tuple(x)};

    CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
    static_assert(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    static_assert(
        stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) ==
        stdx::format_result{"Hello {} int"_ctst, stdx::make_tuple(42)});
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    static_assert(
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
    static_assert(stdx::ct_format<"Hello {} {} {} {} world">(
                      42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                  stdx::format_result{"Hello {} A {} int world"_ctst,
                                      stdx::make_tuple(42, "B"sv)});
}

TEST_CASE("format a format result", "[ct_format]") {
    static_assert(stdx::ct_format<"The value is {}.">(
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
    static_assert(
        stdx::detail::format_convertible<decltype(stdx::ct_format<"Hello">())>);
    static_assert(stdx::detail::format_convertible<
                  decltype(stdx::ct_format<"Hello {}">("world"_ctst))>);
    static_assert(not stdx::detail::format_convertible<
                  decltype(stdx::ct_format<"Hello {}">(42))>);

    static_assert(conversion_success<stdx::ct_format<"Hello">()>);
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
    static_assert(stdx::ct_format<"{}", string_constant>(CX_VALUE("A"sv)) ==
                  stdx::format_result{string_constant<char, 'A'>{}});

    auto x = 17;
    CHECK(stdx::ct_format<"{}", string_constant>(x) ==
          stdx::format_result{string_constant<char, '{', '}'>{},
                              stdx::make_tuple(17)});
    static_assert(stdx::ct_format<"{}", string_constant>(17) ==
                  stdx::format_result{string_constant<char, '{', '}'>{},
                                      stdx::make_tuple(17)});
}

TEST_CASE("format a string-type argument", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}!">(string_constant<char, 'A'>{}) ==
                  "Hello A!"_fmt_res);
}

TEST_CASE("format a format result with different type", "[ct_format]") {
    static_assert(stdx::ct_format<"A{}D", string_constant>(
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
    static_assert(stdx::ct_format<"Hello {} {} {} {} world", string_constant>(
                      42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                  stdx::format_result{
                      stdx::ct_string_to_type<"Hello {} A {} int world"_cts,
                                              string_constant>(),
                      stdx::make_tuple(42, "B"sv)});
}

TEST_CASE("num fmt specifiers", "[ct_format]") {
    static_assert(stdx::num_fmt_specifiers<"{}"> == 1u);
    static_assert(stdx::num_fmt_specifiers<"{} {}"> == 2u);
}

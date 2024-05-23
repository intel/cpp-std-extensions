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
    static_assert(stdx::ct_format<"Hello">() == "Hello"_cts);
}

TEST_CASE("format a compile-time stringish argument", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world"sv)) ==
                  "Hello world"_cts);
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world"_cts)) ==
                  "Hello world"_cts);
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE("world")) ==
                  "Hello world"_cts);
}

TEST_CASE("format a compile-time integral argument", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(42)) == "Hello 42"_cts);
}

TEST_CASE("format a type argument", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(int)) ==
                  "Hello int"_cts);
}

TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
                  "Hello **0x2a"_cts);
}

namespace {
enum struct E { A };
}

TEST_CASE("format a compile-time enum argument", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(E::A)) == "Hello A"_cts);
}

TEST_CASE("format a runtime argument", "[ct_format]") {
    auto x = 42;
    CHECK(stdx::ct_format<"Hello {}">(x) ==
          stdx::format_result{"Hello {}"_cts, stdx::make_tuple(42)});
    static_assert(stdx::ct_format<"Hello {}">(42) ==
                  stdx::format_result{"Hello {}"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    auto x = 42;
    CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) ==
          stdx::format_result{"Hello int {}"_cts, stdx::make_tuple(42)});
    static_assert(
        stdx::ct_format<"Hello {} {}">(CX_VALUE(int), 42) ==
        stdx::format_result{"Hello int {}"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    static_assert(
        stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) ==
        stdx::format_result{"Hello {} int"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    static_assert(
        stdx::ct_format<"Hello {} {}">(42, 17) ==
        stdx::format_result{"Hello {} {}"_cts, stdx::make_tuple(42, 17)});
}

TEST_CASE("format multiple mixed arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;
    CHECK(stdx::ct_format<"Hello {} {} {} {} world">(42, CX_VALUE("A"sv), b,
                                                     CX_VALUE(int)) ==
          stdx::format_result{"Hello {} A {} int world"_cts,
                              stdx::make_tuple(42, "B"sv)});
    static_assert(stdx::ct_format<"Hello {} {} {} {} world">(
                      42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                  stdx::format_result{"Hello {} A {} int world"_cts,
                                      stdx::make_tuple(42, "B"sv)});
}

TEST_CASE("format a formatted string", "[ct_format]") {
    static_assert(stdx::ct_format<"The value is {}.">(
                      CX_VALUE(stdx::ct_format<"(year={})">(2022))) ==
                  stdx::format_result{"The value is (year={})."_cts,
                                      stdx::make_tuple(2022)});
}

TEST_CASE("format a ct-formatted string", "[ct_format]") {
    constexpr static auto cts = stdx::ct_format<"(year={})">(CX_VALUE(2024));
    static_assert(stdx::ct_format<"The value is {}.">(CX_VALUE(cts)) ==
                  "The value is (year=2024)."_cts);
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
                  string_constant<char, 'A'>{});

    auto x = 42;
    CHECK(stdx::ct_format<"{}", string_constant>(x) ==
          stdx::format_result{string_constant<char, '{', '}'>{},
                              stdx::make_tuple(42)});
    static_assert(stdx::ct_format<"{}", string_constant>(42) ==
                  stdx::format_result{string_constant<char, '{', '}'>{},
                                      stdx::make_tuple(42)});
}

TEST_CASE("format a string-type argument", "[ct_format]") {
    static_assert(stdx::ct_format<"Hello {}!">(string_constant<char, 'A'>{}) ==
                  "Hello A!"_cts);
}

TEST_CASE("format a formatted string with different type", "[ct_format]") {
    static_assert(stdx::ct_format<"A{}D", string_constant>(CX_VALUE(
                      stdx::ct_format<"B{}C", string_constant>(2022))) ==
                  stdx::format_result{
                      string_constant<char, 'A', 'B', '{', '}', 'C', 'D'>{},
                      stdx::make_tuple(2022)});
}

TEST_CASE("format a ct-formatted string with different type", "[ct_format]") {
    constexpr static auto cts =
        stdx::ct_format<"B{}C", string_constant>(CX_VALUE(2024));
    static_assert(
        stdx::ct_format<"A{}D", string_constant>(CX_VALUE(cts)) ==
        string_constant<char, 'A', 'B', '2', '0', '2', '4', 'C', 'D'>{});
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

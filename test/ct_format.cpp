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
}

TEST_CASE("format a compile-time integral argument", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(42)) == "Hello 42"_cts);
}

TEST_CASE("format a type argument", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(int)) ==
                  "Hello int"_cts);
}

TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
                  "Hello **0x2a"_cts);
}

namespace {
enum struct E { A };
}

TEST_CASE("format a compile-time enum argument", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(CX_VALUE(E::A)) == "Hello A"_cts);
}

TEST_CASE("format a runtime argument", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {}">(42) ==
                  stdx::format_result{"Hello {}"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(
        stdx::ct_format<"Hello {} {}">(CX_VALUE(int), 42) ==
        stdx::format_result{"Hello int {}"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(
        stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) ==
        stdx::format_result{"Hello {} int"_cts, stdx::make_tuple(42)});
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(
        stdx::ct_format<"Hello {} {}">(42, 17) ==
        stdx::format_result{"Hello {} {}"_cts, stdx::make_tuple(42, 17)});
}

TEST_CASE("format multiple mixed arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    static_assert(stdx::ct_format<"Hello {} {} {} {} world">(
                      42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) ==
                  stdx::format_result{"Hello {} A {} int world"_cts,
                                      stdx::make_tuple(42, "B"sv)});
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

    static_assert(stdx::ct_format<"{}", string_constant>(42) ==
                  stdx::format_result{string_constant<char, '{', '}'>{},
                                      stdx::make_tuple(42)});
}

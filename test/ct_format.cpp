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

TEST_CASE("format a static string", "[ct_format]") {
    STATIC_CHECK(stdx::ct_format<"Hello">() == "Hello"_fmt_res);
}

TEST_CASE("format a compile-time stringish argument (CX_VALUE)",
          "[ct_format]") {
    using namespace std::string_view_literals;
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE("world"sv)) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<std::string_view>{}}));
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE("world"_cts)) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<stdx::ct_string<6>>{}}));
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE("world")) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<char const *>{}}));
}

TEST_CASE("format a compile-time stringish argument (ct)", "[ct_format]") {
    using namespace std::string_view_literals;
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">("world"_ctst) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<stdx::ct_string<6>>{}}));
    STATIC_CHECK(stdx::ct_format<"Hello {}">(stdx::ct<"world">()) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<stdx::ct_string<6>>{}}));
}

TEST_CASE("format a compile-time integral argument (CX_VALUE)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE(42)) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello 42"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("format a negative compile-time integral argument (CX_VALUE)",
          "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 9>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {}">(CX_VALUE(-42)) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello -42"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("format most negative compile-time integral argument (CX_VALUE)",
          "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 17>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {}">(
            CX_VALUE(std::numeric_limits<int>::min())) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello -2147483648"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("format zero (CX_VALUE)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 7>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE(0)) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello 0"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("format a char (CX_VALUE)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 7>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {}orld">(CX_VALUE('w')) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello world"_ctst, stdx::tuple{stdx::ct_format_arg<char>{}}));
}

TEST_CASE("format a compile-time integral argument (ct)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(stdx::ct<42>()) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello 42"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("format a type argument (CX_VALUE)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 9>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {}">(CX_VALUE(int)) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello int"_ctst,
            stdx::tuple{stdx::ct_format_arg<stdx::type_identity<int>>{}}));
}

TEST_CASE("format a type argument (ct)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 9>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {}">(stdx::ct<int>()) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello int"_ctst,
            stdx::tuple{stdx::ct_format_arg<stdx::type_identity<int>>{}}));
}

TEST_CASE("format a compile-time argument with different base", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<8, 10>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello 0x{:x}">(CX_VALUE(42)) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello 0x2a"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

#ifndef STDX_FREESTANDING
TEST_CASE("format a compile-time argument with fmt spec", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 12>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {:*>#6x}">(CX_VALUE(42)) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello **0x2a"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}
#endif

namespace {
enum struct E { A };
}

TEST_CASE("format a compile-time enum argument (CX_VALUE)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 7>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(CX_VALUE(E::A)) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello A"_ctst, stdx::tuple{stdx::ct_format_arg<E>{}}));
}

TEST_CASE("format a compile-time enum argument (ct)", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 7>>;
    STATIC_CHECK(stdx::ct_format<"Hello {}">(stdx::ct<E::A>()) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello A"_ctst, stdx::tuple{stdx::ct_format_arg<E>{}}));
}

TEST_CASE("format multiple compile-time arguments", "[ct_format]") {
    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 7>, stdx::format_span<8, 13>>;
    STATIC_CHECK(
        stdx::ct_format<"Hello {} {}">(CX_VALUE(E::A), CX_VALUE("world")) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello A world"_ctst,
            stdx::tuple{stdx::ct_format_arg<E>{},
                        stdx::ct_format_arg<char const *>{}}));
}

TEST_CASE("format a runtime argument", "[ct_format]") {
    constexpr auto x = 17;
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {}"_ctst, stdx::tuple{x});
    CHECK(stdx::ct_format<"Hello {}">(x) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {}">(x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (1)", "[ct_format]") {
    constexpr auto x = 17;
    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 9>, stdx::format_span<10, 12>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello int {}"_ctst,
        stdx::tuple{stdx::ct_format_arg<stdx::type_identity<int>>{}, x});
    CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {}">(CX_VALUE(int), x) == expected);
}

TEST_CASE("format a compile-time and a runtime argument (2)", "[ct_format]") {
    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 8>, stdx::format_span<9, 12>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {} int"_ctst,
        stdx::tuple{42, stdx::ct_format_arg<stdx::type_identity<int>>{}});
    STATIC_CHECK(stdx::ct_format<"Hello {} {}">(42, CX_VALUE(int)) == expected);
}

TEST_CASE("format multiple runtime arguments", "[ct_format]") {
    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 8>, stdx::format_span<9, 11>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {} {}"_ctst, stdx::tuple{42, 17});
    STATIC_CHECK(stdx::ct_format<"Hello {} {}">(42, 17) == expected);
}

TEST_CASE("format multiple mixed arguments", "[ct_format]") {
    using namespace std::string_view_literals;
    auto b = "B"sv;

    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 8>, stdx::format_span<9, 10>,
                        stdx::format_span<11, 13>, stdx::format_span<14, 17>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {} A {} int world"_ctst,
        stdx::tuple{42, stdx::ct_format_arg<std::string_view>{}, "B"sv,
                    stdx::ct_format_arg<stdx::type_identity<int>>{}});

    CHECK(stdx::ct_format<"Hello {} {} {} {} world">(
              42, CX_VALUE("A"sv), b, CX_VALUE(int)) == expected);
    STATIC_CHECK(stdx::ct_format<"Hello {} {} {} {} world">(
                     42, CX_VALUE("A"sv), "B"sv, CX_VALUE(int)) == expected);
}

TEST_CASE("format a format result", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<19, 21>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "The value is (year={})."_ctst, stdx::tuple{2022});

    STATIC_CHECK(stdx::ct_format<"The value is {}.">(
                     stdx::ct_format<"(year={})">(2022)) == expected);
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

    using expected_spans_t = stdx::type_list<stdx::format_span<0, 1>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        string_constant<char, 'A'>{},
        stdx::tuple{stdx::ct_format_arg<std::string_view>{}});

    STATIC_CHECK(stdx::ct_format<"{}", string_constant>(CX_VALUE("A"sv)) ==
                 expected);
}

TEST_CASE("format_to a different type (runtime value)", "[ct_format]") {
    using namespace std::string_view_literals;

    using expected_spans_t = stdx::type_list<stdx::format_span<0, 2>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        string_constant<char, '{', '}'>{}, stdx::tuple{17});

    CHECK(stdx::ct_format<"{}", string_constant>(17) == expected);
    STATIC_CHECK(stdx::ct_format<"{}", string_constant>(17) == expected);
}

TEST_CASE("format a string-type argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 7>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello A!"_ctst,
        stdx::tuple{stdx::ct_format_arg<stdx::ct_string<2>>{}});

    STATIC_CHECK(stdx::ct_format<"Hello {}!">(string_constant<char, 'A'>{}) ==
                 expected);
}

TEST_CASE("format a format result with different type", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<2, 4>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
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

    using expected_spans_t =
        stdx::type_list<stdx::format_span<6, 8>, stdx::format_span<9, 10>,
                        stdx::format_span<11, 13>, stdx::format_span<14, 17>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        stdx::ct_string_to_type<"Hello {} A {} int world"_cts,
                                string_constant>(),
        stdx::tuple{42, stdx::ct_format_arg<std::string_view>{}, "B"sv,
                    stdx::ct_format_arg<stdx::type_identity<int>>{}});

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
    using expected_spans_t = stdx::type_list<stdx::format_span<9, 11>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello S: {}"_ctst, stdx::tuple{17});

    auto r = stdx::ct_format<"Hello {}">(user::S{17});
    CHECK(r == expected);
}

TEST_CASE("FORMAT with no arguments", "[ct_format]") {
    STATIC_CHECK(STDX_CT_FORMAT("Hello") == "Hello"_fmt_res);
}

TEST_CASE("FORMAT a compile-time string argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", "world") ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<char const *>{}}));
}

TEST_CASE("FORMAT a compile-time int argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", 17) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello 17"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

TEST_CASE("FORMAT a type argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 9>>;
    STATIC_CHECK(
        STDX_CT_FORMAT("Hello {}", int) ==
        stdx::make_format_result<expected_spans_t>(
            "Hello int"_ctst,
            stdx::tuple{stdx::ct_format_arg<stdx::type_identity<int>>{}}));
}

TEST_CASE("FORMAT a constexpr ct_string argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    constexpr static auto S = "world"_cts;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", S) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<stdx::ct_string<6>>{}}));
}

TEST_CASE("FORMAT a cts_t argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    auto S = "world"_ctst;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", S) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello world"_ctst,
                     stdx::tuple{stdx::ct_format_arg<stdx::ct_string<6>>{}}));
}

TEST_CASE("FORMAT a format_result argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<19, 21>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "The value is (year={})."_ctst, stdx::tuple{2022});

    auto S = stdx::ct_format<"(year={})">(2022);
    CHECK(STDX_CT_FORMAT("The value is {}.", S) == expected);
}

TEST_CASE("FORMAT an empty format_result argument", "[ct_format]") {
    auto S = "world"_fmt_res;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", S) == "Hello world"_fmt_res);
}

TEST_CASE("FORMAT a constexpr int argument", "[ct_format]") {
    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    constexpr static auto I = 17;
    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", I) ==
                 stdx::make_format_result<expected_spans_t>(
                     "Hello 17"_ctst, stdx::tuple{stdx::ct_format_arg<int>{}}));
}

#ifdef __clang__
TEST_CASE("FORMAT a constexpr nonstatic string_view argument", "[ct_format]") {
    constexpr auto S = std::string_view{"world"};

    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {}"_ctst, stdx::tuple{S});

    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", S) == expected);
}
#endif

TEST_CASE("FORMAT a constexpr string_view argument", "[ct_format]") {
    constexpr static auto S = std::string_view{"world"};

    using expected_spans_t = stdx::type_list<stdx::format_span<6, 11>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello world"_ctst,
        stdx::tuple{stdx::ct_format_arg<std::string_view>{}});

    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", S) == expected);
}

TEST_CASE("FORMAT an integral_constant argument", "[ct_format]") {
    auto I = std::integral_constant<unsigned int, 17u>{};

    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello 17"_ctst, stdx::tuple{stdx::ct_format_arg<unsigned int>{}});

    STATIC_CHECK(STDX_CT_FORMAT("Hello {}", I) == expected);
}

#ifdef __clang__
namespace {
struct expression_test {
    int f(int x) { return x; }
};
} // namespace

TEST_CASE("FORMAT non-constexpr expression", "[utility]") {
    auto x = 17;

    using expected_spans_t = stdx::type_list<stdx::format_span<6, 8>>;
    constexpr auto expected = stdx::make_format_result<expected_spans_t>(
        "Hello {}"_ctst, stdx::tuple{17});

    CHECK(STDX_CT_FORMAT("Hello {}", expression_test{}.f(x)) == expected);
}
#endif

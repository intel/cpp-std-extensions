#include <stdx/ct_string.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <string_view>
#include <type_traits>

namespace {
template <typename T, T...> struct string_constant {};
} // namespace

TEST_CASE("construction", "[ct_string]") {
    [[maybe_unused]] constexpr auto s = stdx::ct_string{"ABC"};
}

TEST_CASE("UDL", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    [[maybe_unused]] constexpr auto s = "ABC"_cts;
}

TEST_CASE("empty", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{""};
    STATIC_REQUIRE(s1.empty());
    constexpr auto s2 = stdx::ct_string{"A"};
    STATIC_REQUIRE(not s2.empty());
}

TEST_CASE("size", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    STATIC_REQUIRE(s.size() == 3u);
    STATIC_REQUIRE(not s.empty());
}

TEST_CASE("equality", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"ABC"};
    constexpr auto s2 = stdx::ct_string{"ABC"};
    constexpr auto s3 = stdx::ct_string{"ABD"};
    STATIC_REQUIRE(s1 == s2);
    STATIC_REQUIRE(s1 != s3);
}

TEST_CASE("explicit length construction", "[ct_string]") {
    constexpr auto s = stdx::ct_string<3u>{"ABC", 2};
    STATIC_REQUIRE(s == stdx::ct_string{"AB"});
}

TEST_CASE("from type", "[ct_string]") {
    using T = string_constant<char, 'A', 'B', 'C'>;
    constexpr auto s = stdx::ct_string_from_type(T{});
    STATIC_REQUIRE(s == stdx::ct_string{"ABC"});
}

TEST_CASE("to type", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    constexpr auto sc = stdx::ct_string_to_type<s, string_constant>();
    STATIC_REQUIRE(std::is_same_v<decltype(sc),
                                  string_constant<char, 'A', 'B', 'C'> const>);
}

TEST_CASE("to type_t", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    using sc = stdx::ct_string_to_type_t<s, string_constant>;
    STATIC_REQUIRE(std::is_same_v<sc, string_constant<char, 'A', 'B', 'C'>>);
}

TEST_CASE("to string_view", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    auto const sv = static_cast<std::string_view>(s);
    CHECK(sv == "ABC");
}

TEST_CASE("string split (character present)", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"A.B"};
    constexpr auto p = stdx::split<s, '.'>();
    STATIC_REQUIRE(p.first == stdx::ct_string{"A"});
    STATIC_REQUIRE(p.second == stdx::ct_string{"B"});
}

TEST_CASE("string split (character not present)", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"A"};
    constexpr auto p = stdx::split<s, '.'>();
    STATIC_REQUIRE(p.first == stdx::ct_string{"A"});
    STATIC_REQUIRE(p.second.empty());
}

TEST_CASE("string concat (lhs empty)", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{""};
    constexpr auto s2 = stdx::ct_string{"def"};
    STATIC_REQUIRE(s1 + s2 == stdx::ct_string{"def"});
}

TEST_CASE("string concat (rhs empty)", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"abc"};
    constexpr auto s2 = stdx::ct_string{""};
    STATIC_REQUIRE(s1 + s2 == stdx::ct_string{"abc"});
}

TEST_CASE("string concat", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"abc"};
    constexpr auto s2 = stdx::ct_string{"def"};
    STATIC_REQUIRE(s1 + s2 == stdx::ct_string{"abcdef"});
}

TEST_CASE("ct_string as iterable", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"abc"};
    STATIC_REQUIRE(std::next(std::begin(s), std::size(s)) == std::end(s));

    auto it = std::cbegin(s);
    CHECK(*it++ == 'a');
    CHECK(*it++ == 'b');
    CHECK(*it++ == 'c');
    CHECK(it == std::cend(s));
}

TEST_CASE("ct_string as reverse iterable", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"abc"};
    STATIC_REQUIRE(std::next(std::rbegin(s), std::size(s)) == std::rend(s));

    auto it = std::crbegin(s);
    CHECK(*it++ == 'c');
    CHECK(*it++ == 'b');
    CHECK(*it++ == 'a');
    CHECK(it == std::crend(s));
}

namespace {
template <stdx::ct_string S> constexpr auto to_cx_value() {
    return CX_VALUE(S);
}
} // namespace

TEST_CASE("template argument as CX_VALUE", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    constexpr auto s = to_cx_value<"Hello">();
    STATIC_REQUIRE(s() == "Hello"_cts);
}

TEST_CASE("wrap ct_string in type", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    using S = stdx::cts_t<"Hello">;
    STATIC_REQUIRE(S::value == "Hello"_cts);
}

TEST_CASE("ct (ct_string)", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    constexpr auto v1 = stdx::ct<"Hello">();
    STATIC_REQUIRE(v1 == "Hello"_ctst);
    constexpr auto v2 = stdx::ct<"Hello"_cts>();
    STATIC_REQUIRE(v2 == "Hello"_ctst);
}

namespace {
template <stdx::ct_string> constexpr auto conversion_success = true;
} // namespace

TEST_CASE("cts_t can implicitly convert to ct_string", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    STATIC_REQUIRE(conversion_success<"Hello"_ctst>);
}

TEST_CASE("operator+ works to concat cts_t and ct_string", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    STATIC_REQUIRE("Hello"_ctst + " world"_cts == "Hello world"_cts);
    STATIC_REQUIRE("Hello"_cts + " world"_ctst == "Hello world"_cts);
}

TEST_CASE("is_ct (ct_string)", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    constexpr auto v1 = stdx::ct<"Hello">();
    STATIC_REQUIRE(stdx::is_ct_v<decltype(v1)>);
}

TEST_CASE("CT_WRAP", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    auto x1 = "hello"_cts;
    STATIC_REQUIRE(std::is_same_v<decltype(CT_WRAP(x1)), stdx::ct_string<6>>);
    CHECK(CT_WRAP(x1) == "hello"_cts);

    auto x2 = "hello"_ctst;
    STATIC_REQUIRE(std::is_same_v<decltype(CT_WRAP(x2)), stdx::cts_t<"hello">>);
    STATIC_REQUIRE(CT_WRAP(x2) == "hello"_ctst);

    constexpr static auto x3 = "hello"_cts;
    STATIC_REQUIRE(std::is_same_v<decltype(CT_WRAP(x3)), stdx::cts_t<"hello">>);
    STATIC_REQUIRE(CT_WRAP(x3) == "hello"_ctst);

    []<stdx::ct_string X>() {
        STATIC_REQUIRE(
            std::is_same_v<decltype(CT_WRAP(X)), stdx::cts_t<"hello">>);
        STATIC_REQUIRE(CT_WRAP(X) == "hello"_ctst);
    }.template operator()<"hello">();
}

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
    static_assert(s1.empty());
    constexpr auto s2 = stdx::ct_string{"A"};
    static_assert(not s2.empty());
}

TEST_CASE("size", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    static_assert(s.size() == 3u);
    static_assert(not s.empty());
}

TEST_CASE("equality", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"ABC"};
    constexpr auto s2 = stdx::ct_string{"ABC"};
    constexpr auto s3 = stdx::ct_string{"ABD"};
    static_assert(s1 == s2);
    static_assert(s1 != s3);
}

TEST_CASE("explicit length construction", "[ct_string]") {
    constexpr auto s = stdx::ct_string<3u>{"ABC", 2};
    static_assert(s == stdx::ct_string{"AB"});
}

TEST_CASE("from type", "[ct_string]") {
    using T = string_constant<char, 'A', 'B', 'C'>;
    constexpr auto s = stdx::ct_string_from_type(T{});
    static_assert(s == stdx::ct_string{"ABC"});
}

TEST_CASE("to type", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    constexpr auto sc = stdx::ct_string_to_type<s, string_constant>();
    static_assert(std::is_same_v<decltype(sc),
                                 string_constant<char, 'A', 'B', 'C'> const>);
}

TEST_CASE("to string_view", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"ABC"};
    auto const sv = static_cast<std::string_view>(s);
    CHECK(sv == "ABC");
}

TEST_CASE("string split (character present)", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"A.B"};
    constexpr auto p = stdx::split<s, '.'>();
    static_assert(p.first == stdx::ct_string{"A"});
    static_assert(p.second == stdx::ct_string{"B"});
}

TEST_CASE("string split (character not present)", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"A"};
    constexpr auto p = stdx::split<s, '.'>();
    static_assert(p.first == stdx::ct_string{"A"});
    static_assert(p.second.empty());
}

TEST_CASE("string concat (lhs empty)", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{""};
    constexpr auto s2 = stdx::ct_string{"def"};
    static_assert(s1 + s2 == stdx::ct_string{"def"});
}

TEST_CASE("string concat (rhs empty)", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"abc"};
    constexpr auto s2 = stdx::ct_string{""};
    static_assert(s1 + s2 == stdx::ct_string{"abc"});
}

TEST_CASE("string concat", "[ct_string]") {
    constexpr auto s1 = stdx::ct_string{"abc"};
    constexpr auto s2 = stdx::ct_string{"def"};
    static_assert(s1 + s2 == stdx::ct_string{"abcdef"});
}

TEST_CASE("ct_string as iterable", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"abc"};
    static_assert(std::next(std::begin(s), std::size(s)) == std::end(s));

    auto it = std::cbegin(s);
    CHECK(*it++ == 'a');
    CHECK(*it++ == 'b');
    CHECK(*it++ == 'c');
    CHECK(it == std::cend(s));
}

TEST_CASE("ct_string as reverse iterable", "[ct_string]") {
    constexpr auto s = stdx::ct_string{"abc"};
    static_assert(std::next(std::rbegin(s), std::size(s)) == std::rend(s));

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
    static_assert(s() == "Hello"_cts);
}

TEST_CASE("wrap ct_string in type", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    using S = stdx::cts_t<"Hello">;
    static_assert(S::value == "Hello"_cts);
}

TEST_CASE("ct (ct_string)", "[ct_string]") {
    using namespace stdx::ct_string_literals;
    constexpr auto v1 = stdx::ct<"Hello">();
    static_assert(v1 == "Hello"_ctst);
    constexpr auto v2 = stdx::ct<"Hello"_cts>();
    static_assert(v2 == "Hello"_ctst);
}

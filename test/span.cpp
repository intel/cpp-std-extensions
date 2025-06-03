#include <stdx/span.hpp>
#include <stdx/type_traits.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

TEMPLATE_TEST_CASE("span exposes types", "[span]", std::uint8_t,
                   std::uint8_t const, std::uint8_t *) {
    using S = stdx::span<TestType>;
    STATIC_REQUIRE(std::is_same_v<typename S::element_type, TestType>);
    STATIC_REQUIRE(
        std::is_same_v<typename S::value_type, stdx::remove_cvref_t<TestType>>);
    STATIC_REQUIRE(std::is_same_v<typename S::size_type, std::size_t>);
    STATIC_REQUIRE(std::is_same_v<typename S::difference_type, std::ptrdiff_t>);
    STATIC_REQUIRE(std::is_same_v<typename S::pointer, TestType *>);
    STATIC_REQUIRE(std::is_same_v<typename S::const_pointer, TestType const *>);
    STATIC_REQUIRE(std::is_same_v<typename S::reference, TestType &>);
    STATIC_REQUIRE(
        std::is_same_v<typename S::const_reference, TestType const &>);
    STATIC_REQUIRE(std::is_same_v<std::void_t<typename S::iterator>, void>);
    STATIC_REQUIRE(
        std::is_same_v<std::void_t<typename S::const_iterator>, void>);
    STATIC_REQUIRE(
        std::is_same_v<std::void_t<typename S::reverse_iterator>, void>);
    STATIC_REQUIRE(
        std::is_same_v<std::void_t<typename S::const_reverse_iterator>, void>);
}

TEST_CASE("span exposes extent", "[span]") {
    using S = stdx::span<int>;
    STATIC_REQUIRE(S::extent == stdx::dynamic_extent);
    using S4 = stdx::span<int, 4u>;
    STATIC_REQUIRE(S4::extent == 4u);
}

TEST_CASE("span is default constructible", "[span]") {
    constexpr auto s = stdx::span<int>{};
    STATIC_REQUIRE(std::data(s) == nullptr);
    STATIC_REQUIRE(std::size(s) == 0u);
}

TEST_CASE("dynamic span is implicitly constructible from iterator & size "
          "(const data)",
          "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = [&]() -> stdx::span<int const> {
        return {std::data(a), 4};
    }();
    STATIC_REQUIRE(std::data(s) == std::data(a));
    STATIC_REQUIRE(std::size(s) == std::size(a));
}

TEST_CASE("span is explicitly constructible from iterator & size "
          "(const data)",
          "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span<int const, 4>{std::data(a), 4};
    STATIC_REQUIRE(std::data(s) == std::data(a));
}

TEST_CASE("dynamic span is implicitly constructible from iterator & size (non "
          "const data)",
          "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = [&]() -> stdx::span<int> { return {std::data(a), 4}; }();
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int>>);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("span is explicitly constructible from iterator & size "
          "(non const data)",
          "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = stdx::span<int, 4>{std::data(a), 4};
    CHECK(std::data(s) == std::data(a));
}

TEST_CASE("dynamic span is implicitly constructible from iterator & sentinel "
          "(const data)",
          "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = [&]() -> stdx::span<int const> {
        return {std::begin(a), std::end(a)};
    }();
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int const> const>);
    STATIC_REQUIRE(std::data(s) == std::data(a));
    STATIC_REQUIRE(std::size(s) == std::size(a));
}

TEST_CASE("span is explicitly constructible from iterator & sentinel "
          "(const data)",
          "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span<int const, 4>{std::begin(a), std::end(a)};
    STATIC_REQUIRE(std::data(s) == std::data(a));
}

TEST_CASE(
    "dynamic span is implicitly constructible from iterator & sentinel (non "
    "const data)",
    "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = [&]() -> stdx::span<int> {
        return {std::begin(a), std::end(a)};
    }();
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int>>);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("span is explicitly constructible from iterator & sentinel (non "
          "const data)",
          "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = stdx::span<int, 4>{std::begin(a), std::end(a)};
    CHECK(std::data(s) == std::data(a));
}

TEST_CASE("span is constructible from std::array (const data)", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span{a};
    STATIC_REQUIRE(
        std::is_same_v<decltype(s), stdx::span<int const, 4u> const>);
    STATIC_REQUIRE(std::data(s) == std::data(a));
    STATIC_REQUIRE(std::size(s) == std::size(a));
}

TEST_CASE("span is constructible from std::array (non const data)", "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int, 4u>>);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
    STATIC_REQUIRE(std::size(s) == 4);
}

TEST_CASE("dynamic span is constructible from std::array (const data)",
          "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    auto s = [](stdx::span<int const> x) { return x; }(a);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("dynamic span is constructible from std::array (non const data)",
          "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s = [](stdx::span<int> x) { return x; }(a);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("span is constructible from C-style array (const data)", "[span]") {
    constexpr static int a[] = {1, 2, 3, 4};
    constexpr auto s = stdx::span{a};
    STATIC_REQUIRE(
        std::is_same_v<decltype(s), stdx::span<int const, 4u> const>);
    STATIC_REQUIRE(std::data(s) == std::data(a));
    STATIC_REQUIRE(std::size(s) == std::size(a));
}

TEST_CASE("span is constructible from C-style array (non const data)",
          "[span]") {
    int a[] = {1, 2, 3, 4};
    auto s = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int, 4u>>);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("dynamic span is constructible from C-style array (const data)",
          "[span]") {
    constexpr static int a[] = {1, 2, 3, 4};
    auto s = [](stdx::span<int const> x) { return x; }(a);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("dynamic span is constructible from C-style array (non const data)",
          "[span]") {
    int a[] = {1, 2, 3, 4};
    auto s = [](stdx::span<int> x) { return x; }(a);
    CHECK(std::data(s) == std::data(a));
    CHECK(std::size(s) == std::size(a));
}

TEST_CASE("dynamic span is implicitly constructible from range (const data)",
          "[span]") {
    std::vector const v{1, 2, 3, 4};
    auto s = stdx::span{v};
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int const>>);
    CHECK(std::data(s) == std::data(v));
    CHECK(std::size(s) == std::size(v));
}

TEST_CASE("span is explicitly constructible from range (const data)",
          "[span]") {
    std::vector const v{1, 2, 3, 4};
    auto s = stdx::span<int const, 4>{v};
    CHECK(std::data(s) == std::data(v));
}

TEST_CASE(
    "dynamic span is implicitly constructible from range (non const data)",
    "[span]") {
    std::vector v{1, 2, 3, 4};
    auto s = stdx::span{v};
    STATIC_REQUIRE(std::is_same_v<decltype(s), stdx::span<int>>);
    CHECK(std::data(s) == std::data(v));
    CHECK(std::size(s) == std::size(v));
}

TEST_CASE("span is explicitly constructible from range (non const data)",
          "[span]") {
    std::vector v{1, 2, 3, 4};
    auto s = stdx::span<int, 4>{v};
    CHECK(std::data(s) == std::data(v));
}

TEST_CASE("span is copy constructible", "[span]") {
    std::array a{1, 2, 3, 4};
    auto s1 = stdx::span{a};
    auto s2 = s1;
    CHECK(std::data(s1) == std::data(s2));
    CHECK(std::size(s1) == std::size(s2));
}

TEST_CASE("span is copy assignable", "[span]") {
    std::vector v{1, 2, 3, 4};
    auto s1 = stdx::span{v};
    decltype(s1) s2{};
    s2 = s1;
    CHECK(std::data(s1) == std::data(s2));
    CHECK(std::size(s1) == std::size(s2));
}

TEST_CASE("span is a range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    CHECK(*s.begin() == 1);
    CHECK(std::distance(s.begin(), s.end()) == std::size(a));
}

TEST_CASE("span is a const range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s.cbegin()), int const *>);
    CHECK(*std::cbegin(s) == 1);
    CHECK(std::distance(std::cbegin(s), std::cend(s)) == std::size(a));
}

TEST_CASE("span is a mutable range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s.begin()), int *>);
    *s.begin() = 2;
    CHECK(a[0] == 2);
}

TEST_CASE("span is a reverse range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    CHECK(*s.rbegin() == 4);
    CHECK(std::distance(s.rbegin(), s.rend()) == std::size(a));
}

TEST_CASE("span is a const reverse range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    CHECK(*std::crbegin(s) == 4);
    CHECK(std::distance(std::crbegin(s), std::crend(s)) == std::size(a));
}

TEST_CASE("span is a mutable reverse range", "[span]") {
    std::array a{1, 2, 3, 4};
    auto const s = stdx::span{a};
    *s.rbegin() = 2;
    CHECK(a[3] == 2);
}

TEST_CASE("dynamic span reports empty", "[span]") {
    constexpr auto s = stdx::span<int>{};
    STATIC_REQUIRE(s.empty());
}

TEST_CASE("span reports empty", "[span]") {
    constexpr auto s1 = stdx::span<int, 4>{};
    STATIC_REQUIRE(not s1.empty());
    constexpr auto s2 = stdx::span<int, 0>{};
    STATIC_REQUIRE(s2.empty());
}

TEST_CASE("span reports size in bytes", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span{a};
    STATIC_REQUIRE(s.size_bytes() == 4 * sizeof(int));
}

TEST_CASE("dynamic span reports size in bytes", "[span]") {
    auto a = std::vector{1, 2, 3, 4};
    auto s = stdx::span{a};
    CHECK(s.size_bytes() == 4 * sizeof(int));
}

TEST_CASE("front and back", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span{a};
    STATIC_REQUIRE(s.front() == 1);
    STATIC_REQUIRE(s.back() == 4);
}

TEST_CASE("indexing", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s = stdx::span{a};
    STATIC_REQUIRE(s[0] == 1);
    STATIC_REQUIRE(s[1] == 2);
    STATIC_REQUIRE(s[2] == 3);
    STATIC_REQUIRE(s[3] == 4);
}

TEST_CASE("prefix of span", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s1 = stdx::span{a};

    constexpr auto s2 = s1.first<2>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(s2), stdx::span<int const, 2> const>);
    STATIC_REQUIRE(s2[0] == 1);
    STATIC_REQUIRE(s2[1] == 2);

    constexpr auto s3 = s1.first(2);
    STATIC_REQUIRE(std::is_same_v<decltype(s3), stdx::span<int const> const>);
    STATIC_REQUIRE(std::size(s3) == 2);
    STATIC_REQUIRE(s3[0] == 1);
    STATIC_REQUIRE(s3[1] == 2);
}

TEST_CASE("prefix of dynamic span", "[span]") {
    auto v = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{v};

    auto s2 = s1.first<2>();
    STATIC_REQUIRE(std::is_same_v<decltype(s2), stdx::span<int, 2>>);
    CHECK(s2[0] == 1);
    CHECK(s2[1] == 2);

    auto s3 = s1.first(2);
    STATIC_REQUIRE(std::is_same_v<decltype(s3), stdx::span<int>>);
    CHECK(std::size(s3) == 2);
    CHECK(s3[0] == 1);
    CHECK(s3[1] == 2);
}

TEST_CASE("suffix of span", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s1 = stdx::span{a};

    constexpr auto s2 = s1.last<2>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(s2), stdx::span<int const, 2> const>);
    STATIC_REQUIRE(s2[0] == 3);
    STATIC_REQUIRE(s2[1] == 4);

    constexpr auto s3 = s1.last(2);
    STATIC_REQUIRE(std::is_same_v<decltype(s3), stdx::span<int const> const>);
    STATIC_REQUIRE(std::size(s3) == 2);
    STATIC_REQUIRE(s3[0] == 3);
    STATIC_REQUIRE(s3[1] == 4);
}

TEST_CASE("suffix of dynamic span", "[span]") {
    auto v = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{v};

    auto s2 = s1.last<2>();
    STATIC_REQUIRE(std::is_same_v<decltype(s2), stdx::span<int, 2>>);
    CHECK(s2[0] == 3);
    CHECK(s2[1] == 4);

    auto s3 = s1.last(2);
    STATIC_REQUIRE(std::is_same_v<decltype(s3), stdx::span<int>>);
    CHECK(std::size(s3) == 2);
    CHECK(s3[0] == 3);
    CHECK(s3[1] == 4);
}

TEST_CASE("subspan of span", "[span]") {
    constexpr static auto a = std::array{1, 2, 3, 4};
    constexpr auto s1 = stdx::span{a};

    constexpr auto s2 = s1.subspan<1, 2>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(s2), stdx::span<int const, 2> const>);
    STATIC_REQUIRE(std::data(s2) == &a[1]);

    constexpr auto s3 = s1.subspan<1>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(s3), stdx::span<int const, 3> const>);
    STATIC_REQUIRE(std::data(s3) == &a[1]);

    constexpr auto s4 = s1.subspan(1, 2);
    STATIC_REQUIRE(std::is_same_v<decltype(s4), stdx::span<int const> const>);
    STATIC_REQUIRE(std::size(s4) == 2);
    STATIC_REQUIRE(std::data(s4) == &a[1]);

    constexpr auto s5 = s1.subspan(1);
    STATIC_REQUIRE(std::is_same_v<decltype(s5), stdx::span<int const> const>);
    STATIC_REQUIRE(std::size(s5) == 3);
    STATIC_REQUIRE(std::data(s5) == &a[1]);

    constexpr auto s6 = s1.subspan<4, 0>();
    STATIC_REQUIRE(
        std::is_same_v<decltype(s6), stdx::span<int const, 0> const>);
    STATIC_REQUIRE(std::data(s6) == stdx::to_address(std::end(a)));
}

TEST_CASE("subspan of dynamic span", "[span]") {
    auto a = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{a};

    auto s2 = s1.subspan<1, 2>();
    STATIC_REQUIRE(std::is_same_v<decltype(s2), stdx::span<int, 2>>);
    CHECK(std::data(s2) == &a[1]);

    auto s3 = s1.subspan<1>();
    STATIC_REQUIRE(std::is_same_v<decltype(s3), stdx::span<int>>);
    CHECK(std::data(s3) == &a[1]);

    auto s4 = s1.subspan(1, 2);
    STATIC_REQUIRE(std::is_same_v<decltype(s4), stdx::span<int>>);
    CHECK(std::size(s4) == 2);
    CHECK(std::data(s4) == &a[1]);

    auto s5 = s1.subspan(1);
    STATIC_REQUIRE(std::is_same_v<decltype(s5), stdx::span<int>>);
    CHECK(std::size(s5) == 3);
    CHECK(std::data(s5) == &a[1]);

    auto s6 = s1.subspan<4, 0>();
    STATIC_REQUIRE(std::is_same_v<decltype(s6), stdx::span<int, 0>>);
    CHECK(std::data(s6) == stdx::to_address(std::end(a)));
}

TEST_CASE("span is explicitly convertible from dynamic to fixed", "[span]") {
    std::vector v{1, 2, 3, 4};
    auto s1 = stdx::span{v};
    auto s2 = stdx::span<int, 4>{s1};
    CHECK(std::data(s1) == std::data(s2));
    CHECK(std::size(s1) == std::size(s2));
}

TEST_CASE("as_bytes on span", "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s1 = stdx::span{a};
    auto s2 = as_bytes(s1);

    STATIC_REQUIRE(
        std::is_same_v<decltype(s2),
                       stdx::span<std::byte const, 4 * sizeof(int)>>);
    CHECK(static_cast<void const *>(std::data(s2)) ==
          static_cast<void const *>(std::data(a)));
}

TEST_CASE("as_bytes on dynamic span", "[span]") {
    auto v = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{v};
    auto s2 = as_bytes(s1);

    STATIC_REQUIRE(std::is_same_v<decltype(s2), stdx::span<std::byte const>>);
    CHECK(static_cast<void const *>(std::data(s2)) ==
          static_cast<void const *>(std::data(v)));
    CHECK(std::size(s2) == 4 * sizeof(int));
}

TEST_CASE("as_writable_bytes on span", "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s1 = stdx::span{a};
    auto s2 = as_writable_bytes(s1);

    STATIC_REQUIRE(
        std::is_same_v<decltype(s2), stdx::span<std::byte, 4 * sizeof(int)>>);
    CHECK(static_cast<void *>(std::data(s2)) ==
          static_cast<void *>(std::data(a)));
}

TEST_CASE("span-over-const is implicitly constructible from span", "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s1 = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s1), stdx::span<int, 4>>);
    auto s2 = [&]() -> stdx::span<int const, 4> { return s1; }();
    CHECK(std::data(s2) == std::data(s1));
}

TEST_CASE("dynamic span-over-const is implicitly constructible from span",
          "[span]") {
    auto a = std::array{1, 2, 3, 4};
    auto s1 = stdx::span{a};
    STATIC_REQUIRE(std::is_same_v<decltype(s1), stdx::span<int, 4>>);
    auto s2 = [&]() -> stdx::span<int const> { return s1; }();
    CHECK(std::data(s2) == std::data(s1));
    CHECK(std::size(s2) == 4u);
}

TEST_CASE(
    "dynamic span-over-const is implicitly constructible from dynamic span",
    "[span]") {
    auto v = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{v};
    STATIC_REQUIRE(std::is_same_v<decltype(s1), stdx::span<int>>);
    auto s2 = [&]() -> stdx::span<int const> { return s1; }();
    CHECK(std::size(s2) == 4u);
}

TEST_CASE("span-over-const is explicitly constructible from dynamic span",
          "[span]") {
    auto v = std::vector{1, 2, 3, 4};
    auto s1 = stdx::span{v};
    STATIC_REQUIRE(std::is_same_v<decltype(s1), stdx::span<int>>);
    auto s2 = stdx::span<int const, 4>{s1};
    CHECK(std::data(s2) == std::data(s1));
}

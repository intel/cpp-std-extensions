#include <stdx/ct_string.hpp>
#include <stdx/udls.hpp>
#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {
struct X;
struct Y;
struct A;
struct B;
struct Z;
} // namespace

TEST_CASE("look up type in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_CHECK(std::is_same_v<stdx::type_lookup_t<M, A>, X>);
    STATIC_CHECK(std::is_same_v<stdx::type_lookup_t<M, B>, Y>);
}

TEST_CASE("look up type not in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_CHECK(std::is_same_v<stdx::type_lookup_t<M, Z>, stdx::missing_t>);
    STATIC_CHECK(std::is_same_v<stdx::type_lookup_t<M, Z, int>, int>);
}

TEST_CASE("look up type in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, X>, stdx::vt_pair<1, Y>>;
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, 0>, X>);
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, 1>, Y>);
}

TEST_CASE("look up type not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, X>, stdx::vt_pair<1, Y>>;
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, 2>, stdx::missing_t>);
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, 2, int>, int>);
}

TEST_CASE("look up type in map (by string value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<"A", X>, stdx::vt_pair<"B", Y>>;
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, "A">, X>);
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, "B">, Y>);
}

TEST_CASE("look up type not in map (by string value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<"A", X>, stdx::vt_pair<"B", Y>>;
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, "C">, stdx::missing_t>);
    STATIC_CHECK(std::is_same_v<stdx::value_lookup_t<M, "C", int>, int>);
}

TEST_CASE("look up value in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<A, 0>, stdx::tv_pair<B, 1>>;
    STATIC_CHECK(stdx::type_lookup_v<M, A> == 0);
    STATIC_CHECK(stdx::type_lookup_v<M, B> == 1);
}

TEST_CASE("look up value not in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<A, 0>, stdx::tv_pair<B, 1>>;
    STATIC_CHECK(stdx::type_lookup_v<M, Z> == stdx::missing);
    STATIC_CHECK(stdx::type_lookup_v<M, Z, 2> == 2);
}

TEST_CASE("look up string value in map (by type)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::tv_pair<A, "X">, stdx::tv_pair<B, "Y">>;
    STATIC_CHECK(stdx::type_lookup_v<M, A> == "X"_cts);
    STATIC_CHECK(stdx::type_lookup_v<M, B> == "Y"_cts);
}

TEST_CASE("look up string value not in map (by type)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::tv_pair<A, "X">, stdx::tv_pair<B, "Y">>;
    STATIC_CHECK(stdx::type_lookup_v<M, Z> == stdx::missing);
    STATIC_CHECK(stdx::type_lookup_v<M, Z, "Z"> == "Z"_cts);
}

TEST_CASE("look up value in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_CHECK(stdx::value_lookup_v<M, 0> == 10);
    STATIC_CHECK(stdx::value_lookup_v<M, 1> == 11);
}

TEST_CASE("look up value not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_CHECK(stdx::value_lookup_v<M, 2> == stdx::missing);
    STATIC_CHECK(stdx::value_lookup_v<M, 2, 3> == 3);
}

TEST_CASE("look up string value in map (by string value)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<"A", "X">, stdx::vv_pair<"B", "Y">>;
    STATIC_CHECK(stdx::value_lookup_v<M, "A"> == "X"_cts);
    STATIC_CHECK(stdx::value_lookup_v<M, "B"> == "Y"_cts);
}

TEST_CASE("look up string value not in map (by string value)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<"A", "X">, stdx::vv_pair<"B", "Y">>;
    STATIC_CHECK(stdx::value_lookup_v<M, "C"> == stdx::missing);
    STATIC_CHECK(stdx::value_lookup_v<M, "C", "Z"> == "Z"_cts);
}

TEST_CASE("reverse look up type in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_CHECK(std::is_same_v<stdx::reverse_type_lookup_t<M, X>, A>);
    STATIC_CHECK(std::is_same_v<stdx::reverse_type_lookup_t<M, Y>, B>);
}

TEST_CASE("reverse look up type not in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_CHECK(
        std::is_same_v<stdx::reverse_type_lookup_t<M, Z>, stdx::missing_t>);
    STATIC_CHECK(std::is_same_v<stdx::reverse_type_lookup_t<M, Z, int>, int>);
}

TEST_CASE("reverse look up type in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, 0>, stdx::tv_pair<Y, 1>>;
    STATIC_CHECK(std::is_same_v<stdx::reverse_value_lookup_t<M, 0>, X>);
    STATIC_CHECK(std::is_same_v<stdx::reverse_value_lookup_t<M, 1>, Y>);
}

TEST_CASE("reverse look up type not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, 0>, stdx::tv_pair<Y, 1>>;
    STATIC_CHECK(
        std::is_same_v<stdx::reverse_value_lookup_t<M, 2>, stdx::missing_t>);
    STATIC_CHECK(std::is_same_v<stdx::reverse_value_lookup_t<M, 2, int>, int>);
}

TEST_CASE("reverse look up type in map (by string value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, "A">, stdx::tv_pair<Y, "B">>;
    STATIC_CHECK(std::is_same_v<stdx::reverse_value_lookup_t<M, "A">, X>);
    STATIC_CHECK(std::is_same_v<stdx::reverse_value_lookup_t<M, "B">, Y>);
}

TEST_CASE("reverse look up type not in map (by string value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, "A">, stdx::tv_pair<Y, "B">>;
    STATIC_CHECK(
        std::is_same_v<stdx::reverse_value_lookup_t<M, "C">, stdx::missing_t>);
    STATIC_CHECK(
        std::is_same_v<stdx::reverse_value_lookup_t<M, "C", int>, int>);
}

TEST_CASE("reverse look up value in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, A>, stdx::vt_pair<1, B>>;
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, A> == 0);
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, B> == 1);
}

TEST_CASE("reverse look up value not in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, A>, stdx::vt_pair<1, B>>;
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, Z> == stdx::missing);
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, Z, 2> == 2);
}

TEST_CASE("reverse look up string value in map (by type)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vt_pair<"A", A>, stdx::vt_pair<"B", B>>;
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, A> == "A"_cts);
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, B> == "B"_cts);
}

TEST_CASE("reverse look up string value not in map (by type)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vt_pair<"A", A>, stdx::vt_pair<"B", B>>;
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, Z> == stdx::missing);
    STATIC_CHECK(stdx::reverse_type_lookup_v<M, Z, "C"> == "C"_cts);
}

TEST_CASE("reverse look up value in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, 10> == 0);
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, 11> == 1);
}

TEST_CASE("reverse look up value not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, 2> == stdx::missing);
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, 2, 3> == 3);
}

TEST_CASE("reverse look up string value in map (by string value)",
          "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<"X", "A">, stdx::vv_pair<"Y", "B">>;
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, "A"> == "X"_cts);
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, "B"> == "Y"_cts);
}

TEST_CASE("reverse look up string value not in map (by string value)",
          "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<"X", "A">, stdx::vv_pair<"Y", "B">>;
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, "C"> == stdx::missing);
    STATIC_CHECK(stdx::reverse_value_lookup_v<M, "C", "Z"> == "Z"_cts);
}

namespace {
template <auto V> struct S {
    [[nodiscard]] friend constexpr auto operator==(S const &, S const &)
        -> bool = default;
};
} // namespace

TEST_CASE("operator[] lookup (type -> type)", "[type map]") {
    using M = stdx::type_map<stdx::tt_pair<S<0>, S<100>>,
                             stdx::tt_pair<S<1>, S<101>>>;
    constexpr auto m = M{};
    STATIC_CHECK(m[S<0>{}] == S<100>{});
    STATIC_CHECK(m[S<1>{}] == S<101>{});
    STATIC_CHECK(m[S<2>{}] == stdx::missing);
}

TEST_CASE("operator[] lookup (type -> type) (incomplete types)", "[type map]") {
    using M = stdx::type_map<stdx::tt_pair<A, X>, stdx::tt_pair<B, Y>>;
    constexpr auto m = M{};
    STATIC_CHECK(m[stdx::type_identity_v<A>] == stdx::type_identity_v<X>);
    STATIC_CHECK(m[stdx::type_identity_v<B>] == stdx::type_identity_v<Y>);
    STATIC_CHECK(m[stdx::type_identity_v<Z>] == stdx::missing);
}

TEST_CASE("operator[] lookup (type -> value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<S<0>, 0>, stdx::tv_pair<S<1>, 1>>;
    constexpr auto m = M{};
    STATIC_CHECK(m[S<0>{}] == 0);
    STATIC_CHECK(m[S<1>{}] == 1);
    STATIC_CHECK(m[S<2>{}] == stdx::missing);
}

TEST_CASE("operator[] lookup (type -> value) (incomplete types)",
          "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<A, 0>, stdx::tv_pair<B, 1>>;
    constexpr auto m = M{};
    STATIC_CHECK(m[stdx::type_identity_v<A>] == 0);
    STATIC_CHECK(m[stdx::type_identity_v<B>] == 1);
    STATIC_CHECK(m[stdx::type_identity_v<Z>] == stdx::missing);
}

TEST_CASE("operator() lookup (value -> type)", "[type map]") {
    using namespace stdx::literals;
    using M =
        stdx::type_map<stdx::vt_pair<0u, S<100>>, stdx::vt_pair<1u, S<101>>>;
    constexpr auto m = M{};
    STATIC_CHECK(m(0_c) == S<100>{});
    STATIC_CHECK(m(1_c) == S<101>{});
    STATIC_CHECK(m(2_c) == stdx::missing);
}

TEST_CASE("operator() lookup (value -> type) (incomplete types)",
          "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vt_pair<0u, A>, stdx::vt_pair<1u, B>>;
    constexpr auto m = M{};
    STATIC_CHECK(m(0_c) == stdx::type_identity_v<A>);
    STATIC_CHECK(m(1_c) == stdx::type_identity_v<B>);
    STATIC_CHECK(m(2_c) == stdx::missing);
}

TEST_CASE("operator() lookup (value -> value)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<0u, 100>, stdx::vv_pair<1u, 101>>;
    constexpr auto m = M{};
    STATIC_CHECK(m(0_c) == 100);
    STATIC_CHECK(m(1_c) == 101);
    STATIC_CHECK(m(2_c) == stdx::missing);
}

TEST_CASE("operator() lookup (string -> string)", "[type map]") {
    using namespace stdx::literals;
    using M = stdx::type_map<stdx::vv_pair<"A", "X">, stdx::vv_pair<"B", "Y">>;
    constexpr auto m = M{};
    STATIC_CHECK(m("A"_ctst) == "X"_cts);
    STATIC_CHECK(m("B"_ctst) == "Y"_cts);
    STATIC_CHECK(m("C"_ctst) == stdx::missing);
}

TEST_CASE("multi-depth lookup", "[type map]") {
    using namespace stdx::literals;
    using P = stdx::type_map<stdx::vv_pair<"1", "L">, stdx::vv_pair<"2", "M">>;
    using Q = stdx::type_map<stdx::vv_pair<"1", "N">, stdx::vv_pair<"2", "O">>;
    using M = stdx::type_map<stdx::vt_pair<"A", P>, stdx::vt_pair<"B", Q>>;
    constexpr auto m = M{};
    STATIC_CHECK(m("A"_ctst)("1"_ctst) == "L"_cts);
    STATIC_CHECK(m("A"_ctst)("2"_ctst) == "M"_cts);
    STATIC_CHECK(m("B"_ctst)("1"_ctst) == "N"_cts);
    STATIC_CHECK(m("B"_ctst)("2"_ctst) == "O"_cts);
}

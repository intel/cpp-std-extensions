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
    STATIC_REQUIRE(std::is_same_v<stdx::type_lookup_t<M, A>, X>);
    STATIC_REQUIRE(std::is_same_v<stdx::type_lookup_t<M, B>, Y>);
}

TEST_CASE("look up type not in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_REQUIRE(std::is_same_v<stdx::type_lookup_t<M, Z>, void>);
    STATIC_REQUIRE(std::is_same_v<stdx::type_lookup_t<M, Z, int>, int>);
}

TEST_CASE("look up type in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, X>, stdx::vt_pair<1, Y>>;
    STATIC_REQUIRE(std::is_same_v<stdx::value_lookup_t<M, 0>, X>);
    STATIC_REQUIRE(std::is_same_v<stdx::value_lookup_t<M, 1>, Y>);
}

TEST_CASE("look up type not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, X>, stdx::vt_pair<1, Y>>;
    STATIC_REQUIRE(std::is_same_v<stdx::value_lookup_t<M, 2>, void>);
    STATIC_REQUIRE(std::is_same_v<stdx::value_lookup_t<M, 2, int>, int>);
}

TEST_CASE("look up value in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<A, 0>, stdx::tv_pair<B, 1>>;
    STATIC_REQUIRE(stdx::type_lookup_v<M, A> == 0);
    STATIC_REQUIRE(stdx::type_lookup_v<M, B> == 1);
}

TEST_CASE("look up value not in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<A, 0>, stdx::tv_pair<B, 1>>;
    STATIC_REQUIRE(stdx::type_lookup_v<M, Z> == 0);
    STATIC_REQUIRE(stdx::type_lookup_v<M, Z, 2> == 2);
}

TEST_CASE("look up value in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_REQUIRE(stdx::value_lookup_v<M, 0> == 10);
    STATIC_REQUIRE(stdx::value_lookup_v<M, 1> == 11);
}

TEST_CASE("look up value not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_REQUIRE(stdx::value_lookup_v<M, 2> == 0);
    STATIC_REQUIRE(stdx::value_lookup_v<M, 2, 3> == 3);
}

TEST_CASE("reverse look up type in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_type_lookup_t<M, X>, A>);
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_type_lookup_t<M, Y>, B>);
}

TEST_CASE("reverse look up type not in map", "[type map]") {
    using M = stdx::type_map<stdx::type_pair<A, X>, stdx::type_pair<B, Y>>;
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_type_lookup_t<M, Z>, void>);
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_type_lookup_t<M, Z, int>, int>);
}

TEST_CASE("reverse look up type in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, 0>, stdx::tv_pair<Y, 1>>;
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_value_lookup_t<M, 0>, X>);
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_value_lookup_t<M, 1>, Y>);
}

TEST_CASE("reverse look up type not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::tv_pair<X, 0>, stdx::tv_pair<Y, 1>>;
    STATIC_REQUIRE(std::is_same_v<stdx::reverse_value_lookup_t<M, 2>, void>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::reverse_value_lookup_t<M, 2, int>, int>);
}

TEST_CASE("reverse look up value in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, A>, stdx::vt_pair<1, B>>;
    STATIC_REQUIRE(stdx::reverse_type_lookup_v<M, A> == 0);
    STATIC_REQUIRE(stdx::reverse_type_lookup_v<M, B> == 1);
}

TEST_CASE("reverse look up value not in map (by type)", "[type map]") {
    using M = stdx::type_map<stdx::vt_pair<0, A>, stdx::vt_pair<1, B>>;
    STATIC_REQUIRE(stdx::reverse_type_lookup_v<M, Z> == 0);
    STATIC_REQUIRE(stdx::reverse_type_lookup_v<M, Z, 2> == 2);
}

TEST_CASE("reverse look up value in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_REQUIRE(stdx::reverse_value_lookup_v<M, 10> == 0);
    STATIC_REQUIRE(stdx::reverse_value_lookup_v<M, 11> == 1);
}

TEST_CASE("reverse look up value not in map (by value)", "[type map]") {
    using M = stdx::type_map<stdx::vv_pair<0, 10>, stdx::vv_pair<1, 11>>;
    STATIC_REQUIRE(stdx::reverse_value_lookup_v<M, 2> == 0);
    STATIC_REQUIRE(stdx::reverse_value_lookup_v<M, 2, 3> == 3);
}

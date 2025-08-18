#include <stdx/ct_string.hpp>
#include <stdx/env.hpp>

#include <boost/mp11/algorithm.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {
[[maybe_unused]] constexpr inline struct custom_t {
    template <typename T>
        requires true // more constrained
    [[nodiscard]] CONSTEVAL auto operator()(T &&t) const
        noexcept(noexcept(std::forward<T>(t).query(std::declval<custom_t>())))
            -> decltype(std::forward<T>(t).query(*this)) {
        return std::forward<T>(t).query(*this);
    }

    [[nodiscard]] CONSTEVAL auto operator()(auto &&) const { return 42; }
} custom;
} // namespace

TEST_CASE("lookup query with internal default", "[env]") {
    STATIC_REQUIRE(custom(stdx::env<>{}) == 42);
}

TEST_CASE("lookup query with default (free function)", "[env]") {
    STATIC_REQUIRE(stdx::query<stdx::env<>>(custom) == 42);
}

namespace {
[[maybe_unused]] constexpr inline struct custom_no_default_t {
    template <typename T>
    [[nodiscard]] CONSTEVAL auto operator()(T &&t) const
        noexcept(noexcept(std::forward<T>(t).query(std::declval<custom_t>())))
            -> decltype(std::forward<T>(t).query(*this)) {
        return std::forward<T>(t).query(*this);
    }
} custom_no_default;
} // namespace

TEST_CASE("lookup query with external default", "[env]") {
    STATIC_REQUIRE(stdx::query<stdx::env<>>(custom_no_default, 42) == 42);
}

TEST_CASE("make an environment", "[env]") {
    using E = stdx::make_env_t<custom, 17>;
    STATIC_REQUIRE(custom(E{}) == 17);
}

TEST_CASE("extend an environment", "[env]") {
    using E1 = stdx::make_env_t<custom, 17>;
    using E2 = stdx::extend_env_t<E1, custom, 18>;
    STATIC_REQUIRE(custom(E2{}) == 18);
}

TEST_CASE("append an environment", "[env]") {
    using E1 = stdx::make_env_t<custom, 17>;
    using E2 = stdx::make_env_t<custom, 18>;
    using E3 = stdx::make_env_t<custom, 19>;
    using E = stdx::append_env_t<E1, E2, E3>;
    STATIC_REQUIRE(custom(E{}) == 19);
}

TEST_CASE("environment converts string literals to ct_string", "[env]") {
    using namespace stdx::literals;
    using E = stdx::make_env_t<custom, "hello">;
    STATIC_REQUIRE(custom(E{}) == "hello"_cts);
}

TEST_CASE("envlike concept", "[env]") {
    STATIC_REQUIRE(stdx::envlike<stdx::env<>>);
    STATIC_REQUIRE(stdx::envlike<stdx::make_env_t<custom, 17>>);
}

namespace {
template <typename Q> struct match_query {
    template <typename T> using fn = std::is_same<Q, typename T::query_t>;
};
} // namespace

TEST_CASE("extending environment doesn't create duplicate keys", "[env]") {
    using E1 = stdx::make_env_t<custom, 17>;
    using E2 = stdx::extend_env_t<E1, custom, 18>;
    STATIC_REQUIRE(
        boost::mp11::mp_count_if_q<E2, match_query<custom_t>>::value == 1);
}

TEST_CASE("appending environment doesn't create duplicate keys", "[env]") {
    using E1 = stdx::make_env_t<custom, 17>;
    using E2 = stdx::make_env_t<custom, 18>;
    using E3 = stdx::make_env_t<custom, 19>;
    using E = stdx::append_env_t<E1, E2, E3>;
    STATIC_REQUIRE(
        boost::mp11::mp_count_if_q<E, match_query<custom_t>>::value == 1);
}

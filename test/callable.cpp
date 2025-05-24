#include <stdx/type_traits.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace {
[[maybe_unused]] auto func_no_args() {}
[[maybe_unused]] auto func_one_arg(int) {}

struct S {
    auto operator()() {}
};
struct T {
    template <typename> auto operator()() {}
};
} // namespace

TEST_CASE("is_function", "[type_traits]") {
    auto x = []() {};
    STATIC_REQUIRE(not stdx::is_function_v<int>);
    STATIC_REQUIRE(not stdx::is_function_v<int &>);
    STATIC_REQUIRE(not stdx::is_function_v<decltype(x)>);
    STATIC_REQUIRE(stdx::is_function_v<decltype(func_no_args)>);
    STATIC_REQUIRE(stdx::is_function_v<decltype(func_one_arg)>);
}

TEST_CASE("is_function_object", "[type_traits]") {
    auto x = []() {};
    auto y = [](int) {};
    auto z = [](auto) {};
    STATIC_REQUIRE(not stdx::is_function_object_v<int>);
    STATIC_REQUIRE(not stdx::is_function_object_v<decltype(func_no_args)>);
    STATIC_REQUIRE(not stdx::is_function_object_v<decltype(func_one_arg)>);
    STATIC_REQUIRE(stdx::is_function_object_v<decltype(x)>);
    STATIC_REQUIRE(stdx::is_function_object_v<decltype(y)>);
    STATIC_REQUIRE(stdx::is_function_object_v<decltype(z)>);
    STATIC_REQUIRE(stdx::is_function_object_v<S>);
    STATIC_REQUIRE(stdx::is_function_object_v<T>);
}

TEST_CASE("is_callable", "[type_traits]") {
    auto x = []() {};
    auto y = [](int) {};
    auto z = [](auto) {};
    STATIC_REQUIRE(not stdx::is_callable_v<int>);
    STATIC_REQUIRE(stdx::is_callable_v<decltype(func_no_args)>);
    STATIC_REQUIRE(stdx::is_callable_v<decltype(func_one_arg)>);
    STATIC_REQUIRE(stdx::is_callable_v<decltype(x)>);
    STATIC_REQUIRE(stdx::is_callable_v<decltype(y)>);
    STATIC_REQUIRE(stdx::is_callable_v<decltype(z)>);
    STATIC_REQUIRE(stdx::is_callable_v<S>);
    STATIC_REQUIRE(stdx::is_callable_v<T>);
}

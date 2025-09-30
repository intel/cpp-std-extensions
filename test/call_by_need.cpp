#include "detail/tuple_types.hpp"

#include <stdx/call_by_need.hpp>
#include <stdx/tuple.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {
template <auto> struct arg_t {};
template <auto V> constexpr auto arg = arg_t<V>{};

template <auto V1, auto V2>
constexpr auto operator==(arg_t<V1>, arg_t<V2>) -> bool {
    if constexpr (std::is_same_v<stdx::remove_cvref_t<decltype(V1)>,
                                 stdx::remove_cvref_t<decltype(V2)>>) {
        return V1 == V2;
    }
    return false;
}
} // namespace

TEST_CASE("single function, exact args", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }}, stdx::tuple{arg<0>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
}

TEST_CASE("single function, drop void", "[call_by_need]") {
    auto called = 0;
    auto const r = stdx::call_by_need(stdx::tuple{[&](arg_t<0>) { ++called; }},
                                      stdx::tuple{arg<0>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<> const>);
    CHECK(called == 1);
}

TEST_CASE("single function, extra args (beginning)", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(stdx::tuple{[](arg_t<1>) {}},
                                          stdx::tuple{arg<0>, arg<1>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<arg_t<0>> const>);
}

TEST_CASE("single function, extra args (end)", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(stdx::tuple{[](arg_t<0>) {}},
                                          stdx::tuple{arg<0>, arg<1>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<arg_t<1>> const>);
}

TEST_CASE("multi function, exact args", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }, [](arg_t<1>) { return 18; }},
        stdx::tuple{arg<0>, arg<1>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int, int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("multi function, drop void", "[call_by_need]") {
    auto called = 0;
    auto const r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }, [&](arg_t<1>) { ++called; }},
        stdx::tuple{arg<0>, arg<1>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    CHECK(get<0>(r) == 17);
    CHECK(called == 1);
}

TEST_CASE("multi function, extra args (beginning)", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<1>) { return 17; }, [](arg_t<2>) { return 18; }},
        stdx::tuple{arg<0>, arg<1>, arg<2>});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, int, arg_t<0>> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("multi function, extra args (middle)", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }, [](arg_t<2>) { return 18; }},
        stdx::tuple{arg<0>, arg<1>, arg<2>});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, int, arg_t<1>> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("multi function, extra args (end)", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }, [](arg_t<1>) { return 18; }},
        stdx::tuple{arg<0>, arg<1>, arg<2>});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, int, arg_t<2>> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("multi function, same args", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](arg_t<0>) { return 17; }, [](arg_t<0>) { return 18; }},
        stdx::tuple{arg<0>, arg<1>});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, int, arg_t<1>> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("multi function, overlapping args", "[call_by_need]") {
    constexpr auto r =
        stdx::call_by_need(stdx::tuple{[](arg_t<0>, arg_t<1>) { return 17; },
                                       [](arg_t<1>, arg_t<2>) { return 18; }},
                           stdx::tuple{arg<0>, arg<1>, arg<2>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int, int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
    STATIC_REQUIRE(get<1>(r) == 18);
}

TEST_CASE("move-only arg", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](move_only) { return 17; }}, stdx::tuple{move_only{17}});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
}

TEST_CASE("move-only arg (by reference)", "[call_by_need]") {
    auto t = stdx::tuple{move_only{1}};
    auto const r =
        stdx::call_by_need(stdx::tuple{[](move_only &) { return 17; }}, t);
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    CHECK(get<0>(r) == 17);
}

TEST_CASE("move-only return", "[call_by_need]") {
    constexpr auto r =
        stdx::call_by_need(stdx::tuple{[](arg_t<0>) { return move_only{17}; }},
                           stdx::tuple{arg<0>});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<move_only> const>);
    STATIC_REQUIRE(get<0>(r).value == 17);
}

TEST_CASE("converted arguments", "[call_by_need]") {
    auto const r = stdx::call_by_need(stdx::tuple{[](char c) {
                                                      CHECK(c == 'a');
                                                      return 17;
                                                  },
                                                  [](int i) {
                                                      CHECK(i == 'a');
                                                      return 18;
                                                  }},
                                      stdx::tuple{'a', 42});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, int, int> const>);
    CHECK(get<0>(r) == 17);
    CHECK(get<1>(r) == 18);
    CHECK(get<2>(r) == 42);
}

TEST_CASE("nullary function, no args", "[call_by_need]") {
    auto called = 0;
    auto const r = stdx::call_by_need(stdx::tuple{[&] {
                                          ++called;
                                          return 17;
                                      }},
                                      stdx::tuple{});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    CHECK(called == 1);
}

TEST_CASE("nullary function, passthrough arg", "[call_by_need]") {
    auto called = 0;
    auto const r = stdx::call_by_need(stdx::tuple{[&] {
                                          ++called;
                                          return 17;
                                      }},
                                      stdx::tuple{arg<0>});
    STATIC_REQUIRE(
        std::is_same_v<decltype(r), stdx::tuple<int, arg_t<0>> const>);
    CHECK(called == 1);
}

TEST_CASE("default arguments unfilled", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](int i = 17) { return i; }}, stdx::tuple{});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
}

TEST_CASE("default arguments filled", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(
        stdx::tuple{[](int i = 17) { return i; }}, stdx::tuple{18});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    STATIC_REQUIRE(get<0>(r) == 18);
}

TEST_CASE("no functions given", "[call_by_need]") {
    constexpr auto r = stdx::call_by_need(stdx::tuple{}, stdx::tuple{17});
    STATIC_REQUIRE(std::is_same_v<decltype(r), stdx::tuple<int> const>);
    STATIC_REQUIRE(get<0>(r) == 17);
}

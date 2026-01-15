#include <stdx/compiler.hpp>
#include <stdx/pp_map.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {
auto count_args = [](auto const &...args) { return sizeof...(args); };
} // namespace

TEST_CASE("pp_map zero arguments", "[pp_map]") {
#ifdef __clang__
    STDX_PRAGMA(diagnostic push)
    STDX_PRAGMA(diagnostic ignored "-Wgnu-zero-variadic-macro-arguments")
    STDX_PRAGMA(diagnostic ignored "-Wc++20-extensions")
#endif
    static_assert(count_args(STDX_MAP(int)) == 0);
#ifdef __clang__
    STDX_PRAGMA(diagnostic pop)
#endif
}

TEST_CASE("pp_map one argument", "[pp_map]") {
    static_assert(count_args(STDX_MAP(double, 1)) == 1.0);
}

TEST_CASE("pp_map n arguments", "[pp_map]") {
    static_assert(count_args(STDX_MAP(double, 1, 2, 3)) == 3);
}

TEST_CASE("pp_map parenthesized arguments", "[pp_map]") {
    static_assert(count_args(STDX_MAP(double, ((void)1, 2), 3)) == 2);
}

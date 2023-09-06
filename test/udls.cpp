#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time named bools", "[utility]") {
    using namespace stdx::literals;
    static_assert("variable"_b);
    static_assert(not(not "variable"_b));

    static_assert("variable"_true);
    static_assert(not "variable"_false);
}

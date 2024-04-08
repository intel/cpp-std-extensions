#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compile-time named bools", "[utility]") {
    using namespace stdx::literals;
    static_assert("variable"_b);
    static_assert(not(not "variable"_b));

    static_assert("variable"_true);
    static_assert(not "variable"_false);
}

TEST_CASE("decimal units", "[units]") {
    using namespace stdx::literals;
    static_assert(1_k == 1'000ull);
    static_assert(1_M == 1'000'000ull);
    static_assert(1_G == 1'000'000'000ull);
}

TEST_CASE("binary units", "[units]") {
    using namespace stdx::literals;
    static_assert(1_ki == 1'024ull);
    static_assert(1_Mi == 1'024ull * 1'024ull);
    static_assert(1_Gi == 1'024ull * 1'024ull * 1'024ull);
}

TEST_CASE("compile-time named small indices", "[units]") {
    using namespace stdx::literals;
    static_assert(std::is_same_v<decltype("index"_0),
                                 std::integral_constant<size_t, 0u>>);
    static_assert("index"_0 == 0u);
    static_assert("index"_1 == 1u);
    static_assert("index"_2 == 2u);
    static_assert("index"_3 == 3u);
    static_assert("index"_4 == 4u);
    static_assert("index"_5 == 5u);
    static_assert("index"_6 == 6u);
    static_assert("index"_7 == 7u);
    static_assert("index"_8 == 8u);
    static_assert("index"_9 == 9u);
}

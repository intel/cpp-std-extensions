#include <stdx/type_traits.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace {
enum UnscopedEnum { Value3 = 3 };
enum UnscopedEnumWithUnderlying : short int { Value4 = 4 };
enum struct ScopedEnum : char { Value5 = 5 };
} // namespace

TEST_CASE("to_underlying types", "[type_traits]") {
    CHECK(std::is_same_v<decltype(stdx::to_underlying(Value3)),
                         std::underlying_type_t<UnscopedEnum>>);
    CHECK(std::is_same_v<decltype(stdx::to_underlying(Value4)),
                         std::underlying_type_t<UnscopedEnumWithUnderlying>>);
    CHECK(std::is_same_v<decltype(stdx::to_underlying(ScopedEnum::Value5)),
                         std::underlying_type_t<ScopedEnum>>);
}

TEST_CASE("to_underlying values", "[type_traits]") {
    CHECK(stdx::to_underlying(Value3) == 3);
    CHECK(stdx::to_underlying(Value4) == 4);
    CHECK(stdx::to_underlying(ScopedEnum::Value5) == 5);
}

TEST_CASE("to_underlying works on integral types", "[type_traits]") {
    constexpr int x = 0;
    CHECK(std::is_same_v<decltype(stdx::to_underlying(x)), int>);
    CHECK(stdx::to_underlying(x) == 0);
}

#include <stdx/atomic_bitset.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <type_traits>

TEST_CASE("atomic_bitset works with overridden type",
          "[atomic_bitset_override]") {
    auto bs = stdx::atomic_bitset<4>{};
    static_assert(sizeof(decltype(bs)) == sizeof(std::uint32_t));
    static_assert(alignof(decltype(bs)) == alignof(std::uint32_t));
}

TEST_CASE("to_natural returns smallest_uint", "[atomic_bitset_override]") {
    auto bs = stdx::atomic_bitset<4>{stdx::all_bits};
    auto value = bs.to_natural();
    CHECK(value == 0b1111);
    static_assert(std::is_same_v<decltype(value), std::uint8_t>);
}

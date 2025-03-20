#include <stdx/atomic.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <type_traits>

TEST_CASE("atomic with overridden type is correctly sized/aligned",
          "[atomic_override]") {
    auto bs = stdx::atomic<bool>{};
    static_assert(sizeof(decltype(bs)) == sizeof(std::uint32_t));
    static_assert(alignof(decltype(bs)) == alignof(std::uint32_t));
}

TEST_CASE("atomic with overridden type presents interface of original type",
          "[atomic_override]") {
    auto bs = stdx::atomic<bool>{};
    static_assert(std::is_same_v<decltype(bs.load()), bool>);
}

TEST_CASE("atomic works with overridden type", "[atomic_override]") {
    auto bs = stdx::atomic<bool>{};
    CHECK(!bs);
    CHECK(!bs.exchange(true));
    CHECK(bs);
}

TEST_CASE("atomic config works with partial specialization",
          "[atomic_override]") {
    using elem_t = ::atomic::atomic_type_t<int *>;
    static_assert(std::is_same_v<elem_t, uintptr_t>);
}

#if __cplusplus >= 202002L
namespace {
enum E : std::uint8_t {};
}

TEST_CASE("atomic config works with enum", "[atomic_override]") {
    auto bs = stdx::atomic<E>{};
    static_assert(sizeof(decltype(bs)) == sizeof(std::uint32_t));
    static_assert(alignof(decltype(bs)) == alignof(std::uint32_t));
}
#endif

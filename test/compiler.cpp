#include <stdx/compiler.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("using attribute namespace", "[compiler]") {
    [[USING_ATTR_NS warn_unused_result]] auto f(int) -> int;
}

ALWAYS_INLINE auto f1(int) -> void;

TEST_CASE("NEVER_INLINE", "[compiler]") { NEVER_INLINE auto f2(int) -> void; }

TEST_CASE("FLATTEN", "[compiler]") { FLATTEN auto f3(int) -> void; }

ALWAYS_INLINE FLATTEN auto f4(int) -> void;

#include <stdx/utility.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("forward_like", "[utility]") {
    static_assert(std::is_same_v<stdx::forward_like_t<int, float>, float &&>);
    static_assert(
        std::is_same_v<stdx::forward_like_t<int const, float>, float const &&>);

    static_assert(
        std::is_same_v<stdx::forward_like_t<int &&, float>, float &&>);
    static_assert(std::is_same_v<stdx::forward_like_t<int const &&, float>,
                                 float const &&>);

    static_assert(std::is_same_v<stdx::forward_like_t<int &, float>, float &>);
    static_assert(std::is_same_v<stdx::forward_like_t<int const &, float>,
                                 float const &>);
}

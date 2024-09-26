#include <stdx/ct_string.hpp>
#include <stdx/panic.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string_view>

namespace {
int runtime_calls{};
#if __cplusplus >= 202002L
int compile_time_calls{};
#endif

struct injected_handler {
    template <typename Why, typename... Ts>
    static auto panic(Why why, Ts &&...) noexcept -> void {
        CHECK(std::string_view{why} == "uh-oh");
        ++runtime_calls;
    }

#if __cplusplus >= 202002L
    template <stdx::ct_string Why, typename... Ts>
    static auto panic(Ts &&...) noexcept -> void {
        static_assert(std::string_view{Why} == "uh-oh");
        ++compile_time_calls;
    }
#endif
};
} // namespace

template <> inline auto stdx::panic_handler<> = injected_handler{};

TEST_CASE("panic called with runtime arguments", "[panic]") {
    runtime_calls = 0;
    stdx::panic("uh-oh");
    CHECK(runtime_calls == 1);
}

#if __cplusplus >= 202002L
TEST_CASE("panic called with compile-time strings", "[panic]") {
    compile_time_calls = 0;
    using namespace stdx::ct_string_literals;
    stdx::panic<"uh-oh"_cts>();
    CHECK(compile_time_calls == 1);
}

TEST_CASE("compile-time panic called through macro", "[panic]") {
    compile_time_calls = 0;
    STDX_PANIC("uh-oh");
    CHECK(compile_time_calls == 1);
}
#else
TEST_CASE("runtime panic called through macro", "[panic]") {
    runtime_calls = 0;
    STDX_PANIC("uh-oh");
    CHECK(runtime_calls == 1);
}
#endif

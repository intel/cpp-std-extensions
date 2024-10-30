#include <stdx/static_assert.hpp>
#include <stdx/utility.hpp>

#include <string_view>

// EXPECT: hello world int 123

template <typename T> constexpr auto f() {
    using namespace std::string_view_literals;
    STATIC_ASSERT(false, "hello {} {} {}", CX_VALUE("world"sv), CX_VALUE(T),
                  CX_VALUE(123));
}

auto main() -> int { f<int>(); }

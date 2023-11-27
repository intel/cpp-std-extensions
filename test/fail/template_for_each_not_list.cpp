#include <stdx/type_traits.hpp>

// EXPECT: must be called with a type list

auto main() -> int {
    stdx::template_for_each<int>([] {});
}

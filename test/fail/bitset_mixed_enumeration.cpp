#include <stdx/bitset.hpp>

// EXPECT: T is not the required enumeration type

enum struct E1 { A, B, C, MAX };
enum struct E2 { X, Y, Z };

auto main() -> int {
    auto b = stdx::bitset<E1::MAX>{};
    b.set(E2::X);
}

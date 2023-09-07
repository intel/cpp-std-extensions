#pragma once

#include <stdx/compiler.hpp>

#include <cstddef>

namespace stdx {
inline namespace v1 {

template <typename... Fs> struct overload : Fs... {
    using Fs::operator()...;
};

#if __cpp_deduction_guides < 201907L
template <typename... Fs> overload(Fs...) -> overload<Fs...>;
#endif

namespace literals {
CONSTEVAL auto operator""_b(char const *, std::size_t) -> bool { return true; }
CONSTEVAL auto operator""_true(char const *, std::size_t) -> bool {
    return true;
}
CONSTEVAL auto operator""_false(char const *, std::size_t) -> bool {
    return false;
}
} // namespace literals

[[noreturn]] inline auto unreachable() -> void { __builtin_unreachable(); }

} // namespace v1
} // namespace stdx

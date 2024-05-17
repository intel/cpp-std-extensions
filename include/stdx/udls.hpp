#pragma once

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace stdx {
inline namespace v1 {

namespace detail {
template <typename T, char... Chars> CONSTEVAL auto decimal() -> T {
    static_assert((... and (Chars >= '0' and Chars <= '9')),
                  "decimal numbers only are supported");
    using U = decltype(stdx::to_underlying(std::declval<T>()));
    auto x = U{};
    ((x *= 10, x += Chars - '0'), ...);
    return T{x};
}
} // namespace detail

template <auto I> using constant = std::integral_constant<decltype(I), I>;
template <auto I> constexpr static constant<I> _c{};

inline namespace literals {
template <char... Chars> CONSTEVAL auto operator""_c() {
    return _c<detail::decimal<std::uint32_t, Chars...>()>;
}
} // namespace literals

enum struct lsb_t : std::uint32_t {};
enum struct msb_t : std::uint32_t {};
enum struct length_t : std::uint32_t {};

inline namespace literals {
// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto operator""_lsb(unsigned long long int n) -> lsb_t {
    return static_cast<lsb_t>(n);
}
CONSTEVAL auto operator""_msb(unsigned long long int n) -> msb_t {
    return static_cast<msb_t>(n);
}
CONSTEVAL auto operator""_len(unsigned long long int n) -> length_t {
    return static_cast<length_t>(n);
}
// NOLINTEND(google-runtime-int)
} // namespace literals

inline namespace literals {
CONSTEVAL auto operator""_b(char const *, std::size_t) -> bool { return true; }
CONSTEVAL auto operator""_true(char const *, std::size_t) -> bool {
    return true;
}
CONSTEVAL auto operator""_false(char const *, std::size_t) -> bool {
    return false;
}

// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto
operator""_k(unsigned long long int n) -> unsigned long long int {
    return n * 1'000u;
}
CONSTEVAL auto
operator""_M(unsigned long long int n) -> unsigned long long int {
    return n * 1'000'000u;
}
CONSTEVAL auto
operator""_G(unsigned long long int n) -> unsigned long long int {
    return n * 1'000'000'000u;
}

CONSTEVAL auto
operator""_ki(unsigned long long int n) -> unsigned long long int {
    return n * 1'024u;
}
CONSTEVAL auto
operator""_Mi(unsigned long long int n) -> unsigned long long int {
    return n * 1'024ull * 1'024ull;
}
CONSTEVAL auto
operator""_Gi(unsigned long long int n) -> unsigned long long int {
    return n * 1'024ull * 1'024ull * 1'024ull;
}

#define STDX_SMALL_INT_LITERAL_DEF(x)                                          \
    CONSTEVAL auto operator""_##x(char const *, std::size_t)                   \
        ->std::integral_constant<std::size_t, x##u> {                          \
        return {};                                                             \
    }

STDX_SMALL_INT_LITERAL_DEF(0)
STDX_SMALL_INT_LITERAL_DEF(1)
STDX_SMALL_INT_LITERAL_DEF(2)
STDX_SMALL_INT_LITERAL_DEF(3)
STDX_SMALL_INT_LITERAL_DEF(4)
STDX_SMALL_INT_LITERAL_DEF(5)
STDX_SMALL_INT_LITERAL_DEF(6)
STDX_SMALL_INT_LITERAL_DEF(7)
STDX_SMALL_INT_LITERAL_DEF(8)
STDX_SMALL_INT_LITERAL_DEF(9)

#undef STDX_SMALL_INT_LITERAL_DEF

// NOLINTEND(google-runtime-int)
} // namespace literals

} // namespace v1
} // namespace stdx

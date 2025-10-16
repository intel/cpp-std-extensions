#pragma once

#include <stdx/detail/freestanding.hpp>

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifndef STDX_FREESTANDING

#include <fmt/compile.h>
#include <fmt/format.h>

#include <string_view>

#define STDX_FMT_COMPILE(Fmt) FMT_COMPILE(std::string_view{Fmt})

#else

#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/ranges.hpp>

#include <cstddef>

#define STDX_FMT_COMPILE(X) [] { return X; }

namespace stdx {
inline namespace v1 {
namespace fmt {

namespace detail {
struct fmt_spec {
    int len{};
    int base{};
    bool well_formed{true};
};

CONSTEVAL auto parse_fmt_spec(auto fmtstr) -> fmt_spec {
    constexpr auto fmt = fmtstr();
    auto i = fmt.begin();
    while (*i != '{') {
        ++i;
    }
    ++i;
    if (*i == '}') {
        return {.len = 2, .base = 10};
    } else if (*i++ == ':' and *i++ == 'x' and *i == '}') {
        return {.len = 4, .base = 16};
    }
    return {.well_formed = false};
}

template <stdx::range R>
CONSTEVAL auto formatted_size(fmt_spec, R const &r) -> std::size_t {
    return r.size();
}

CONSTEVAL auto formatted_size(fmt_spec, char const *str) -> std::size_t {
    std::size_t sz{};
    while (*str) {
        ++sz;
        ++str;
    }
    return sz;
}

CONSTEVAL auto formatted_size(fmt_spec, char) -> std::size_t { return 1; }

template <stdx::integral I>
CONSTEVAL auto formatted_size(fmt_spec s, I i) -> std::size_t {
    if (i == 0) {
        return 1;
    } else {
        std::size_t sz{};
        if (i < 0) {
            ++sz;
        }

        while (i != 0) {
            ++sz;
            i /= s.base;
        }
        return sz;
    }
}
} // namespace detail

CONSTEVAL auto formatted_size(auto fmtstr, auto v) -> std::size_t {
    constexpr auto spec = detail::parse_fmt_spec(fmtstr);
    static_assert(
        spec.well_formed,
        "Freestanding fmt implementation does not support that format "
        "specifier");

    return fmtstr().size() - spec.len + detail::formatted_size(spec, v);
}

namespace detail {
template <typename It, stdx::range R>
CONSTEVAL auto format_to(fmt_spec, It dest, R const &r) -> It {
    for (auto const c : r) {
        *dest++ = c;
    }
    return dest;
}

template <typename It>
CONSTEVAL auto format_to(fmt_spec, It dest, char const *str) -> It {
    while (*str) {
        *dest++ = *str++;
    }
    return dest;
}

template <typename It>
CONSTEVAL auto format_to(fmt_spec, It dest, char c) -> It {
    *dest++ = c;
    return dest;
}

template <typename It, stdx::integral I>
CONSTEVAL auto format_to(fmt_spec s, It dest, I i) -> It {
    if (i == 0) {
        *dest++ = '0';
    } else {
        auto abs = []<typename T>(T v) -> T { return v < 0 ? -v : v; };
        auto to_digit = [](auto n) -> char { return "0123456789abcdef"[n]; };

        if (i < 0) {
            *dest++ = '-';
        }

        auto b = dest;
        while (i != 0) {
            auto digit = to_digit(abs(i % s.base));
            i /= s.base;
            *dest++ = digit;
        }

        auto e = dest - 1;
        while (b < e) {
            auto tmp = *b;
            *b++ = *e;
            *e-- = tmp;
        }
    }
    return dest;
}

} // namespace detail

template <typename It>
CONSTEVAL auto format_to(It dest, auto fmtstr, auto v) -> void {
    constexpr auto fmt = fmtstr();
    constexpr auto spec = detail::parse_fmt_spec(fmtstr);

    // copy to opening {
    auto src = fmt.begin();
    while (*src != '{') {
        *dest++ = *src++;
    }

    // skip fmt spec
    src += spec.len;

    // format the value into the buffer
    dest = detail::format_to(spec, dest, v);

    // copy the rest of src
    while (*src) {
        *dest++ = *src++;
    }
}

} // namespace fmt
} // namespace v1
} // namespace stdx

#endif

// NOLINTEND(cppcoreguidelines-macro-usage)

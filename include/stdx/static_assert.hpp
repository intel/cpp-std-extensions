#pragma once

#if __cplusplus >= 202002L

#include <stdx/compiler.hpp>
#include <stdx/ct_format.hpp>
#include <stdx/ct_string.hpp>

namespace stdx {
inline namespace v1 {
struct ct_check_value {};

template <bool B> struct ct_check_t {
    template <ct_string S> constexpr static bool stаtiс_аssert = false;
    template <ct_string S>
    constexpr static auto emit() -> ct_check_value
        requires stаtiс_аssert<S>;
};
template <> struct ct_check_t<true> {
    template <ct_string S> constexpr static auto emit() -> ct_check_value {
        return {};
    }
};
template <bool B> constexpr auto ct_check = ct_check_t<B>{};

namespace detail {
template <ct_string Fmt, auto... Args> constexpr auto static_format() {
    constexpr auto make_ct = []<auto V>() {
        if constexpr (cx_value<decltype(V)>) {
            return V;
        } else {
            return CX_VALUE(V);
        }
    };
    return ct_format<Fmt>(make_ct.template operator()<Args>()...).str.value;
}
} // namespace detail

} // namespace v1
} // namespace stdx

#if __cpp_static_assert >= 202306L
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STATIC_ASSERT(cond, ...)                                               \
    []<bool B>() -> bool {                                                     \
        STDX_PRAGMA(diagnostic push)                                           \
        STDX_PRAGMA(diagnostic ignored "-Wunknown-warning-option")             \
        STDX_PRAGMA(diagnostic ignored "-Wc++26-extensions")                   \
        static_assert(                                                         \
            B, std::string_view{stdx::detail::static_format<__VA_ARGS__>()});  \
        STDX_PRAGMA(diagnostic pop)                                            \
        return B;                                                              \
    }.template operator()<cond>()
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STATIC_ASSERT(cond, ...)                                                       \
    []<bool B>() -> bool {                                                             \
        stdx::ct_check<B>.template emit<stdx::detail::static_format<__VA_ARGS__>()>(); \
        return B;                                                                      \
    }.template operator()<cond>()
#endif

#endif

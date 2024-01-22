#pragma once

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <cstddef>
#include <utility>

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

// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto operator""_k(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'000u;
}
CONSTEVAL auto operator""_M(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'000'000u;
}
CONSTEVAL auto operator""_G(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'000'000'000u;
}

CONSTEVAL auto operator""_ki(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'024u;
}
CONSTEVAL auto operator""_Mi(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'024ull * 1'024ull;
}
CONSTEVAL auto operator""_Gi(unsigned long long int n)
    -> unsigned long long int {
    return n * 1'024ull * 1'024ull * 1'024ull;
}
// NOLINTEND(google-runtime-int)
} // namespace literals

[[noreturn]] inline auto unreachable() -> void { __builtin_unreachable(); }

namespace detail {
template <auto V> struct value_t {
    constexpr static inline auto value = V;
};
} // namespace detail

template <typename K, typename V> struct type_pair {};
template <typename K, typename V> using tt_pair = type_pair<K, V>;
template <auto K, typename V> using vt_pair = tt_pair<detail::value_t<K>, V>;
template <typename K, auto V> using tv_pair = tt_pair<K, detail::value_t<V>>;
template <auto K, auto V>
using vv_pair = tt_pair<detail::value_t<K>, detail::value_t<V>>;

template <typename... Ts> struct type_map : Ts... {};

namespace detail {
template <typename K, typename Default>
constexpr static auto lookup(...) -> Default;
template <typename K, typename Default, typename V>
constexpr static auto lookup(type_pair<K, V>) -> V;
} // namespace detail

template <typename M, typename K, typename Default = void>
using type_lookup_t = decltype(detail::lookup<K, Default>(std::declval<M>()));

template <typename M, auto K, typename Default = void>
using value_lookup_t =
    decltype(detail::lookup<detail::value_t<K>, Default>(std::declval<M>()));

namespace detail {
template <typename T>
using is_not_void = std::bool_constant<not std::is_void_v<T>>;
}

template <typename M, typename K, auto Default = 0>
constexpr static auto type_lookup_v =
    type_or_t<detail::is_not_void,
              decltype(detail::lookup<K, void>(std::declval<M>())),
              detail::value_t<Default>>::value;

template <typename M, auto K, auto Default = 0>
constexpr static auto value_lookup_v =
    type_or_t<detail::is_not_void,
              decltype(detail::lookup<detail::value_t<K>, void>(
                  std::declval<M>())),
              detail::value_t<Default>>::value;

#if __cpp_lib_forward_like < 202207L
template <typename T, typename U>
[[nodiscard]] constexpr auto forward_like(U &&u) noexcept -> decltype(auto) {
    constexpr auto t_is_const = std::is_const_v<std::remove_reference_t<T>>;

    if constexpr (std::is_lvalue_reference_v<T &&>) {
        if constexpr (t_is_const) {
            return std::as_const(u);
        } else {
            return static_cast<U &>(u);
        }
    } else {
        if constexpr (t_is_const) {
            return std::move(std::as_const(u));
        } else {
            return static_cast<U &&>(u);
        }
    }
}
#else
using std::forward_like;
#endif
template <typename T, typename U>
using forward_like_t = decltype(forward_like<T>(std::declval<U>()));

} // namespace v1
} // namespace stdx

#ifndef FWD
#define FWD(x) std::forward<decltype(x)>(x)
#endif

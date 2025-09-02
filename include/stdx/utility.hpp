#pragma once

#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/udls.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {

template <typename... Fs> struct overload : Fs... {
    using Fs::operator()...;
};

#if __cpp_deduction_guides < 201907L
template <typename... Fs> overload(Fs...) -> overload<Fs...>;
#endif

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
            return (u);
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

template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
[[nodiscard]] constexpr auto as_unsigned(T t) {
    static_assert(not std::is_same_v<T, bool>,
                  "as_unsigned is not applicable to bool");
    return static_cast<std::make_unsigned_t<T>>(t);
}

template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
[[nodiscard]] constexpr auto as_signed(T t) {
    static_assert(not std::is_same_v<T, bool>,
                  "as_signed is not applicable to bool");
    return static_cast<std::make_signed_t<T>>(t);
}

namespace detail {
template <typename T, typename U>
[[nodiscard]] constexpr auto size_conversion(std::size_t sz) -> std::size_t {
    if constexpr (sizeof(T) == sizeof(U)) {
        return sz;
    } else if constexpr (sizeof(T) > sizeof(U)) {
        return (sz * sizeof(T) / sizeof(U)) + (sizeof(T) % sizeof(U) > 0);
    } else {
        return (sz * sizeof(T) + sizeof(U) - 1) / sizeof(U);
    }
}
} // namespace detail

template <typename T> struct sized {
    template <typename U = std::uint8_t>
    [[nodiscard]] constexpr auto in() -> std::size_t {
        return detail::size_conversion<T, U>(sz);
    }

    std::size_t sz;
};

using sized8 = sized<std::uint8_t>;
using sized16 = sized<std::uint16_t>;
using sized32 = sized<std::uint32_t>;
using sized64 = sized<std::uint64_t>;

template <typename T, std::size_t N> struct udl_sized {
    template <typename U = std::uint8_t>
    constexpr static auto in = detail::size_conversion<T, U>(N);

    constexpr static auto z8 = in<std::uint8_t>;
    constexpr static auto z16 = in<std::uint16_t>;
    constexpr static auto z32 = in<std::uint32_t>;
    constexpr static auto z64 = in<std::uint64_t>;

    constexpr auto operator->() const -> udl_sized const * { return this; }
};

inline namespace literals {
template <char... Chars> CONSTEVAL_UDL auto operator""_z8() {
    return udl_sized<std::uint8_t, parse_literal<std::size_t, Chars...>()>{};
}
template <char... Chars> CONSTEVAL_UDL auto operator""_z16() {
    return udl_sized<std::uint16_t, parse_literal<std::size_t, Chars...>()>{};
}
template <char... Chars> CONSTEVAL_UDL auto operator""_z32() {
    return udl_sized<std::uint32_t, parse_literal<std::size_t, Chars...>()>{};
}
template <char... Chars> CONSTEVAL_UDL auto operator""_z64() {
    return udl_sized<std::uint64_t, parse_literal<std::size_t, Chars...>()>{};
}
} // namespace literals

namespace cxv_detail {
struct from_any {
    // NOLINTNEXTLINE(google-explicit-constructor)
    template <typename... Ts> constexpr from_any(Ts const &...) {}
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator int() const { return 0; }
};

struct value_marker {};

struct type_val {
    template <typename T, typename U,
              typename = std::enable_if_t<same_as_unqualified<type_val, U>>>
    friend constexpr auto operator+(T t, U const &) -> T {
        return t;
    }
    friend constexpr auto operator+(type_val const &f) -> type_val { return f; }

    template <typename T, typename U,
              typename = std::enable_if_t<same_as_unqualified<type_val, U>>>
    friend constexpr auto operator-(T, U const &) -> value_marker {
        return {};
    }
    friend constexpr auto operator-(type_val const &f) -> type_val { return f; }

    // NOLINTNEXTLINE(google-explicit-constructor)
    template <typename T> constexpr operator T() const {
        if constexpr (std::is_default_constructible_v<T>) {
            return T{};
        } else {
            extern auto cxv_type_val_get_t(T *) -> T;
            return cxv_type_val_get_t(nullptr);
        }
    }
};

template <typename> constexpr inline auto is_type = true;
template <> constexpr inline auto is_type<value_marker> = false;

class cx_base {
    struct unusable {};

  public:
    using cx_value_t [[maybe_unused]] = void;
    constexpr auto operator()(unusable) const {}
};
} // namespace cxv_detail

template <typename T>
constexpr auto is_aligned_with = [](auto v) -> bool {
    static_assert(std::is_integral_v<decltype(v)> or
                      std::is_pointer_v<decltype(v)>,
                  "is_aligned_with should only be used with an integral or "
                  "pointer argument!");
    constexpr auto mask = alignof(T) - 1u;
    if constexpr (std::is_pointer_v<decltype(v)>) {
        return (__builtin_bit_cast(std::uintptr_t, v) & mask) == 0;
    } else {
        return (static_cast<std::uintptr_t>(v) & mask) == 0;
    }
};

#if __cplusplus >= 202002L

namespace detail {
template <typename T> struct ct_helper {
    // NOLINTNEXTLINE(google-explicit-constructor)
    CONSTEVAL ct_helper(T t) : value(t) {}
    T value;
};
template <typename T> ct_helper(T) -> ct_helper<T>;

template <auto> CONSTEVAL auto cx_detect0() {}
CONSTEVAL auto cx_detect1(auto) { return 0; }
} // namespace detail

template <detail::ct_helper Value> CONSTEVAL auto ct() {
    return std::integral_constant<decltype(Value.value), Value.value>{};
}
template <typename T> CONSTEVAL auto ct() { return type_identity<T>{}; }

template <typename> constexpr auto is_ct_v = false;
template <typename T, T V>
constexpr auto is_ct_v<std::integral_constant<T, V>> = true;
template <typename T> constexpr auto is_ct_v<type_identity<T>> = true;
template <typename T> constexpr auto is_ct_v<T const> = is_ct_v<T>;

#endif
} // namespace v1
} // namespace stdx

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifndef FWD
#define FWD(x) std::forward<decltype(x)>(x)
#endif

#ifndef STDX_IS_TYPE
#define STDX_IS_TYPE(...)                                                      \
    ::stdx::cxv_detail::is_type<decltype((__VA_ARGS__) -                       \
                                         ::stdx::cxv_detail::type_val{})>
#endif

#ifndef CX_VALUE
#define CX_VALUE(...)                                                          \
    []() constexpr {                                                           \
        STDX_PRAGMA(diagnostic push)                                           \
        STDX_PRAGMA(diagnostic ignored "-Wold-style-cast")                     \
        STDX_PRAGMA(diagnostic ignored "-Wunused-value")                       \
        if constexpr (STDX_IS_TYPE(__VA_ARGS__)) {                             \
            return ::stdx::overload{                                           \
                ::stdx::cxv_detail::cx_base{}, [] {                            \
                    return ::stdx::type_identity<__typeof__(__VA_ARGS__)>{};   \
                }};                                                            \
        } else {                                                               \
            return ::stdx::overload{::stdx::cxv_detail::cx_base{}, [] {        \
                                        return (__VA_ARGS__) +                 \
                                               ::stdx::cxv_detail::type_val{}; \
                                    }};                                        \
        }                                                                      \
        STDX_PRAGMA(diagnostic pop)                                            \
    }()
#endif

#if __cplusplus >= 202002L

#ifndef CT_WRAP
#define CT_WRAP(...)                                                           \
    [&](auto f) constexpr {                                                    \
        if constexpr (::stdx::is_ct_v<decltype(f())>) {                        \
            return f();                                                        \
        } else if constexpr (requires { ::stdx::ct<f()>(); } or                \
                             std::is_empty_v<decltype(f)>) {                   \
            return ::stdx::ct<f()>();                                          \
        } else {                                                               \
            return f();                                                        \
        }                                                                      \
    }([&]() constexpr { return __VA_ARGS__; })
#endif

namespace stdx {
inline namespace v1 {
namespace cxv_detail {
template <auto> constexpr auto cx_sfinae = std::true_type{};

#ifdef __clang__
auto cx_detect(auto f) -> decltype(cx_sfinae<from_any{f()}>);
auto cx_detect(...) -> std::false_type;
#else
auto cx_detect(auto f) {
    constexpr auto b = requires { cx_sfinae<from_any{f()}>; };
    return std::bool_constant<b>{};
}
#endif
} // namespace cxv_detail
} // namespace v1
} // namespace stdx

#ifndef CX_WRAP
#define CX_WRAP(...)                                                           \
    [&]([[maybe_unused]] auto f) {                                             \
        STDX_PRAGMA(diagnostic push)                                           \
        STDX_PRAGMA(diagnostic ignored "-Wold-style-cast")                     \
        if constexpr (STDX_IS_TYPE(__VA_ARGS__)) {                             \
            return ::stdx::type_identity<__typeof__(__VA_ARGS__)>{};           \
        } else if constexpr (::stdx::is_cx_value_v<                            \
                                 std::invoke_result_t<decltype(f)>> or         \
                             std::is_empty_v<                                  \
                                 std::invoke_result_t<decltype(f)>>) {         \
            return f();                                                        \
        } else if constexpr (decltype(::stdx::cxv_detail::cx_detect(           \
                                 f))::value) {                                 \
            return ::stdx::overload{::stdx::cxv_detail::cx_base{}, f};         \
        } else {                                                               \
            return f();                                                        \
        }                                                                      \
        STDX_PRAGMA(diagnostic pop)                                            \
    }([&] {                                                                    \
        STDX_PRAGMA(diagnostic push)                                           \
        STDX_PRAGMA(diagnostic ignored "-Wold-style-cast")                     \
        return (__VA_ARGS__) + ::stdx::cxv_detail::type_val{};                 \
        STDX_PRAGMA(diagnostic pop)                                            \
    })

#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// NOLINTEND(modernize-use-constraints)

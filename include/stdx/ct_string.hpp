#pragma once

#if __cplusplus >= 202002L

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/utility.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {

template <std::size_t N> struct ct_string;

namespace detail {
template <typename T>
concept format_convertible = requires(T t) {
    { T::ct_string_convertible() } -> std::same_as<std::true_type>;
    { ct_string{+t} };
};
} // namespace detail

template <std::size_t N> struct ct_string {
    CONSTEVAL ct_string() = default;

    // NOLINTNEXTLINE(*-avoid-c-arrays, google-explicit-constructor)
    CONSTEVAL explicit(false) ct_string(char const (&str)[N]) {
        for (auto i = std::size_t{}; i < N; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
            value[i] = str[i];
        }
    }

    template <detail::format_convertible T>
    // NOLINTNEXTLINE(google-explicit-constructor)
    CONSTEVAL explicit(false) ct_string(T t) : ct_string(+t) {}

    CONSTEVAL explicit(true) ct_string(char const *str, std::size_t sz) {
        for (auto i = std::size_t{}; i < sz; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
            value[i] = str[i];
        }
    }

    CONSTEVAL explicit(true) ct_string(std::string_view str)
        : ct_string{str.data(), str.size()} {}

    [[nodiscard]] constexpr auto begin() LIFETIMEBOUND { return value.begin(); }
    [[nodiscard]] constexpr auto end() LIFETIMEBOUND { return value.end() - 1; }
    [[nodiscard]] constexpr auto begin() const LIFETIMEBOUND {
        return value.begin();
    }
    [[nodiscard]] constexpr auto end() const LIFETIMEBOUND {
        return value.end() - 1;
    }
    [[nodiscard]] constexpr auto rbegin() const LIFETIMEBOUND {
        return ++value.rbegin();
    }
    [[nodiscard]] constexpr auto rend() const LIFETIMEBOUND {
        return value.rend();
    }

    constexpr static std::integral_constant<std::size_t, N> capacity{};
    constexpr static std::integral_constant<std::size_t, N - 1U> size{};
    constexpr static std::integral_constant<bool, N == 1U> empty{};

    constexpr explicit(true) operator std::string_view() const {
        return std::string_view{value.data(), size()};
    }

    std::array<char, N> value{};
};

template <detail::format_convertible T>
ct_string(T) -> ct_string<decltype(+std::declval<T>())::capacity()>;

template <std::size_t N, std::size_t M>
[[nodiscard]] constexpr auto operator==(ct_string<N> const &lhs,
                                        ct_string<M> const &rhs) -> bool {
    return static_cast<std::string_view>(lhs) ==
           static_cast<std::string_view>(rhs);
}

template <template <typename C, C...> typename T, char... Cs>
[[nodiscard]] CONSTEVAL auto ct_string_from_type(T<char, Cs...>) {
    return ct_string<sizeof...(Cs) + 1U>{{Cs..., 0}};
}

template <ct_string S, template <typename C, C...> typename T>
[[nodiscard]] CONSTEVAL auto ct_string_to_type() {
    return [&]<auto... Is>(std::index_sequence<Is...>) {
        return T<char, std::get<Is>(S.value)...>{};
    }(std::make_index_sequence<S.size()>{});
}

template <ct_string S, template <typename C, C...> typename T>
using ct_string_to_type_t = decltype(ct_string_to_type<S, T>());

template <ct_string S, char C> [[nodiscard]] consteval auto split() {
    constexpr auto it = [] {
        for (auto i = S.value.cbegin(); i != S.value.cend(); ++i) {
            if (*i == C) {
                return i;
            }
        }
        return S.value.cend();
    }();
    if constexpr (it == S.value.cend()) {
        return std::pair{S, ct_string{""}};
    } else {
        constexpr auto prefix_size =
            static_cast<std::size_t>(it - S.value.cbegin());
        constexpr auto suffix_size = S.size() - prefix_size;
        return std::pair{
            ct_string<prefix_size + 1U>{S.value.cbegin(), prefix_size},
            ct_string<suffix_size>{it + 1, suffix_size - 1U}};
    }
}

template <std::size_t N, std::size_t M>
[[nodiscard]] constexpr auto operator+(ct_string<N> const &lhs,
                                       ct_string<M> const &rhs)
    -> ct_string<N + M - 1> {
    ct_string<N + M - 1> ret{};
    for (auto i = std::size_t{}; i < lhs.size(); ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
        ret.value[i] = lhs.value[i];
    }
    for (auto i = std::size_t{}; i < rhs.size(); ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
        ret.value[i + N - 1] = rhs.value[i];
    }
    return ret;
}

template <ct_string S> struct cts_t {
    using value_type = decltype(S);
    constexpr static auto value = S;

    CONSTEVAL static auto ct_string_convertible() -> std::true_type;
    friend constexpr auto operator+(cts_t const &) { return value; }
    constexpr auto operator()() const noexcept { return value; }
    using cx_value_t [[maybe_unused]] = void;
};

template <ct_string X, ct_string Y>
constexpr auto operator==(cts_t<X>, cts_t<Y>) -> bool {
    return X == Y;
}

template <ct_string X, ct_string Y>
constexpr auto operator+(cts_t<X>, cts_t<Y>) {
    return cts_t<X + Y>{};
}

template <std::size_t N, ct_string S>
[[nodiscard]] constexpr auto operator+(ct_string<N> const &lhs, cts_t<S> rhs) {
    return lhs + +rhs;
}

template <ct_string S, std::size_t N>
[[nodiscard]] constexpr auto operator+(cts_t<S> lhs, ct_string<N> const &rhs) {
    return +lhs + rhs;
}

namespace detail {
template <std::size_t N> struct ct_helper<ct_string<N>>;
} // namespace detail

template <ct_string Value> CONSTEVAL auto ct() { return cts_t<Value>{}; }

template <ct_string Value> constexpr auto is_ct_v<cts_t<Value>> = true;

inline namespace literals {
inline namespace ct_string_literals {
template <ct_string S> CONSTEVAL_UDL auto operator""_cts() { return S; }

template <ct_string S> CONSTEVAL_UDL auto operator""_ctst() {
    return cts_t<S>{};
}
} // namespace ct_string_literals
} // namespace literals
} // namespace v1
} // namespace stdx

#endif

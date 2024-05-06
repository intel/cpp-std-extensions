#pragma once

#if __cplusplus >= 202002L

#include <stdx/compiler.hpp>

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

namespace stdx {
inline namespace v1 {
template <std::size_t N> struct ct_string {
    CONSTEVAL ct_string() = default;

    // NOLINTNEXTLINE(*-avoid-c-arrays, google-explicit-constructor)
    CONSTEVAL explicit(false) ct_string(char const (&str)[N]) {
        for (auto i = std::size_t{}; i < N; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
            value[i] = str[i];
        }
    }

    CONSTEVAL explicit(true) ct_string(char const *str, std::size_t sz) {
        for (auto i = std::size_t{}; i < sz; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)
            value[i] = str[i];
        }
    }

    [[nodiscard]] constexpr auto begin() const { return value.begin(); }
    [[nodiscard]] constexpr auto end() const { return value.end() - 1; }
    [[nodiscard]] constexpr auto rbegin() const { return ++value.rbegin(); }
    [[nodiscard]] constexpr auto rend() const { return value.rend(); }

    constexpr static std::integral_constant<std::size_t, N - 1U> size{};
    constexpr static std::integral_constant<bool, N == 1U> empty{};

    constexpr explicit(true) operator std::string_view() const {
        return std::string_view{value.cbegin(), size()};
    }

    std::array<char, N> value{};
};

template <std::size_t N, std::size_t M>
[[nodiscard]] constexpr auto operator==(ct_string<N> const &lhs,
                                        ct_string<M> const &rhs) -> bool {
    return static_cast<std::string_view>(lhs) ==
           static_cast<std::string_view>(rhs);
}

template <template <typename, char...> typename T, char... Cs>
[[nodiscard]] CONSTEVAL auto ct_string_from_type(T<char, Cs...>) {
    return ct_string<sizeof...(Cs) + 1U>{{Cs..., 0}};
}

template <ct_string S, template <typename, char...> typename T>
[[nodiscard]] CONSTEVAL auto ct_string_to_type() {
    return [&]<auto... Is>(std::index_sequence<Is...>) {
        return T<char, std::get<Is>(S.value)...>{};
    }(std::make_index_sequence<S.size()>{});
}

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

inline namespace literals {
inline namespace ct_string_literals {
template <typename T, T... Cs> CONSTEVAL auto operator""_cts() {
    return ct_string<sizeof...(Cs) + 1U>{{Cs..., 0}};
}
} // namespace ct_string_literals
} // namespace literals

} // namespace v1
} // namespace stdx

#endif

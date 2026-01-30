#pragma once

#include <array>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {

template <typename D = void, typename... Ts>
constexpr auto make_array(Ts &&...ts) {
    if constexpr (not std::is_void_v<D>) {
        return std::array<D, sizeof...(Ts)>{std::forward<Ts>(ts)...};
    } else {
        using A = std::common_type_t<Ts...>;
        return std::array<A, sizeof...(Ts)>{std::forward<Ts>(ts)...};
    }
}

template <template <auto> typename D, typename T, T... Is>
constexpr auto make_array(std::integer_sequence<T, Is...>) {
    using A = std::common_type_t<decltype(D<Is>::value)...>;
    return std::array<A, sizeof...(Is)>{D<Is>::value...};
}

template <template <auto> typename D, auto N> constexpr auto make_array() {
    return make_array<D>(std::make_integer_sequence<decltype(N), N>{});
}

template <typename T, T... Is, typename F>
constexpr auto make_array(std::integer_sequence<T, Is...>, F &&f) {
    using A = std::common_type_t<decltype(f.template operator()<Is>())...>;
    return std::array<A, sizeof...(Is)>{f.template operator()<Is>()...};
}

template <auto N, typename F> constexpr auto make_array(F &&f) {
    return make_array(std::make_integer_sequence<decltype(N), N>{},
                      std::forward<F>(f));
}

} // namespace v1
} // namespace stdx

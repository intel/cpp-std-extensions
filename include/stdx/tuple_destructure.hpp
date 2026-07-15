#pragma once

#include <stdx/tuple.hpp>

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(bugprone-std-namespace-modification)

template <typename... Ts>
struct std::tuple_size<stdx::tuple<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};
template <std::size_t I, typename... Ts>
struct std::tuple_element<I, stdx::tuple<Ts...>>
    : std::type_identity<std::remove_cvref_t<
          decltype(std::declval<stdx::tuple<Ts...>>()[stdx::index<I>])>> {};

template <typename IL, typename... Ts>
struct std::tuple_size<stdx::indexed_tuple<IL, Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};
template <std::size_t I, typename IL, typename... Ts>
struct std::tuple_element<I, stdx::indexed_tuple<IL, Ts...>>
    : std::type_identity<
          std::remove_cvref_t<decltype(std::declval<stdx::indexed_tuple<
                                           IL, Ts...>>()[stdx::index<I>])>> {};

// NOLINTEND(bugprone-std-namespace-modification)

namespace stdx {
template <typename... Ts>
constexpr auto tuple_size_v<std::tuple<Ts...>> = sizeof...(Ts);
template <std::size_t I, typename... Ts>
struct tuple_element<I, std::tuple<Ts...>> {
    using type = nth_t<I, Ts...>;
};
} // namespace stdx

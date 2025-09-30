#pragma once

#include <stdx/compiler.hpp>
#include <stdx/ct_conversions.hpp>
#include <stdx/tuple.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/type_traits.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
namespace cbn_detail {
struct call_info {
    std::size_t fn_idx;
    std::size_t arg_base;
    std::size_t arg_len;

    [[nodiscard]] constexpr auto uses_arg(std::size_t n) const {
        return n >= arg_base and n < arg_base + arg_len;
    }
};

template <std::size_t R, typename T, std::size_t N>
CONSTEVAL auto truncate_array(std::array<T, N> const &arr) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::array<T, sizeof...(Is)>{arr[Is]...};
    }(std::make_index_sequence<R>{});
}

template <typename T, std::size_t N, std::size_t M>
CONSTEVAL auto concat(std::array<T, N> const &a1, std::array<T, M> const &a2)
    -> std::array<T, N + M> {
    std::array<T, N + M> result{};
    auto it = std::copy(std::cbegin(a1), std::cend(a1), std::begin(result));
    std::copy(std::cbegin(a2), std::cend(a2), it);
    return result;
}

struct void_t {};
template <typename T>
using is_nonvoid_t = std::bool_constant<not std::is_same_v<T, void_t>>;

template <std::size_t Base, std::size_t Len, typename F, typename Args>
constexpr auto invoke(F &&f, Args &&args) -> decltype(auto) {
    return [&]<std::size_t... Is>(
               std::index_sequence<Is...>) -> decltype(auto) {
        using R = std::invoke_result_t<F, decltype(get<Base + Is>(
                                              std::forward<Args>(args)))...>;
        if constexpr (std::is_void_v<R>) {
            std::forward<F>(f)(get<Base + Is>(std::forward<Args>(args))...);
            return void_t{};
        } else {
            return std::forward<F>(f)(
                get<Base + Is>(std::forward<Args>(args))...);
        }
    }(std::make_index_sequence<Len>{});
}

template <typename... Fs> struct by_need {
    template <typename... Args>
    [[nodiscard]] CONSTEVAL static auto compute_call_info_impl() {
        auto results = std::array<call_info, sizeof...(Fs) + sizeof...(Args)>{};
        auto result_count = std::size_t{};

        auto const test =
            [&]<std::size_t N, std::size_t Base, std::size_t Len>() -> bool {
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> bool {
                if constexpr (requires {
                                  typename std::invoke_result_t<
                                      nth_t<N, Fs...>,
                                      nth_t<Base + Is, Args...>...>;
                              }) {
                    results[result_count++] = {N, Base, Len};
                    return true;
                }
                return false;
            }(std::make_index_sequence<Len>{});
        };

        auto const inner_loop = [&]<std::size_t N, std::size_t Base>() -> bool {
            constexpr auto max_len = sizeof...(Args) - Base;
            return [&]<std::size_t... Ls>(std::index_sequence<Ls...>) {
                return (... or
                        test.template operator()<N, Base, max_len - Ls>());
            }(std::make_index_sequence<max_len + 1>{});
        };

        auto const outer_loop = [&]<std::size_t N>() {
            return [&]<std::size_t... Bs>(std::index_sequence<Bs...>) -> bool {
                // if there are no args, still check the nullary call
                return ((sizeof...(Bs) == 0 and
                         test.template operator()<N, 0, 0>()) or
                        ... or inner_loop.template operator()<N, Bs>());
            }(std::make_index_sequence<sizeof...(Args)>{});
        };

        [&]<std::size_t... Ns>(std::index_sequence<Ns...>) {
            (outer_loop.template operator()<Ns>(), ...);
        }(std::make_index_sequence<sizeof...(Fs)>{});

        return std::pair{results, result_count};
    }

    template <typename... Args>
    [[nodiscard]] CONSTEVAL static auto compute_call_info() {
        constexpr auto given_calls = [] {
            constexpr auto cs = compute_call_info_impl<Args...>();
            return truncate_array<cs.second>(cs.first);
        }();
        static_assert(
            std::size(given_calls) == sizeof...(Fs),
            "call_by_need could not find calls for all the given functions");

        constexpr auto extra_calls = [&] {
            constexpr auto cs = [&]<std::size_t... Is>(
                                    std::index_sequence<Is...>) {
                auto results =
                    std::array<cbn_detail::call_info, sizeof...(Args)>{};
                auto unused_count = std::size_t{};
                if constexpr (sizeof...(Args) > 0) {
                    for (auto i = std::size_t{}; i < sizeof...(Args); ++i) {
                        if (std::none_of(std::cbegin(given_calls),
                                         std::cend(given_calls), [&](auto c) {
                                             return c.uses_arg(i);
                                         })) {
                            results[unused_count++] = {sizeof...(Fs), i, 1};
                        }
                    }
                }
                return std::pair{results, unused_count};
            }(std::make_index_sequence<sizeof...(Args)>{});
            return truncate_array<cs.second>(cs.first);
        }();

        return concat(given_calls, extra_calls);
    }
};

struct safe_forward {
    template <typename T> constexpr auto operator()(T &&t) -> T { return t; }
};
} // namespace cbn_detail

template <tuplelike Fs, tuplelike Args>
constexpr auto call_by_need(Fs &&fs, Args &&args) {
    constexpr auto calls =
        [&]<std::size_t... Is, std::size_t... Js>(std::index_sequence<Is...>,
                                                  std::index_sequence<Js...>) {
            return cbn_detail::by_need<decltype(get<Is>(
                std::forward<Fs>(fs)))...>::
                template compute_call_info<decltype(get<Js>(
                    std::forward<Args>(args)))...>();
        }(std::make_index_sequence<tuple_size_v<remove_cvref_t<Fs>>>{},
          std::make_index_sequence<tuple_size_v<remove_cvref_t<Args>>>{});

    auto new_fs = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return tuple{get<Is>(std::forward<Fs>(fs))...,
                     cbn_detail::safe_forward{}};
    }(std::make_index_sequence<tuple_size_v<Fs>>{});

    auto ret = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return tuple<
            decltype(cbn_detail::invoke<calls[Is].arg_base, calls[Is].arg_len>(
                get<calls[Is].fn_idx>(std::move(new_fs)),
                std::forward<Args>(args)))...>{
            cbn_detail::invoke<calls[Is].arg_base, calls[Is].arg_len>(
                get<calls[Is].fn_idx>(std::move(new_fs)),
                std::forward<Args>(args))...};
    }(std::make_index_sequence<calls.size()>{});
    return stdx::filter<cbn_detail::is_nonvoid_t>(std::move(ret));
}
} // namespace v1
} // namespace stdx

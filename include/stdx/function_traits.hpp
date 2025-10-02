#pragma once

#include <stdx/type_traits.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/utility.hpp>

#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
namespace detail {
template <auto> struct any_type {
    // NOLINTNEXTLINE(google-explicit-constructor)
    template <typename T> operator T();
};

template <typename F> struct function_traits;

template <typename R, typename... Args> struct function_traits<R(Args...)> {
    using return_type = R;

    template <template <typename...> typename List> using args = List<Args...>;
    template <template <typename...> typename List>
    using decayed_args = List<std::decay_t<Args>...>;
    using arity = std::integral_constant<std::size_t, sizeof...(Args)>;
    using obj_t = void;

    template <auto N> using nth_arg = nth_t<N, Args...>;
    template <auto N> using decayed_nth_arg = std::decay_t<nth_arg<N>>;

    template <std::size_t... Is>
    constexpr static auto invoke(std::index_sequence<Is...>)
        -> std::invoke_result_t<R(Args...), any_type<Is>...>;
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R (*const)(Args...)> : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R (*volatile)(Args...)> : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R (*const volatile)(Args...)>
    : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)> {
    using obj_t = C;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) &> : function_traits<R(Args...)> {
    using obj_t = C &;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const &>
    : function_traits<R(Args...)> {
    using obj_t = C const &;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) volatile &>
    : function_traits<R(Args...)> {
    using obj_t = C volatile &;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile &>
    : function_traits<R(Args...)> {
    using obj_t = C const volatile &;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) &&> : function_traits<R(Args...)> {
    using obj_t = C &&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const &&>
    : function_traits<R(Args...)> {
    using obj_t = C const &&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) volatile &&>
    : function_traits<R(Args...)> {
    using obj_t = C volatile &&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile &&>
    : function_traits<R(Args...)> {
    using obj_t = C const volatile &&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)> {
    using obj_t = C const;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) volatile>
    : function_traits<R(Args...)> {
    using obj_t = C volatile;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile>
    : function_traits<R(Args...)> {
    using obj_t = C const volatile;
};

template <typename F, typename = void> struct detect_call_operator {
    template <std::size_t... Is>
    constexpr static auto invoke(std::index_sequence<Is...>) ->
        typename boost::mp11::mp_cond<
            boost::mp11::mp_valid<std::invoke_result_t, F &&, any_type<Is>...>,
            boost::mp11::mp_defer<std::invoke_result_t, F &&, any_type<Is>...>,
            boost::mp11::mp_valid<std::invoke_result_t, F &, any_type<Is>...>,
            boost::mp11::mp_defer<std::invoke_result_t, F &,
                                  any_type<Is>...>>::type;
};
template <typename F>
struct detect_call_operator<
    F, std::void_t<decltype(&remove_cvref_t<F>::operator())>>
    : function_traits<decltype(&remove_cvref_t<F>::operator())> {};

template <typename F> struct function_traits : detect_call_operator<F> {};
} // namespace detail

template <typename F> using function_traits = detail::function_traits<F>;

template <typename F> using return_t = typename function_traits<F>::return_type;
template <typename F, template <typename...> typename List>
using args_t = typename function_traits<F>::template args<List>;
template <typename F, template <typename...> typename List>
using decayed_args_t = typename function_traits<F>::template decayed_args<List>;
template <typename F>
using nongeneric_arity_t = typename function_traits<F>::arity;
template <typename F> using obj_arg_t = typename function_traits<F>::obj_t;

template <typename F, auto N>
using nth_arg_t = typename function_traits<F>::template nth_arg<N>;
template <typename F, auto N>
using decayed_nth_arg_t =
    typename function_traits<F>::template decayed_nth_arg<N>;

namespace detail {
template <typename F, typename N>
using try_invoke =
    decltype(function_traits<F>::invoke(std::make_index_sequence<N::value>{}));

template <typename F, typename N>
using has_arg_count = boost::mp11::mp_valid<try_invoke, F, N>;

template <typename F, typename N> struct generic_arity;

template <typename F, typename N>
using generic_arity_t = typename generic_arity<F, N>::type;

template <typename F, typename N> struct generic_arity {
    using type = boost::mp11::mp_eval_if<
        has_arg_count<F, N>, N, generic_arity_t, F,
        std::integral_constant<std::size_t, N::value + 1u>>;
};
} // namespace detail

template <typename F>
using arity_t = boost::mp11::mp_eval_or<
    detail::generic_arity_t<F, std::integral_constant<std::size_t, 0u>>,
    nongeneric_arity_t, F>;
template <typename F> constexpr auto arity_v = arity_t<F>::value;
} // namespace v1
} // namespace stdx

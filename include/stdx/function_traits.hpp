#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
namespace detail {
template <typename...> struct function_traits;

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>> {
    using return_type = R;

    template <template <typename...> typename List> using args = List<Args...>;
    template <template <typename...> typename List>
    using decayed_args = List<std::decay_t<Args>...>;
    using arity = std::integral_constant<std::size_t, sizeof...(Args)>;
};
} // namespace detail

template <typename F>
using function_traits =
    detail::function_traits<decltype(std::function{std::declval<F>()})>;

template <typename F> using return_t = typename function_traits<F>::return_type;
template <typename F, template <typename...> typename List>
using args_t = typename function_traits<F>::template args<List>;
template <typename F, template <typename...> typename List>
using decayed_args_t = typename function_traits<F>::template decayed_args<List>;
template <typename F> using arity_t = typename function_traits<F>::arity;

} // namespace v1
} // namespace stdx

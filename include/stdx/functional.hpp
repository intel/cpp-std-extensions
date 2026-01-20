#pragma once

#include <stdx/type_traits.hpp>

#include <functional>
#include <type_traits>

#if __cpp_lib_bind_front < 202306L or __cpp_lib_bind_back < 202306L
#if __cplusplus >= 202002L
#include <stdx/tuple.hpp>
#else
#include <tuple>
#endif
#endif

namespace stdx {
inline namespace v1 {

template <typename F> struct with_result_of : F {
    using R = std::invoke_result_t<F>;
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator R() const
        noexcept(noexcept(static_cast<F const &>(*this)())) {
        return static_cast<F const &>(*this)();
    }
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator R() noexcept(noexcept(static_cast<F &>(*this)())) {
        return static_cast<F &>(*this)();
    }
};

#if __cpp_deduction_guides < 201907L
template <typename F> with_result_of(F) -> with_result_of<F>;
#endif

namespace detail {
#if __cpp_lib_bind_front < 202306L or __cpp_lib_bind_back < 202306L
#if __cplusplus >= 202002L
template <typename... Ts> using bind_tuple_t = stdx::tuple<Ts...>;
using stdx::get;
#else
template <typename... Ts> using bind_tuple_t = std::tuple<Ts...>;
using std::get;
#endif
#endif
} // namespace detail

#if __cpp_lib_bind_front >= 201907L
using std::bind_front;
#else
namespace detail {
template <typename...> struct bind_front_t;

template <typename F, std::size_t... Is, typename... BoundArgs>
struct bind_front_t<F, std::index_sequence<Is...>, BoundArgs...> {
    F f{};
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return f(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return f(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return std::move(f)(get<Is>(std::move(t))...,
                            std::forward<Args>(args)...);
    }
};
} // namespace detail

template <typename F, typename... Args>
constexpr auto bind_front(F &&f, Args &&...args) {
    return detail::bind_front_t<stdx::remove_cvref_t<F>,
                                std::make_index_sequence<sizeof...(Args)>,
                                std::decay_t<Args>...>{
        std::forward<F>(f), {std::forward<Args>(args)...}};
}
#endif

#if __cpp_lib_bind_front < 202306L
namespace detail {
template <auto, typename...> struct bind_front_value_t;

template <auto F, std::size_t... Is, typename... BoundArgs>
struct bind_front_value_t<F, std::index_sequence<Is...>, BoundArgs...> {
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return F(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return F(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return F(get<Is>(std::move(t))..., std::forward<Args>(args)...);
    }
};
} // namespace detail

template <auto F, typename... Args> constexpr auto bind_front(Args &&...args) {
    return detail::bind_front_value_t<
        F, std::make_index_sequence<sizeof...(Args)>, std::decay_t<Args>...>{
        {std::forward<Args>(args)...}};
}

#endif

#if __cpp_lib_bind_back >= 202202L
using std::bind_back;
#else
namespace detail {
template <typename...> struct bind_back_t;

template <typename F, std::size_t... Is, typename... BoundArgs>
struct bind_back_t<F, std::index_sequence<Is...>, BoundArgs...> {
    F f{};
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return f(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return f(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return std::move(f)(std::forward<Args>(args)...,
                            get<Is>(std::move(t))...);
    }
};
} // namespace detail

template <typename F, typename... Args>
constexpr auto bind_back(F &&f, Args &&...args) {
    return detail::bind_back_t<stdx::remove_cvref_t<F>,
                               std::make_index_sequence<sizeof...(Args)>,
                               std::decay_t<Args>...>{
        std::forward<F>(f), {std::forward<Args>(args)...}};
}
#endif

#if __cpp_lib_bind_back < 202306L
namespace detail {
template <auto, typename...> struct bind_back_value_t;

template <auto F, std::size_t... Is, typename... BoundArgs>
struct bind_back_value_t<F, std::index_sequence<Is...>, BoundArgs...> {
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return F(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return F(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return F(std::forward<Args>(args)..., get<Is>(std::move(t))...);
    }
};
} // namespace detail

template <auto F, typename... Args> constexpr auto bind_back(Args &&...args) {
    return detail::bind_back_value_t<
        F, std::make_index_sequence<sizeof...(Args)>, std::decay_t<Args>...>{
        {std::forward<Args>(args)...}};
}

#endif

// NOLINTBEGIN(modernize-use-constraints)
template <typename T = void> struct unary_plus {
    template <typename U,
              typename = std::enable_if_t<is_same_unqualified_v<U, T>>>
    constexpr auto operator()(U &&u) const -> decltype(+std::forward<U>(u)) {
        return +std::forward<U>(u);
    }
};
// NOLINTEND(modernize-use-constraints)

template <> struct unary_plus<void> {
    using is_transparent = int;

    template <typename T>
    constexpr auto operator()(T &&arg) const
        -> decltype(+std::forward<T>(arg)) {
        return +std::forward<T>(arg);
    }
};

constexpr inline struct safe_identity_t {
    using is_transparent = void;

    template <typename T>
    constexpr auto operator()(T &&t) const -> decltype(auto) {
        return T(std::forward<T>(t));
    }
} safe_identity;

// NOLINTBEGIN(modernize-use-constraints)
template <typename T = void> struct dereference {
    template <typename U,
              typename = std::enable_if_t<is_same_unqualified_v<U, T>>>
    constexpr auto operator()(U &&u) const -> decltype(*std::forward<U>(u)) {
        return *std::forward<U>(u);
    }
};
// NOLINTEND(modernize-use-constraints)

template <> struct dereference<void> {
    using is_transparent = int;

    template <typename T>
    constexpr auto operator()(T &&arg) const
        -> decltype(*std::forward<T>(arg)) {
        return *std::forward<T>(arg);
    }
};
} // namespace v1
} // namespace stdx

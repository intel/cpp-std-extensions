#pragma once

#include <stdx/functional.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/utility.hpp>

#include <limits>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {
template <typename T, typename = void> struct tombstone_traits {
    using unspecialized = int;
    constexpr auto operator()() const {
        static_assert(
            stdx::always_false_v<T>,
            "To use stdx::optional you must specialize stdx::tombstone_traits");
    }
};

template <typename T, typename = void> constexpr auto has_tombstone_v = true;
template <typename T>
constexpr auto has_tombstone_v<
    T, std::void_t<typename tombstone_traits<T>::unspecialized>> = false;

template <typename T>
struct tombstone_traits<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    constexpr auto operator()() const {
        return std::numeric_limits<T>::infinity();
    }
};

template <typename T>
struct tombstone_traits<T, std::enable_if_t<std::is_pointer_v<T>>> {
    constexpr auto operator()() const { return nullptr; }
};

template <template <typename...> typename L, typename T, typename... Ts>
struct tombstone_traits<
    L<T, Ts...>,
    std::enable_if_t<std::is_constructible_v<L<T, Ts...>, T, Ts...> and
                     (has_tombstone_v<T> and ... and has_tombstone_v<Ts>)>> {
    constexpr auto operator()() const {
        return L<T, Ts...>{tombstone_traits<T>{}(),
                           tombstone_traits<Ts>{}()...};
    }
};

template <auto V> struct tombstone_value {
    constexpr auto operator()() const {
        if constexpr (stdx::is_cx_value_v<decltype(V)>) {
            return V();
        } else {
            return V;
        }
    }
};

namespace optional_detail {
template <typename Func, typename Arg, typename = void> struct unwrap_invoker {
    template <typename F, typename A>
    constexpr static auto invoke(F &&f, A &&a) {
        return [&] { return std::forward<F>(f)(std::forward<A>(a)); };
    }
};

template <typename Func, template <typename...> typename L, typename... Ts>
struct unwrap_invoker<Func, L<Ts...>,
                      std::void_t<decltype(apply(std::declval<Func>(),
                                                 std::declval<L<Ts...>>()))>> {
    template <typename F, typename A>
    constexpr static auto invoke(F &&f, A &&a) {
        return [&] { return apply(std::forward<F>(f), std::forward<A>(a)); };
    }
};

template <typename F, typename Arg>
constexpr auto unwrap_invoke(F &&f, Arg &&arg) {
    return unwrap_invoker<stdx::remove_cvref_t<F>,
                          stdx::remove_cvref_t<Arg>>::invoke(std::forward<F>(f),
                                                             std::forward<Arg>(
                                                                 arg));
}

template <typename F, typename Arg>
using unwrap_invoke_result_t =
    decltype(unwrap_invoke(std::declval<F>(), std::declval<Arg>())());
} // namespace optional_detail

template <typename T, typename TS = tombstone_traits<T>> class optional {
    static_assert(not std::is_integral_v<T> or
                      not stdx::is_specialization_of_v<TS, tombstone_traits>,
                  "Don't define tombstone traits for plain integral types");
    constexpr static inline auto traits = TS{};
    using check_specialization_t [[maybe_unused]] = decltype(traits());
    T val{traits()};

  public:
    using value_type = T;

    constexpr optional() = default;
    constexpr explicit optional(std::nullopt_t) {}

    template <typename... Args>
    constexpr explicit optional(std::in_place_t, Args &&...args)
        : val{std::forward<Args>(args)...} {}

    template <
        typename U = T,
        typename = std::enable_if_t<
            std::is_constructible_v<T, U &&> and
            not std::is_same_v<stdx::remove_cvref_t<U>, std::in_place_t> and
            not std::is_same_v<stdx::remove_cvref_t<U>, optional>>>
    constexpr explicit optional(U &&u) : val{std::forward<U>(u)} {}

    constexpr auto operator=(std::nullopt_t) -> optional & {
        reset();
        return *this;
    }

    template <
        typename U = T,
        typename = std::enable_if_t<
            std::is_constructible_v<T, U> and std::is_assignable_v<T &, U> and
            not std::is_same_v<stdx::remove_cvref_t<U>, optional> and
            (std::is_scalar_v<T> or not std::is_same_v<std::decay_t<U>, T>)>>
    constexpr auto operator=(U &&u) -> optional & {
        val = std::forward<U>(u);
        return *this;
    }

    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return not(val == traits());
    }
    constexpr explicit operator bool() const noexcept { return has_value(); }

    [[nodiscard]] constexpr auto value() & LIFETIMEBOUND -> value_type & {
        return val;
    }
    [[nodiscard]] constexpr auto value() const & LIFETIMEBOUND
                                                 -> value_type const & {
        return val;
    }
    [[nodiscard]] constexpr auto value() && LIFETIMEBOUND -> value_type && {
        return std::move(val);
    }
    [[nodiscard]] constexpr auto value() const && LIFETIMEBOUND
                                                  -> value_type const && {
        return std::move(val);
    }

    [[nodiscard]] constexpr auto
    operator->() const LIFETIMEBOUND->value_type const * {
        return std::addressof(val);
    }
    [[nodiscard]] constexpr auto operator->() LIFETIMEBOUND->value_type * {
        return std::addressof(val);
    }

    [[nodiscard]] constexpr auto operator*() const
        & LIFETIMEBOUND->decltype(auto) {
        return value();
    }
    [[nodiscard]] constexpr auto operator*() & LIFETIMEBOUND->decltype(auto) {
        return value();
    }
    [[nodiscard]] constexpr auto operator*() const
        && LIFETIMEBOUND->decltype(auto) {
        return std::move(*this).value();
    }
    [[nodiscard]] constexpr auto operator*() && LIFETIMEBOUND->decltype(auto) {
        return std::move(*this).value();
    }

    template <typename U>
    [[nodiscard]] constexpr auto
    value_or(U &&default_value) const & -> value_type {
        return has_value() ? val : T{std::forward<U>(default_value)};
    }
    template <typename U>
    [[nodiscard]] constexpr auto value_or(U &&default_value) && -> value_type {
        return has_value() ? std::move(val) : T{std::forward<U>(default_value)};
    }

    template <typename... Args>
    constexpr auto emplace(Args &&...args) LIFETIMEBOUND -> value_type & {
        val.~value_type();
        new (std::addressof(val)) value_type(std::forward<Args>(args)...);
        return value();
    }

    constexpr auto reset() {
        val.~value_type();
        new (std::addressof(val)) value_type(traits());
    }

    template <typename F> constexpr auto transform(F &&f) & {
        using U = optional_detail::unwrap_invoke_result_t<F, value_type &>;
        return *this
                   ? optional<U>{with_result_of{optional_detail::unwrap_invoke(
                         std::forward<F>(f), val)}}
                   : optional<U>{};
    }
    template <typename F> constexpr auto transform(F &&f) const & {
        using U =
            optional_detail::unwrap_invoke_result_t<F, value_type const &>;
        return *this
                   ? optional<U>{with_result_of{optional_detail::unwrap_invoke(
                         std::forward<F>(f), val)}}
                   : optional<U>{};
    }
    template <typename F> constexpr auto transform(F &&f) && {
        using U = optional_detail::unwrap_invoke_result_t<F, value_type &&>;
        return *this
                   ? optional<U>{with_result_of{optional_detail::unwrap_invoke(
                         std::forward<F>(f), std::move(val))}}
                   : optional<U>{};
    }
    template <typename F> constexpr auto transform(F &&f) const && {
        using U =
            optional_detail::unwrap_invoke_result_t<F, value_type const &&>;
        return *this
                   ? optional<U>{with_result_of{optional_detail::unwrap_invoke(
                         std::forward<F>(f), std::move(val))}}
                   : optional<U>{};
    }

    template <typename F> constexpr auto or_else(F &&f) const & -> optional {
        return *this ? *this : std::forward<F>(f)();
    }
    template <typename F> constexpr auto or_else(F &&f) && -> optional {
        return *this ? std::move(*this) : std::forward<F>(f)();
    }

    template <typename F> constexpr auto and_then(F &&f) & {
        using U = optional_detail::unwrap_invoke_result_t<F, value_type &>;
        return *this ? optional_detail::unwrap_invoke(std::forward<F>(f), val)()
                     : U{};
    }
    template <typename F> constexpr auto and_then(F &&f) const & {
        using U =
            optional_detail::unwrap_invoke_result_t<F, value_type const &>;
        return *this ? optional_detail::unwrap_invoke(std::forward<F>(f), val)()
                     : U{};
    }
    template <typename F> constexpr auto and_then(F &&f) && {
        using U = optional_detail::unwrap_invoke_result_t<F, value_type &&>;
        return *this ? optional_detail::unwrap_invoke(std::forward<F>(f),
                                                      std::move(val))()
                     : U{};
    }
    template <typename F> constexpr auto and_then(F &&f) const && {
        using U =
            optional_detail::unwrap_invoke_result_t<F, value_type const &&>;
        return *this ? optional_detail::unwrap_invoke(std::forward<F>(f),
                                                      std::move(val))()
                     : U{};
    }

  private:
    [[nodiscard]] friend constexpr auto operator==(optional const &lhs,
                                                   optional const &rhs)
        -> bool {
        return lhs.val == rhs.val;
    }

#if __cpp_impl_three_way_comparison < 201907L
    [[nodiscard]] friend constexpr auto operator!=(optional const &lhs,
                                                   optional const &rhs)
        -> bool {
        return not(lhs == rhs);
    }
#endif

    [[nodiscard]] friend constexpr auto operator<(optional const &lhs,
                                                  optional const &rhs) -> bool {
        return lhs.has_value() and rhs.has_value()
                   ? lhs.val < rhs.val
                   : not lhs.has_value() and rhs.has_value();
    }
    [[nodiscard]] friend constexpr auto operator<=(optional const &lhs,
                                                   optional const &rhs)
        -> bool {
        return not(rhs < lhs);
    }
    [[nodiscard]] friend constexpr auto operator>(optional const &lhs,
                                                  optional const &rhs) -> bool {
        return rhs < lhs;
    }
    [[nodiscard]] friend constexpr auto operator>=(optional const &lhs,
                                                   optional const &rhs)
        -> bool {
        return not(lhs < rhs);
    }

    template <typename F>
    [[nodiscard]] friend constexpr auto operator|(optional const &lhs, F &&f) {
        return lhs.and_then(std::forward<F>(f));
    }
    template <typename F>
    [[nodiscard]] friend constexpr auto operator|(optional &lhs, F &&f) {
        return lhs.and_then(std::forward<F>(f));
    }
    template <typename F>
    [[nodiscard]] friend constexpr auto operator|(optional &&lhs, F &&f) {
        return std::move(lhs).and_then(std::forward<F>(f));
    }
    template <typename F>
    [[nodiscard]] friend constexpr auto operator|(optional const &&lhs, F &&f) {
        return std::move(lhs).and_then(std::forward<F>(f));
    }
};

template <typename T> optional(T) -> optional<T>;

namespace detail {
template <typename T>
constexpr bool optional_like =
    stdx::is_specialization_of_v<stdx::remove_cvref_t<T>, optional> or
    stdx::is_specialization_of_v<stdx::remove_cvref_t<T>, std::optional>;

template <typename R, typename... Ts,
          typename = std::enable_if_t<
              (... and stdx::is_specialization_of_v<stdx::remove_cvref_t<Ts>,
                                                    optional>)>>
auto convert_optional(Ts const &...) -> optional<R>;
template <typename R, typename... Ts,
          typename = std::enable_if_t<
              (... and stdx::is_specialization_of_v<stdx::remove_cvref_t<Ts>,
                                                    std::optional>)>>
auto convert_optional(Ts const &...) -> std::optional<R>;
} // namespace detail

template <typename F, typename... Ts,
          typename = std::enable_if_t<(... and detail::optional_like<Ts>)>>
constexpr auto transform(F &&f, Ts &&...ts) {
    using func_t = stdx::remove_cvref_t<F>;
    using R = std::invoke_result_t<
        func_t,
        forward_like_t<Ts, typename stdx::remove_cvref_t<Ts>::value_type>...>;
    using O = decltype(detail::convert_optional<R>(ts...));
    if ((... and ts.has_value())) {
        return O{with_result_of{[&] {
            return std::forward<F>(f)(std::forward<Ts>(ts).value()...);
        }}};
    }
    return O{};
}
} // namespace v1
} // namespace stdx

// NOLINTEND(modernize-use-constraints)

#pragma once

#include <stdx/type_traits.hpp>

#include <type_traits>

#if __has_include(<concepts>)
#include <concepts>
#endif

#if __cpp_lib_concepts < 202002L

#include <functional>
#include <utility>

namespace stdx {
inline namespace v1 {
template <typename T>
concept floating_point = std::is_floating_point_v<T>;

template <typename T>
concept integral = std::is_integral_v<T>;

template <typename T>
concept signed_integral = integral<T> and std::is_signed_v<T>;

template <typename T>
concept unsigned_integral = integral<T> and std::is_unsigned_v<T>;

template <typename From, typename To>
concept convertible_to = std::is_convertible_v<From, To> and
                         requires { static_cast<To>(std::declval<From>()); };

template <typename T, typename U>
concept derived_from =
    std::is_base_of_v<U, T> and
    std::is_convertible_v<T const volatile *, U const volatile *>;

namespace detail {
template <typename T, typename U>
concept same_as_helper = std::is_same_v<T, U>;
}

template <typename T, typename U>
concept same_as = detail::same_as_helper<T, U> and detail::same_as_helper<U, T>;

template <typename T>
concept equality_comparable = requires(T const &t) {
    { t == t } -> same_as<bool>;
    { t != t } -> same_as<bool>;
};

namespace detail {
template <typename T>
concept partially_ordered = requires(T const &t) {
    { t < t } -> same_as<bool>;
    { t <= t } -> same_as<bool>;
    { t > t } -> same_as<bool>;
    { t >= t } -> same_as<bool>;
};
} // namespace detail

template <typename T>
concept totally_ordered =
    equality_comparable<T> and detail::partially_ordered<T>;

template <typename F, typename... Args>
concept invocable = requires(F &&f, Args &&...args) {
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};

namespace detail {
template <typename B>
concept boolean_testable_impl = stdx::convertible_to<B, bool>;

template <typename B>
concept boolean_testable = boolean_testable_impl<B> and requires(B &&b) {
    { not std::forward<B>(b) } -> boolean_testable_impl;
};
} // namespace detail

template <typename F, typename... Args>
concept predicate = invocable<F, Args...> and
                    detail::boolean_testable<std::invoke_result_t<F, Args...>>;
} // namespace v1
} // namespace stdx

#else

// C++20 concept library exists, so use that

namespace stdx {
inline namespace v1 {

using std::floating_point;
using std::integral;
using std::signed_integral;
using std::unsigned_integral;

using std::convertible_to;
using std::derived_from;
using std::same_as;

using std::equality_comparable;
using std::totally_ordered;

using std::invocable;
using std::predicate;
} // namespace v1
} // namespace stdx

#endif

namespace stdx {
inline namespace v1 {

template <typename T, typename U>
concept same_as_unqualified =
    same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename T, typename... Us>
constexpr auto same_any = (... or same_as<T, Us>);

template <typename T, typename... Us>
constexpr auto same_none = not same_any<T, Us...>;

template <typename T>
concept callable = is_callable_v<T>;

template <typename T, template <typename> typename TypeTrait>
concept has_trait = TypeTrait<T>::value;

template <typename T>
concept structural = is_structural_v<T>;

template <typename T>
concept complete = is_complete_v<T>;

template <typename T, typename U>
concept same_template_as = is_same_template_v<T, U>;

template <typename T>
concept tuple_comparable = requires { typename T::common_tuple_comparable; };

template <typename T>
concept tuplelike = requires { typename std::remove_cvref_t<T>::is_tuple; };

namespace detail {
template <typename T>
concept has_vacuous_tuple_protocol = requires {
    {
        tuple_size_v<std::remove_cvref_t<T>>
    } -> std::same_as<std::size_t const &>;
};
template <typename T>
concept is_vacuous_tuple = tuple_size_v<std::remove_cvref_t<T>> == 0;

constexpr inline auto concept_try_get = [](auto &x) -> decltype(auto) {
    using std::get;
    return get<0>(x);
};

template <typename T>
concept has_get_protocol = requires(T &t) {
    {
        concept_try_get(t)
    } -> same_as_unqualified<tuple_element_t<0, std::remove_cvref_t<T>>>;
};
} // namespace detail

template <typename T>
concept has_tuple_protocol = detail::has_vacuous_tuple_protocol<T> and
                             (detail::is_vacuous_tuple<T> or
                              detail::has_get_protocol<std::remove_cvref_t<T>>);

template <typename T>
concept has_array_protocol = has_tuple_protocol<T> and requires {
    typename std::remove_cvref_t<T>::value_type;
};

} // namespace v1
} // namespace stdx

#pragma once

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
namespace detail {
template <typename T> struct ct_capacity_fail {
    static_assert(always_false_v<stdx::remove_cvref_t<T>>,
                  "Type does not support compile-time capacity");
};
} // namespace detail

template <typename T>
constexpr auto ct_capacity_v = detail::ct_capacity_fail<T>{};

template <typename T, std::size_t N>
constexpr auto ct_capacity_v<std::array<T, N>> = N;

template <typename T> constexpr auto ct_capacity_v<T const> = ct_capacity_v<T>;

template <typename T>
CONSTEVAL auto ct_capacity([[maybe_unused]] T &&) -> std::size_t {
    return ct_capacity_v<remove_cvref_t<T>>;
}

template <typename T = int, typename = std::enable_if_t<std::is_integral_v<T>>>
struct counting_iterator {
    using difference_type = decltype(std::declval<T>() - std::declval<T>());
    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
#if __cplusplus >= 202002L
    using iterator_category = std::contiguous_iterator_tag;
#else
    using iterator_category = std::random_access_iterator_tag;
#endif

    auto operator*() -> reference { return i; }
    auto operator*() const -> const_reference { return i; }

    constexpr auto operator++() -> counting_iterator & {
        ++i;
        return *this;
    }
    [[nodiscard]] constexpr auto operator++(int) -> counting_iterator {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr auto operator--() -> counting_iterator & {
        --i;
        return *this;
    }
    [[nodiscard]] constexpr auto operator--(int) -> counting_iterator {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    constexpr auto operator+=(difference_type d) -> counting_iterator & {
        i += d;
        return *this;
    }
    constexpr auto operator-=(difference_type d) -> counting_iterator & {
        i -= d;
        return *this;
    }
    constexpr auto advance(difference_type d) -> counting_iterator & {
        i += d;
        return *this;
    }

    [[nodiscard]] friend constexpr auto operator+(counting_iterator ci,
                                                  difference_type d)
        -> counting_iterator {
        ci += d;
        return ci;
    }
    [[nodiscard]] friend constexpr auto operator+(difference_type d,
                                                  counting_iterator ci)
        -> counting_iterator {
        ci += d;
        return ci;
    }
    [[nodiscard]] friend constexpr auto operator-(counting_iterator ci,
                                                  difference_type d)
        -> counting_iterator {
        ci -= d;
        return ci;
    }
    [[nodiscard]] friend constexpr auto operator-(counting_iterator x,
                                                  counting_iterator y)
        -> difference_type {
        return x.i - y.i;
    }

    [[nodiscard]] friend constexpr auto operator==(counting_iterator const &x,
                                                   counting_iterator const &y)
        -> bool {
        return x.i == y.i;
    }

#if __cpp_impl_three_way_comparison >= 201907L
    [[nodiscard]] friend constexpr auto
    operator<=>(counting_iterator const &x, counting_iterator const &y) {
        return x.i <=> y.i;
    }
#else
    [[nodiscard]] friend constexpr auto operator!=(counting_iterator const &x,
                                                   counting_iterator const &y)
        -> bool {
        return not(x == y);
    }

    [[nodiscard]] friend constexpr auto operator<(counting_iterator const &x,
                                                  counting_iterator const &y)
        -> bool {
        return x.i < y.i;
    }
    [[nodiscard]] friend constexpr auto operator<=(counting_iterator const &x,
                                                   counting_iterator const &y)
        -> bool {
        return not(y < x);
    }
    [[nodiscard]] friend constexpr auto operator>(counting_iterator const &x,
                                                  counting_iterator const &y)
        -> bool {
        return y < x;
    }
    [[nodiscard]] friend constexpr auto operator>=(counting_iterator const &x,
                                                   counting_iterator const &y)
        -> bool {
        return not(x < y);
    }
#endif

    T i{};
};

template <typename T> counting_iterator(T) -> counting_iterator<T>;

} // namespace v1
} // namespace stdx

#if __has_include(<span>)
#include <span>

#if __cpp_lib_span >= 202002L
namespace stdx {
inline namespace v1 {
template <typename T, std::size_t N>
constexpr auto ct_capacity_v<std::span<T, N>> = N;

template <typename T>
constexpr auto ct_capacity_v<std::span<T, std::dynamic_extent>> =
    detail::ct_capacity_fail<std::span<T, std::dynamic_extent>>{};
} // namespace v1
} // namespace stdx

#endif
#endif

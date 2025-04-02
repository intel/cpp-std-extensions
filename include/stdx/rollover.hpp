#pragma once

#include <stdx/concepts.hpp>

#include <type_traits>

namespace stdx {
inline namespace v1 {
template <typename T> struct rollover_t {
    static_assert(unsigned_integral<T>,
                  "Argument to rollover_t must be an unsigned integral type.");
    using underlying_t = T;

    constexpr rollover_t() = default;
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr explicit rollover_t(U u) : value{static_cast<underlying_t>(u)} {}
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr explicit rollover_t(rollover_t<U> u)
        : rollover_t{static_cast<U>(u)} {}

    [[nodiscard]] constexpr auto as_underlying() const -> underlying_t {
        return value;
    }
    constexpr explicit operator underlying_t() const { return value; }

    [[nodiscard]] constexpr auto operator+() const -> rollover_t {
        return *this;
    }
    [[nodiscard]] constexpr auto operator-() const -> rollover_t {
        return rollover_t{static_cast<underlying_t>(-value)};
    }

    constexpr auto operator++() -> rollover_t & {
        ++value;
        return *this;
    }
    constexpr auto operator++(int) -> rollover_t { return rollover_t{value++}; }

    constexpr auto operator--() -> rollover_t & {
        --value;
        return *this;
    }
    constexpr auto operator--(int) -> rollover_t { return rollover_t{value--}; }

    constexpr auto operator+=(rollover_t other) -> rollover_t & {
        value += other.value;
        return *this;
    }
    constexpr auto operator-=(rollover_t other) -> rollover_t & {
        value -= other.value;
        return *this;
    }
    constexpr auto operator*=(rollover_t other) -> rollover_t & {
        value *= other.value;
        return *this;
    }
    constexpr auto operator/=(rollover_t other) -> rollover_t & {
        value /= other.value;
        return *this;
    }
    constexpr auto operator%=(rollover_t other) -> rollover_t & {
        value %= other.value;
        return *this;
    }

  private:
    [[nodiscard]] friend constexpr auto operator==(rollover_t lhs,
                                                   rollover_t rhs) -> bool {
        return lhs.value == rhs.value;
    }
    [[nodiscard]] friend constexpr auto operator!=(rollover_t lhs,
                                                   rollover_t rhs) -> bool {
        return not(lhs == rhs);
    }

    friend constexpr auto operator<(rollover_t, rollover_t) -> bool = delete;
    friend constexpr auto operator<=(rollover_t, rollover_t) -> bool = delete;
    friend constexpr auto operator>(rollover_t, rollover_t) -> bool = delete;
    friend constexpr auto operator>=(rollover_t, rollover_t) -> bool = delete;

    [[nodiscard]] friend constexpr auto cmp_less(rollover_t lhs, rollover_t rhs)
        -> bool {
        constexpr auto mid = static_cast<underlying_t>(~underlying_t{}) / 2;
        return static_cast<underlying_t>(lhs.value - rhs.value) > mid;
    }

    [[nodiscard]] friend constexpr auto operator+(rollover_t lhs,
                                                  rollover_t rhs)
        -> rollover_t {
        lhs += rhs;
        return lhs;
    }
    [[nodiscard]] friend constexpr auto operator-(rollover_t lhs,
                                                  rollover_t rhs)
        -> rollover_t {
        lhs -= rhs;
        return lhs;
    }
    [[nodiscard]] friend constexpr auto operator*(rollover_t lhs,
                                                  rollover_t rhs)
        -> rollover_t {
        lhs *= rhs;
        return lhs;
    }
    [[nodiscard]] friend constexpr auto operator/(rollover_t lhs,
                                                  rollover_t rhs)
        -> rollover_t {
        lhs /= rhs;
        return lhs;
    }
    [[nodiscard]] friend constexpr auto operator%(rollover_t lhs,
                                                  rollover_t rhs)
        -> rollover_t {
        lhs %= rhs;
        return lhs;
    }

    underlying_t value{};
};

template <typename T> rollover_t(T) -> rollover_t<T>;
} // namespace v1
} // namespace stdx

template <typename T, typename U>
struct std::common_type<stdx::rollover_t<T>, stdx::rollover_t<U>> {
    using type = stdx::rollover_t<std::common_type_t<T, U>>;
};

template <typename T, typename I>
struct std::common_type<stdx::rollover_t<T>, I> {
    using type =
        stdx::rollover_t<std::common_type_t<T, std::make_unsigned_t<I>>>;
};

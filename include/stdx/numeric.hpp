#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
#if __cplusplus >= 202002L
#define CONSTEXPR_INVOKE constexpr
#else
#define CONSTEXPR_INVOKE
#endif

template <typename T, typename InputIt, typename ROp, typename TOp,
          typename... InputItN>
CONSTEXPR_INVOKE auto transform_reduce(InputIt first, InputIt last, T init,
                                       ROp rop, TOp top, InputItN... first_n)
    -> T {
    while (first != last) {
        init = std::invoke(rop, std::move(init),
                           std::invoke(top, *first, *first_n...));
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return init;
}

template <typename T, typename InputIt, typename Size, typename ROp,
          typename TOp, typename... InputItN>
CONSTEXPR_INVOKE auto transform_reduce_n(InputIt first, Size n, T init, ROp rop,
                                         TOp top, InputItN... first_n) -> T {
    while (n-- != 0) {
        init = std::invoke(rop, std::move(init),
                           std::invoke(top, *first, *first_n...));
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return init;
}
#undef CONSTEXPR_INVOKE

template <typename To, typename From,
          typename = std::enable_if_t<std::is_integral_v<To> &&
                                      std::is_integral_v<From>>>
constexpr auto saturate_cast(From from) noexcept -> To {
    [[maybe_unused]] constexpr int digits_to = std::numeric_limits<To>::digits;
    [[maybe_unused]] constexpr int _digits_from =
        std::numeric_limits<From>::digits;
    [[maybe_unused]] constexpr To max_to = std::numeric_limits<To>::max();

    if constexpr (std::is_signed_v<To> == std::is_signed_v<From>) {
        if constexpr (digits_to < _digits_from) {
            [[maybe_unused]] constexpr To min_to =
                std::numeric_limits<To>::min();

            if (from < static_cast<From>(min_to)) {
                return min_to;
            }
            if (from > static_cast<From>(max_to)) {
                return max_to;
            }
        }
    } else if constexpr (std::is_signed_v<From>) // To is unsigned
    {
        if (from < 0) {
            return 0;
        }
        if (std::make_unsigned_t<From>(from) > max_to) {
            return std::numeric_limits<To>::max();
        }
    } else // From is unsigned, To is signed
    {
        if (from > std::make_unsigned_t<To>(max_to)) {
            return max_to;
        }
    }
    return static_cast<To>(from);
}

} // namespace v1
} // namespace stdx

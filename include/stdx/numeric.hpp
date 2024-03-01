#pragma once

#include <functional>
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
} // namespace v1
} // namespace stdx

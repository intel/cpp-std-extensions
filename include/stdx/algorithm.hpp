#pragma once

#include <stdx/tuple.hpp>

#include <functional>
#include <iterator>

namespace stdx {
inline namespace v1 {
namespace detail {
template <typename... Ts> using result_tuple_t = stdx::tuple<Ts...>;
} // namespace detail

template <typename O, typename... Is>
using transform_result = detail::result_tuple_t<O, Is...>;

template <typename InputIt, typename OutputIt, typename Operation,
          typename... InputItN>
constexpr auto transform(InputIt first, InputIt last, OutputIt d_first,
                         Operation op, InputItN... first_n)
    -> transform_result<OutputIt, InputIt, InputItN...> {
    while (first != last) {
        *d_first = std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...),
            ++d_first;
    }
    return {d_first, first, first_n...};
}

template <typename InputIt, typename Size, typename OutputIt,
          typename Operation, typename... InputItN>
constexpr auto transform_n(InputIt first, Size n, OutputIt d_first,
                           Operation op, InputItN... first_n)
    -> transform_result<OutputIt, InputIt, InputItN...> {
    while (n-- > 0) {
        *d_first = std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...),
            ++d_first;
    }
    return {d_first, first, first_n...};
}

template <typename Op, typename... Is>
using for_each_result = detail::result_tuple_t<Op, Is...>;

template <typename InputIt, typename Operation, typename... InputItN>
constexpr auto for_each(InputIt first, InputIt last, Operation op,
                        InputItN... first_n)
    -> for_each_result<Operation, InputItN...> {
    while (first != last) {
        std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return {op, first_n...};
}

template <typename InputIt, typename Size, typename Operation,
          typename... InputItN>
constexpr auto for_each_n(InputIt first, Size n, Operation op,
                          InputItN... first_n)
    -> for_each_result<Operation, InputIt, InputItN...> {
    while (n-- > 0) {
        std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return {op, first, first_n...};
}

namespace detail {
template <typename FwdIt, typename N, typename Operation, typename... FwdItN>
constexpr auto for_each_butlastn(std::forward_iterator_tag, FwdIt first,
                                 FwdIt last, N n, Operation op,
                                 FwdItN... first_n)
    -> for_each_result<Operation, FwdIt, FwdItN...> {
    auto adv_it = first;
    for (auto i = N{}; i < n; ++i) {
        if (adv_it == last) {
            break;
        }
        ++adv_it;
    }

    while (adv_it != last) {
        std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
        ++adv_it;
    }
    return {op, first, first_n...};
}

template <typename RandIt, typename N, typename Operation, typename... RandItN>
constexpr auto for_each_butlastn(std::random_access_iterator_tag, RandIt first,
                                 RandIt last, N n, Operation op,
                                 RandItN... first_n) {
    auto const sz = std::distance(first, last);
    return for_each_n(first, sz - static_cast<decltype(sz)>(n), op, first_n...);
}
} // namespace detail

template <typename FwdIt, typename N, typename Operation, typename... FwdItN>
constexpr auto for_each_butlastn(FwdIt first, FwdIt last, N n, Operation op,
                                 FwdItN... first_n) {
    return detail::for_each_butlastn(
        typename std::iterator_traits<FwdIt>::iterator_category{}, first, last,
        n, op, first_n...);
}

template <typename FwdIt, typename Operation, typename... FwdItN>
constexpr auto for_each_butlast(FwdIt first, FwdIt last, Operation op,
                                FwdItN... first_n) {
    return for_each_butlastn(first, last, 1, op, first_n...);
}

template <typename FwdIt, typename IOp, typename MOp, typename FOp>
constexpr auto initial_medial_final(FwdIt first, FwdIt last, IOp iop, MOp mop,
                                    FOp fop) -> for_each_result<IOp, MOp, FOp> {
    if (first != last) {
        iop(*first);
        auto r = for_each_butlast(++first, last, mop);
        if (auto it = get<1>(r); it != last) {
            fop(*it);
        }
        return {iop, get<0>(r), fop};
    }
    return {iop, mop, fop};
}
} // namespace v1
} // namespace stdx

#pragma once

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/integral.hpp>

#include <limits>

namespace stdx {
inline namespace v1 {
namespace mp {
namespace detail {
template <template <typename...> typename P> struct mask_bit_q {
    template <typename T>
    using fn = boost::mp11::mp_size_t<P<T>::value ? 1 : 0>;
};

template <typename L, template <typename...> typename P>
using list_mask_t = boost::mp11::mp_transform_q<mask_bit_q<P>, L>;

template <typename L, template <typename...> typename P>
using list_mask_scan_t = boost::mp11::mp_partial_sum<
    list_mask_t<L, P>,
    boost::mp11::mp_size_t<std::numeric_limits<std::size_t>::max()>,
    boost::mp11::mp_plus>;
} // namespace detail

template <typename L, template <typename...> typename P, typename I>
using filtered_index =
    boost::mp11::mp_if<P<boost::mp11::mp_at<L, I>>,
                       boost::mp11::mp_at<detail::list_mask_scan_t<L, P>, I>,
                       boost::mp11::mp_size<boost::mp11::mp_copy_if<L, P>>>;

template <typename L, template <typename...> typename P, std::size_t I>
using filtered_index_c = filtered_index<L, P, boost::mp11::mp_size_t<I>>;
} // namespace mp
} // namespace v1
} // namespace stdx

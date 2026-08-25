#include <stdx/meta.hpp>

#include <boost/mp11/algorithm.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {
using list_t = boost::mp11::mp_iota_c<5>;
template <typename T> using is_even = std::bool_constant<T::value % 2 == 0>;
} // namespace

TEST_CASE("filtered_index (items that pass the predicate)", "[meta]") {
    STATIC_CHECK(
        std::same_as<stdx::mp::filtered_index<list_t, is_even,
                                              boost::mp11::mp_size_t<0>>,
                     boost::mp11::mp_size_t<0>>);
    STATIC_CHECK(
        std::same_as<stdx::mp::filtered_index<list_t, is_even,
                                              boost::mp11::mp_size_t<2>>,
                     boost::mp11::mp_size_t<1>>);
    STATIC_CHECK(
        std::same_as<stdx::mp::filtered_index<list_t, is_even,
                                              boost::mp11::mp_size_t<4>>,
                     boost::mp11::mp_size_t<2>>);
}

TEST_CASE("filtered_index (items that fail the predicate)", "[meta]") {
    STATIC_CHECK(
        std::same_as<stdx::mp::filtered_index<list_t, is_even,
                                              boost::mp11::mp_size_t<1>>,
                     boost::mp11::mp_size_t<3>>);
    STATIC_CHECK(
        std::same_as<stdx::mp::filtered_index<list_t, is_even,
                                              boost::mp11::mp_size_t<3>>,
                     boost::mp11::mp_size_t<3>>);
}

TEST_CASE("filtered_index_c", "[meta]") {
    STATIC_CHECK(std::same_as<stdx::mp::filtered_index_c<list_t, is_even, 0>,
                              boost::mp11::mp_size_t<0>>);
    STATIC_CHECK(std::same_as<stdx::mp::filtered_index_c<list_t, is_even, 2>,
                              boost::mp11::mp_size_t<1>>);
    STATIC_CHECK(std::same_as<stdx::mp::filtered_index_c<list_t, is_even, 4>,
                              boost::mp11::mp_size_t<2>>);
}

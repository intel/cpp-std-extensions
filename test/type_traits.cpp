#include <stdx/ct_conversions.hpp>
#include <stdx/type_traits.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace {
template <typename> struct unary_t {};
template <typename...> struct variadic_t {};
template <typename T> struct derived_t : unary_t<T> {};
} // namespace

TEST_CASE("detect specializations", "[type_traits]") {
    static_assert(stdx::is_specialization_of_v<unary_t<int>, unary_t>);
    static_assert(stdx::is_type_specialization_of_v<unary_t<int>, unary_t>);
    static_assert(stdx::is_specialization_of<unary_t<int>, unary_t>());

    static_assert(not stdx::is_specialization_of_v<int, unary_t>);
    static_assert(not stdx::is_type_specialization_of_v<int, unary_t>);
    static_assert(not stdx::is_specialization_of<int, unary_t>());

    static_assert(stdx::is_specialization_of_v<variadic_t<>, variadic_t>);
    static_assert(stdx::is_type_specialization_of_v<variadic_t<>, variadic_t>);
    static_assert(stdx::is_specialization_of<variadic_t<>, variadic_t>());

    static_assert(not stdx::is_specialization_of_v<int, variadic_t>);
    static_assert(not stdx::is_type_specialization_of_v<int, variadic_t>);
    static_assert(not stdx::is_specialization_of<int, variadic_t>());
}

TEST_CASE("derived types are not specializations", "[type_traits]") {
    static_assert(not stdx::is_specialization_of_v<derived_t<int>, unary_t>);
    static_assert(
        not stdx::is_type_specialization_of_v<derived_t<int>, unary_t>);
    static_assert(not stdx::is_specialization_of<derived_t<int>, unary_t>());
}

namespace {
template <auto> struct value_unary_t {};
template <auto...> struct value_variadic_t {};
template <auto V> struct value_derived_t : value_unary_t<V> {};
} // namespace

TEST_CASE("detect specializations (value templates)", "[type_traits]") {
    static_assert(
        stdx::is_value_specialization_of_v<value_unary_t<0>, value_unary_t>);
    static_assert(
        stdx::is_specialization_of<value_unary_t<0>, value_unary_t>());

    static_assert(not stdx::is_value_specialization_of_v<int, value_unary_t>);
    static_assert(not stdx::is_specialization_of<int, value_unary_t>());

    static_assert(stdx::is_value_specialization_of_v<value_variadic_t<>,
                                                     value_variadic_t>);
    static_assert(
        stdx::is_specialization_of<value_variadic_t<>, value_variadic_t>());

    static_assert(
        not stdx::is_value_specialization_of_v<int, value_variadic_t>);
    static_assert(not stdx::is_specialization_of<int, value_variadic_t>());
}

TEST_CASE("derived types are not specializations (value templates)",
          "[type_traits]") {
    static_assert(not stdx::is_value_specialization_of_v<value_derived_t<0>,
                                                         value_unary_t>);
    static_assert(
        not stdx::is_specialization_of<value_derived_t<0>, value_unary_t>());
}

namespace {
enum E1 {};
enum struct E2 {};
} // namespace

TEST_CASE("is_scoped_enum", "[type_traits]") {
    static_assert(not stdx::is_scoped_enum_v<int>);
    static_assert(not stdx::is_scoped_enum_v<E1>);
    static_assert(stdx::is_scoped_enum_v<E2>);

    static_assert(not stdx::is_scoped_enum<int>::value);
    static_assert(not stdx::is_scoped_enum<E1>::value);
    static_assert(stdx::is_scoped_enum<E2>::value);
}

TEST_CASE("type_identity", "[type_traits]") {
    static_assert(std::is_same_v<stdx::type_identity_t<void>, void>);
}

TEST_CASE("type_or_t", "[type_traits]") {
    static_assert(
        std::is_same_v<stdx::type_or_t<std::is_void, void, int>, void>);
    static_assert(
        std::is_same_v<stdx::type_or_t<std::is_void, int, float>, float>);
    static_assert(std::is_same_v<stdx::type_or_t<std::is_void, int>, void>);
}

namespace {
int value{};
struct add_value {
    add_value() = default;
    add_value(int init) { value = init; }

    template <typename T> constexpr auto operator()() const -> void {
        value += T::value;
    }
    template <auto T> constexpr auto operator()() const -> void { value += T; }
};

struct add_values {
    template <typename... Ts> constexpr auto operator()() const {
        return (0 + ... + Ts::value);
    }
    template <auto... Ts> constexpr auto operator()() const {
        return (0 + ... + Ts);
    }
};
} // namespace

TEST_CASE("template_for_each with type list", "[type_traits]") {
    value = 0;
    using L = stdx::type_list<std::integral_constant<int, 1>,
                              std::integral_constant<int, 2>>;
    stdx::template_for_each<L>(add_value{});
    CHECK(value == 3);
}

TEST_CASE("template_for_each with empty type list", "[type_traits]") {
    using L = stdx::type_list<>;
    stdx::template_for_each<L>(add_value{17});
    CHECK(value == 17);
}

TEST_CASE("template_for_each with value list", "[type_traits]") {
    using L = stdx::value_list<1, 2>;
    stdx::template_for_each<L>(add_value{0});
    CHECK(value == 3);
}

TEST_CASE("template_for_each with empty value list", "[type_traits]") {
    using L = stdx::value_list<>;
    stdx::template_for_each<L>(add_value{17});
    CHECK(value == 17);
}

TEST_CASE("template_for_each with index sequence", "[type_traits]") {
    using L = std::make_index_sequence<3>;
    stdx::template_for_each<L>(add_value{0});
    CHECK(value == 3);
}

TEST_CASE("apply_sequence with type list", "[type_traits]") {
    using L = stdx::type_list<std::integral_constant<int, 1>,
                              std::integral_constant<int, 2>>;
    CHECK(stdx::apply_sequence<L>(add_values{}) == 3);
}

TEST_CASE("apply_sequence with value list", "[type_traits]") {
    using L = stdx::value_list<1, 2>;
    CHECK(stdx::apply_sequence<L>(add_values{}) == 3);
}

TEST_CASE("apply_sequence with index sequence", "[type_traits]") {
    using L = std::make_index_sequence<3>;
    CHECK(stdx::apply_sequence<L>(add_values{}) == 3);
}

TEST_CASE("is_same_unqualified", "[type_traits]") {
    static_assert(stdx::is_same_unqualified_v<int, int>);
    static_assert(not stdx::is_same_unqualified_v<int, void>);
    static_assert(stdx::is_same_unqualified_v<int, int &>);
    static_assert(stdx::is_same_unqualified_v<int, int const &>);
    static_assert(stdx::is_same_unqualified_v<int, int &&>);
    static_assert(stdx::is_same_unqualified_v<int, int const &&>);
    static_assert(stdx::is_same_unqualified_v<int &, int>);
    static_assert(stdx::is_same_unqualified_v<int const &, int>);
    static_assert(stdx::is_same_unqualified_v<int &&, int>);
    static_assert(stdx::is_same_unqualified_v<int const &&, int>);
}

// for a taxonomy of structural types below, see
// https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter

namespace structural {
struct base_t {
    int x;
    auto f() { return 42; }
};
using pmd_t = decltype(&base_t::x);
using pmf_t = decltype(&base_t::f);

struct derived_t : base_t {
    constexpr derived_t(int _x, int _y) : base_t{_x}, y{_y} {}
    int y;
};

struct class_t : base_t {
    constexpr class_t(int _x, int _y) : base_t{_x}, d{_x, _y}, y{_y} {}
    derived_t d;
    int y;
};

enum struct enum_t {};

constexpr auto l = [](int) { return 42; };
using lambda_t = decltype(l);

using union_t = union {
    int x;
    int y;
};
} // namespace structural

TEST_CASE("structural types", "[type_traits]") {
    static_assert(stdx::is_structural_v<int &>);
    static_assert(stdx::is_structural_v<int>);
    static_assert(stdx::is_structural_v<int *>);
    static_assert(stdx::is_structural_v<structural::pmd_t>);
    static_assert(stdx::is_structural_v<structural::pmf_t>);
    static_assert(stdx::is_structural_v<structural::enum_t>);
    static_assert(stdx::is_structural_v<std::nullptr_t>);

#if __cpp_nontype_template_args >= 201911L
    static_assert(stdx::is_structural_v<structural::base_t>);
    static_assert(stdx::is_structural_v<structural::derived_t>);
    static_assert(stdx::is_structural_v<structural::class_t>);
    static_assert(stdx::is_structural_v<structural::lambda_t>);
    static_assert(stdx::is_structural_v<structural::union_t>);
    static_assert(stdx::is_structural_v<float>);
#endif
}

namespace non_structural {
struct S {
    ~S() {} // nontrivial destructor
};
} // namespace non_structural

TEST_CASE("non-structural types", "[type_traits]") {
    static_assert(not stdx::is_structural_v<non_structural::S>);
}

#if __cplusplus >= 202002L
namespace {
template <typename...> struct long_type_name {};
} // namespace

TEST_CASE("type shrinkage", "[type_traits]") {
    using A = long_type_name<int, int, int, int, int, int, int, int>;
    using B = long_type_name<A, A, A, A, A, A, A, A>;
    using C = long_type_name<B, B, B, B, B, B, B, B>;
    using X = stdx::shrink_t<C>;
    static_assert(stdx::type_as_string<X>().size() <
                  stdx::type_as_string<C>().size());
    static_assert(std::same_as<stdx::expand_t<X>, C>);
}
#endif

#include <stdx/type_traits.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

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
    static_assert(not stdx::is_scoped_enum_v<E1>);
    static_assert(stdx::is_scoped_enum_v<E2>);
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
    template <typename T> constexpr auto operator()() const -> void {
        value += T::value;
    }
    template <auto T> constexpr auto operator()() const -> void { value += T; }
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
    value = 42;
    using L = stdx::type_list<>;
    stdx::template_for_each<L>(add_value{});
    CHECK(value == 42);
}

TEST_CASE("template_for_each with value list", "[type_traits]") {
    value = 0;
    using L = stdx::value_list<1, 2>;
    stdx::template_for_each<L>(add_value{});
    CHECK(value == 3);
}

TEST_CASE("template_for_each with empty value list", "[type_traits]") {
    value = 17;
    using L = stdx::value_list<>;
    stdx::template_for_each<L>(add_value{});
    CHECK(value == 17);
}

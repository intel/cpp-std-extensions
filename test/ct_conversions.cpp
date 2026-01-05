#include <stdx/ct_conversions.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string_view>

struct complete {};
struct incomplete;

TEST_CASE("type as string", "[ct_conversion]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::type_as_string<int>() == "int"sv);
    STATIC_REQUIRE(stdx::type_as_string<complete>() == "complete"sv);
    STATIC_REQUIRE(stdx::type_as_string<incomplete>() == "incomplete"sv);
}

template <typename T> struct type_template;

template <auto T> struct value_template;

TEST_CASE("template base", "[ct_conversion]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::template_base<type_template<int>>() ==
                   "type_template"sv);
    STATIC_REQUIRE(stdx::template_base<value_template<42>>() ==
                   "value_template"sv);
    STATIC_REQUIRE(stdx::template_base<int>() == "int"sv);
}

namespace {
enum A { X };
enum struct B { Y };
} // namespace

TEST_CASE("enum as string", "[ct_conversion]") {
    using namespace std::string_view_literals;
    STATIC_REQUIRE(stdx::enum_as_string<X>() == "X"sv);
    STATIC_REQUIRE(stdx::enum_as_string<B::Y>() == "Y"sv);
}

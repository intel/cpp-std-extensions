#include <stdx/concepts.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <type_traits>

TEST_CASE("integral", "[concepts]") {
    STATIC_REQUIRE(stdx::integral<int>);
    STATIC_REQUIRE(not stdx::integral<float>);
}

TEST_CASE("floating_point", "[concepts]") {
    STATIC_REQUIRE(stdx::floating_point<float>);
    STATIC_REQUIRE(not stdx::floating_point<int>);
}

TEST_CASE("signed_integral", "[concepts]") {
    STATIC_REQUIRE(stdx::signed_integral<int>);
    STATIC_REQUIRE(not stdx::signed_integral<unsigned int>);
}

TEST_CASE("unsigned_integral", "[concepts]") {
    STATIC_REQUIRE(stdx::unsigned_integral<unsigned int>);
    STATIC_REQUIRE(not stdx::unsigned_integral<int>);
}

TEST_CASE("same_as", "[concepts]") {
    STATIC_REQUIRE(stdx::same_as<int, int>);
    STATIC_REQUIRE(not stdx::same_as<float, int>);
}

TEST_CASE("same_any", "[concepts]") {
    STATIC_REQUIRE(stdx::same_any<int, float, bool, int>);
    STATIC_REQUIRE(not stdx::same_any<float, char, bool, int>);
}

TEST_CASE("same_none", "[concepts]") {
    STATIC_REQUIRE(stdx::same_none<int, float, bool, char>);
    STATIC_REQUIRE(not stdx::same_none<float, bool, char, float>);
}

TEST_CASE("same_as_unqualified", "[concepts]") {
    STATIC_REQUIRE(stdx::same_as_unqualified<int, int>);
    STATIC_REQUIRE(not stdx::same_as_unqualified<int, void>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int, int &>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int, int const &>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int, int &&>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int, int const &&>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int &, int>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int const &, int>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int &&, int>);
    STATIC_REQUIRE(stdx::same_as_unqualified<int const &&, int>);
}

TEST_CASE("convertible_to", "[concepts]") {
    STATIC_REQUIRE(stdx::convertible_to<char, int>);
    STATIC_REQUIRE(not stdx::convertible_to<float *, int *>);
}

namespace {
struct S {};
} // namespace

TEST_CASE("equality_comparable", "[concepts]") {
    STATIC_REQUIRE(stdx::equality_comparable<int>);
    STATIC_REQUIRE(not stdx::equality_comparable<S>);
}

TEST_CASE("totally_ordered", "[concepts]") {
    STATIC_REQUIRE(stdx::totally_ordered<int>);
    STATIC_REQUIRE(not stdx::totally_ordered<S>);
}

namespace {
struct A {};
struct B : A {};
struct C : private A {};
} // namespace

TEST_CASE("derived_from", "[concepts]") {
    STATIC_REQUIRE(stdx::derived_from<A, A>);
    STATIC_REQUIRE(stdx::derived_from<B, A>);
    STATIC_REQUIRE(not stdx::derived_from<C, A>);
    STATIC_REQUIRE(not stdx::derived_from<int, int>);
}

TEST_CASE("invocable", "[concepts]") {
    using invocable_no_args = auto (*)()->void;
    using invocable_int = auto (*)(int)->void;
    [[maybe_unused]] constexpr auto l_invocable_no_args = [] {};
    [[maybe_unused]] constexpr auto l_invocable_int = [](int) {};

    STATIC_REQUIRE(not stdx::invocable<int>);
    STATIC_REQUIRE(stdx::invocable<invocable_no_args>);
    STATIC_REQUIRE(not stdx::invocable<invocable_no_args, int>);
    STATIC_REQUIRE(stdx::invocable<invocable_int, int>);
    STATIC_REQUIRE(not stdx::invocable<invocable_int>);
    STATIC_REQUIRE(stdx::invocable<decltype(l_invocable_no_args)>);
    STATIC_REQUIRE(not stdx::invocable<decltype(l_invocable_no_args), int>);
    STATIC_REQUIRE(stdx::invocable<decltype(l_invocable_int), int>);
    STATIC_REQUIRE(not stdx::invocable<decltype(l_invocable_int)>);
}

TEST_CASE("predicate (negative cases)", "[concepts]") {
    using not_predicate_no_args = auto (*)()->void;
    using not_predicate_int = auto (*)(int)->void;
    [[maybe_unused]] constexpr auto l_not_predicate_no_args = [] {};
    [[maybe_unused]] constexpr auto l_not_predicate_int = [](int) {};

    STATIC_REQUIRE(not stdx::predicate<not_predicate_no_args>);
    STATIC_REQUIRE(not stdx::predicate<not_predicate_int, int>);
    STATIC_REQUIRE(not stdx::predicate<decltype(l_not_predicate_no_args)>);
    STATIC_REQUIRE(not stdx::predicate<decltype(l_not_predicate_int), int>);
}

TEST_CASE("predicate (positive cases)", "[concepts]") {
    using predicate_no_args = auto (*)()->bool;
    using predicate_int = auto (*)(int)->bool;
    [[maybe_unused]] constexpr auto l_predicate_no_args = [] { return true; };
    [[maybe_unused]] constexpr auto l_predicate_int = [](int) { return true; };
    [[maybe_unused]] constexpr auto convert_predicate_no_args = [] {
        return std::true_type{};
    };
    [[maybe_unused]] constexpr auto convert_predicate_int = [](int) {
        return std::true_type{};
    };

    STATIC_REQUIRE(stdx::predicate<predicate_no_args>);
    STATIC_REQUIRE(stdx::predicate<predicate_int, int>);
    STATIC_REQUIRE(stdx::predicate<decltype(l_predicate_no_args)>);
    STATIC_REQUIRE(stdx::predicate<decltype(l_predicate_int), int>);
    STATIC_REQUIRE(stdx::predicate<decltype(convert_predicate_no_args)>);
    STATIC_REQUIRE(stdx::predicate<decltype(convert_predicate_int), int>);
}

namespace {
[[maybe_unused]] auto func_no_args() {}
[[maybe_unused]] auto func_one_arg(int) {}

struct funcobj {
    auto operator()() {}
};
struct generic_funcobj {
    template <typename> auto operator()() {}
};
} // namespace

TEST_CASE("callable", "[concepts]") {
    [[maybe_unused]] constexpr auto l_callable_no_args = [] {};
    [[maybe_unused]] constexpr auto l_callable_int = [](int) {};
    [[maybe_unused]] constexpr auto l_callable_generic = [](auto) {};

    STATIC_REQUIRE(not stdx::callable<int>);
    STATIC_REQUIRE(stdx::callable<decltype(func_no_args)>);
    STATIC_REQUIRE(stdx::callable<decltype(func_one_arg)>);
    STATIC_REQUIRE(stdx::callable<funcobj>);
    STATIC_REQUIRE(stdx::callable<generic_funcobj>);
    STATIC_REQUIRE(stdx::callable<decltype(l_callable_no_args)>);
    STATIC_REQUIRE(stdx::callable<decltype(l_callable_int)>);
    STATIC_REQUIRE(stdx::callable<decltype(l_callable_generic)>);
}

TEST_CASE("models_trait", "[concepts]") {
    STATIC_REQUIRE(stdx::has_trait<int *, std::is_pointer>);
    STATIC_REQUIRE(not stdx::has_trait<int, std::is_pointer>);
}

namespace {
struct non_structural {
    ~non_structural() {} // nontrivial destructor
};
} // namespace

TEST_CASE("structural", "[type_traits]") {
    STATIC_REQUIRE(stdx::structural<int>);
    STATIC_REQUIRE(not stdx::structural<non_structural>);
}

TEST_CASE("complete", "[type_traits]") {
    struct incomplete;
    STATIC_REQUIRE(stdx::complete<int>);
    STATIC_REQUIRE(not stdx::complete<incomplete>);
}

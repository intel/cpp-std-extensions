#include <stdx/function_traits.hpp>
#include <stdx/priority.hpp>

#include <catch2/catch_test_macros.hpp>

#include <tuple>
#include <type_traits>

namespace {
[[maybe_unused]] auto func_no_args() -> void {}
[[maybe_unused]] auto func_one_arg(int) -> int { return 0; }
[[maybe_unused]] auto func_ref_arg(int &) -> void {}
} // namespace

TEST_CASE("function return type", "[function_traits]") {
    STATIC_REQUIRE(std::is_void_v<typename stdx::function_traits<
                       decltype(func_no_args)>::return_type>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(func_no_args)>>);

    STATIC_REQUIRE(
        std::is_same_v<
            typename stdx::function_traits<decltype(func_one_arg)>::return_type,
            int>);
    STATIC_REQUIRE(std::is_same_v<stdx::return_t<decltype(func_one_arg)>, int>);
}

TEST_CASE("function pointer return type", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_void_v<typename stdx::function_traits<
            std::add_pointer_t<decltype(func_no_args)>>::return_type>);
    STATIC_REQUIRE(std::is_void_v<
                   stdx::return_t<std::add_pointer_t<decltype(func_no_args)>>>);

    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<std::add_pointer_t<
                           decltype(func_one_arg)>>::return_type,
                       int>);
    STATIC_REQUIRE(
        std::is_same_v<
            stdx::return_t<std::add_pointer_t<decltype(func_one_arg)>>, int>);
}

TEST_CASE("qualified function pointer return type", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_void_v<
            stdx::return_t<std::add_pointer_t<decltype(func_no_args)> const>>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<
                       std::add_pointer_t<decltype(func_no_args)> volatile>>);
    STATIC_REQUIRE(
        std::is_void_v<stdx::return_t<
            std::add_pointer_t<decltype(func_no_args)> const volatile>>);
}

TEST_CASE("lambda return type", "[function_traits]") {
    [[maybe_unused]] auto const x = []() {};
    [[maybe_unused]] auto const y = []() mutable {};

    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(x)>>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(y)>>);
}

TEST_CASE("function args", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(func_no_args)>::template args<std::tuple>,
                       std::tuple<>>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::args_t<decltype(func_no_args), std::tuple>,
                       std::tuple<>>);

    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(func_one_arg)>::template args<std::tuple>,
                       std::tuple<int>>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::args_t<decltype(func_one_arg), std::tuple>,
                       std::tuple<int>>);
}

TEST_CASE("function decayed args", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<decltype(func_ref_arg)>::
                           template decayed_args<std::tuple>,
                       std::tuple<int>>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::decayed_args_t<decltype(func_ref_arg), std::tuple>,
                       std::tuple<int>>);
}

TEST_CASE("function nth arg", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(func_one_arg)>::template nth_arg<0>,
                       int>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::nth_arg_t<decltype(func_one_arg), 0>, int>);
}

TEST_CASE("function decayed nth arg", "[function_traits]") {
    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(func_ref_arg)>::template decayed_nth_arg<0>,
                       int>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::decayed_nth_arg_t<decltype(func_ref_arg), 0>,
                       int>);
}

TEST_CASE("lambda args", "[function_traits]") {
    [[maybe_unused]] auto const x = [](int) {};
    [[maybe_unused]] auto const y = [](int) mutable {};

    STATIC_REQUIRE(std::is_same_v<typename stdx::function_traits<
                                      decltype(x)>::template args<std::tuple>,
                                  std::tuple<int>>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::args_t<decltype(x), std::tuple>, std::tuple<int>>);

    STATIC_REQUIRE(std::is_same_v<typename stdx::function_traits<
                                      decltype(y)>::template args<std::tuple>,
                                  std::tuple<int>>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::args_t<decltype(y), std::tuple>, std::tuple<int>>);
}

TEST_CASE("lambda decayed args", "[function_traits]") {
    [[maybe_unused]] auto const x = [](int &) {};
    [[maybe_unused]] auto const y = [](int &) mutable {};

    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(x)>::template decayed_args<std::tuple>,
                       std::tuple<int>>);
    STATIC_REQUIRE(std::is_same_v<stdx::decayed_args_t<decltype(x), std::tuple>,
                                  std::tuple<int>>);

    STATIC_REQUIRE(
        std::is_same_v<typename stdx::function_traits<
                           decltype(y)>::template decayed_args<std::tuple>,
                       std::tuple<int>>);
    STATIC_REQUIRE(std::is_same_v<stdx::decayed_args_t<decltype(y), std::tuple>,
                                  std::tuple<int>>);
}

TEST_CASE("lambda nth arg", "[function_traits]") {
    [[maybe_unused]] auto const x = [](int) {};

    STATIC_REQUIRE(
        std::is_same_v<
            typename stdx::function_traits<decltype(x)>::template nth_arg<0>,
            int>);
    STATIC_REQUIRE(std::is_same_v<stdx::nth_arg_t<decltype(x), 0>, int>);
}

TEST_CASE("lambda decayed nth arg", "[function_traits]") {
    [[maybe_unused]] auto const x = [](int &) {};

    STATIC_REQUIRE(std::is_same_v<typename stdx::function_traits<
                                      decltype(x)>::template decayed_nth_arg<0>,
                                  int>);
    STATIC_REQUIRE(
        std::is_same_v<stdx::decayed_nth_arg_t<decltype(x), 0>, int>);
}

TEST_CASE("function arity", "[function_traits]") {
    STATIC_REQUIRE(
        stdx::function_traits<decltype(func_no_args)>::arity::value == 0u);
    STATIC_REQUIRE(
        stdx::function_traits<decltype(func_one_arg)>::arity::value == 1u);
    STATIC_REQUIRE(stdx::arity_t<decltype(func_no_args)>::value == 0u);
    STATIC_REQUIRE(stdx::arity_t<decltype(func_one_arg)>::value == 1u);

    STATIC_REQUIRE(stdx::arity_v<decltype(func_no_args)> == 0u);
    STATIC_REQUIRE(stdx::arity_v<decltype(func_one_arg)> == 1u);
}

TEST_CASE("lambda arity", "[function_traits]") {
    [[maybe_unused]] auto const x = []() {};
    [[maybe_unused]] auto const y = [](int) {};

    STATIC_REQUIRE(stdx::function_traits<decltype(x)>::arity::value == 0u);
    STATIC_REQUIRE(stdx::function_traits<decltype(y)>::arity::value == 1u);
    STATIC_REQUIRE(stdx::arity_t<decltype(x)>::value == 0u);
    STATIC_REQUIRE(stdx::arity_t<decltype(y)>::value == 1u);

    STATIC_REQUIRE(stdx::arity_v<decltype(x)> == 0u);
    STATIC_REQUIRE(stdx::arity_v<decltype(y)> == 1u);
}

TEST_CASE("generic lambda arity", "[function_traits]") {
    [[maybe_unused]] auto const x = [](auto) {};
    [[maybe_unused]] auto const y = [](auto, auto) {};
    STATIC_REQUIRE(stdx::arity_t<decltype(x)>::value == 1u);
    STATIC_REQUIRE(stdx::arity_t<decltype(y)>::value == 2u);

    STATIC_REQUIRE(stdx::arity_v<decltype(x)> == 1u);
    STATIC_REQUIRE(stdx::arity_v<decltype(y)> == 2u);
}

namespace {
int called_1{};
int called_2{};

template <typename F>
constexpr auto call_f(F f, stdx::priority_t<1>) -> stdx::return_t<F> {
    ++called_1;
    return f();
}

template <typename F> constexpr auto call_f(F f, stdx::priority_t<0>) -> void {
    ++called_2;
    f(0);
}
} // namespace

TEST_CASE("SFINAE friendly", "[function_traits]") {
    called_1 = 0;
    called_2 = 0;
    auto f1 = []() -> int { return 1; };
    auto f2 = [](auto) -> void {};
    call_f(f1, stdx::priority<1>);
    CHECK(called_1 == 1);
    CHECK(called_2 == 0);
    call_f(f2, stdx::priority<1>);
    CHECK(called_2 == 1);
}

namespace {
struct S {
    [[maybe_unused]] auto f() -> void {}
};
struct S_C {
    [[maybe_unused]] auto f() const -> void {}
};
struct S_V {
    [[maybe_unused]] auto f() volatile -> void {}
};
struct S_CV {
    [[maybe_unused]] auto f() const volatile -> void {}
};

struct S_LV {
    [[maybe_unused]] auto f() & -> void {}
};
struct S_RV {
    [[maybe_unused]] auto f() && -> void {}
};
struct S_CLV {
    [[maybe_unused]] auto f() const & -> void {}
};
struct S_CRV {
    [[maybe_unused]] auto f() const && -> void {}
};

struct S_VLV {
    [[maybe_unused]] auto f() volatile & -> void {}
};
struct S_VRV {
    [[maybe_unused]] auto f() volatile && -> void {}
};

struct S_CVLV {
    [[maybe_unused]] auto f() const volatile & -> void {}
};
struct S_CVRV {
    [[maybe_unused]] auto f() const volatile && -> void {}
};
} // namespace

TEST_CASE("member function return type", "[function_traits]") {
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(&S::f)>>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(&S_C::f)>>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(&S_V::f)>>);
    STATIC_REQUIRE(std::is_void_v<stdx::return_t<decltype(&S_CV::f)>>);
}

TEST_CASE("member function arity", "[function_traits]") {
    STATIC_REQUIRE(stdx::arity_v<decltype(&S::f)> == 0);
    STATIC_REQUIRE(stdx::arity_v<decltype(&S_C::f)> == 0);
    STATIC_REQUIRE(stdx::arity_v<decltype(&S_V::f)> == 0);
    STATIC_REQUIRE(stdx::arity_v<decltype(&S_CV::f)> == 0);
}

TEST_CASE("object argument types (const/volatile)", "[function_traits]") {
    STATIC_REQUIRE(std::is_void_v<stdx::obj_arg_t<decltype(&func_no_args)>>);
    STATIC_REQUIRE(std::is_same_v<S, stdx::obj_arg_t<decltype(&S::f)>>);
    STATIC_REQUIRE(
        std::is_same_v<S_C const, stdx::obj_arg_t<decltype(&S_C::f)>>);
    STATIC_REQUIRE(
        std::is_same_v<S_V volatile, stdx::obj_arg_t<decltype(&S_V::f)>>);
    STATIC_REQUIRE(std::is_same_v<S_CV const volatile,
                                  stdx::obj_arg_t<decltype(&S_CV::f)>>);
}

TEST_CASE("object argument types (value categories)", "[function_traits]") {
    STATIC_REQUIRE(std::is_same_v<S_LV &, stdx::obj_arg_t<decltype(&S_LV::f)>>);
    STATIC_REQUIRE(
        std::is_same_v<S_RV &&, stdx::obj_arg_t<decltype(&S_RV::f)>>);
    STATIC_REQUIRE(
        std::is_same_v<S_CLV const &, stdx::obj_arg_t<decltype(&S_CLV::f)>>);
    STATIC_REQUIRE(
        std::is_same_v<S_CRV const &&, stdx::obj_arg_t<decltype(&S_CRV::f)>>);

    STATIC_REQUIRE(
        std::is_same_v<S_VLV volatile &, stdx::obj_arg_t<decltype(&S_VLV::f)>>);
    STATIC_REQUIRE(std::is_same_v<S_VRV volatile &&,
                                  stdx::obj_arg_t<decltype(&S_VRV::f)>>);
    STATIC_REQUIRE(std::is_same_v<S_CVLV const volatile &,
                                  stdx::obj_arg_t<decltype(&S_CVLV::f)>>);
    STATIC_REQUIRE(std::is_same_v<S_CVRV const volatile &&,
                                  stdx::obj_arg_t<decltype(&S_CVRV::f)>>);
}

TEST_CASE("object argument type (lambda)", "[function_traits]") {
    [[maybe_unused]] auto const x = [](int) {};
    STATIC_REQUIRE(std::is_same_v<decltype(x), stdx::obj_arg_t<decltype(x)>>);
}

namespace {
struct TS {
    template <typename T> auto operator()(T) -> void {}
};
struct TS_C {
    template <typename T> auto operator()(T) const -> void {}
};
struct TS_V {
    template <typename T> auto operator()(T) volatile -> void {}
};
struct TS_CV {
    template <typename T> auto operator()(T) const volatile -> void {}
};

struct TS_LV {
    template <typename T> auto operator()(T) & -> void {}
};
struct TS_RV {
    template <typename T> auto operator()(T) && -> void {}
};
struct TS_CLV {
    template <typename T> auto operator()(T) const & -> void {}
};
struct TS_CRV {
    template <typename T> auto operator()(T) const && -> void {}
};

struct TS_VLV {
    template <typename T> auto operator()(T) volatile & -> void {}
};
struct TS_VRV {
    template <typename T> auto operator()(T) volatile && -> void {}
};

struct TS_CVLV {
    template <typename T> auto operator()(T) const volatile & -> void {}
};
struct TS_CVRV {
    template <typename T> auto operator()(T) const volatile && -> void {}
};
} // namespace

TEST_CASE("member function template arity", "[function_traits]") {
    STATIC_REQUIRE(stdx::arity_v<TS> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_C> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_V> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_CV> == 1);

    STATIC_REQUIRE(stdx::arity_v<TS_LV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_RV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_CLV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_CRV> == 1);

    STATIC_REQUIRE(stdx::arity_v<TS_VLV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_VRV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_CVLV> == 1);
    STATIC_REQUIRE(stdx::arity_v<TS_CVRV> == 1);
}

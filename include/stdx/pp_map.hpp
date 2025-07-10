#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define STDX_EVAL0(...) __VA_ARGS__
#define STDX_EVAL1(...) STDX_EVAL0(STDX_EVAL0(STDX_EVAL0(__VA_ARGS__)))
#define STDX_EVAL2(...) STDX_EVAL1(STDX_EVAL1(STDX_EVAL1(__VA_ARGS__)))
#define STDX_EVAL3(...) STDX_EVAL2(STDX_EVAL2(STDX_EVAL2(__VA_ARGS__)))
#define STDX_EVAL4(...) STDX_EVAL3(STDX_EVAL3(STDX_EVAL3(__VA_ARGS__)))
#define STDX_EVAL5(...) STDX_EVAL4(STDX_EVAL4(STDX_EVAL4(__VA_ARGS__)))
#define STDX_EVAL(...) STDX_EVAL5(__VA_ARGS__)

#define STDX_MAP_END(...)
#define STDX_MAP_OUT

#define STDX_EMPTY()
#define STDX_DEFER(id) id STDX_EMPTY()

#define STDX_MAP_GET_END2() 0, STDX_MAP_END
#define STDX_MAP_GET_END1(...) STDX_MAP_GET_END2
#define STDX_MAP_GET_END(...) STDX_MAP_GET_END1
#define STDX_MAP_NEXT0(test, next, ...) next STDX_MAP_OUT
#define STDX_MAP_NEXT1(test, next) STDX_DEFER(STDX_MAP_NEXT0)(test, next, 0)
#define STDX_MAP_NEXT(test, next) STDX_MAP_NEXT1(STDX_MAP_GET_END test, next)
#define STDX_MAP_INC(X) STDX_MAP_INC_##X

#define STDX_MAP_A(f, x, peek, ...)                                            \
    , f(x) STDX_DEFER(STDX_MAP_NEXT(peek, STDX_MAP_B))(f, peek, __VA_ARGS__)
#define STDX_MAP_B(f, x, peek, ...)                                            \
    , f(x) STDX_DEFER(STDX_MAP_NEXT(peek, STDX_MAP_A))(f, peek, __VA_ARGS__)

#define STDX_DROP0(X, ...) __VA_ARGS__
#define STDX_DROP1(...) STDX_DROP0(__VA_ARGS__)

#define STDX_MAP(f, ...)                                                       \
    __VA_OPT__(STDX_DROP1(                                                     \
        0 STDX_EVAL(STDX_MAP_A(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))))

// NOLINTEND(cppcoreguidelines-macro-usage)

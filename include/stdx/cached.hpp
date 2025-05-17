#pragma once

#include <stdx/compiler.hpp>
#include <stdx/latched.hpp>
#include <stdx/type_traits.hpp>

namespace stdx {
inline namespace v1 {
template <typename F> struct cached : latched<F> {
    using latched<F>::latched;

    auto reset() { this->opt.reset(); }
    auto refresh() LIFETIMEBOUND -> typename latched<F>::value_type & {
        this->opt.reset();
        this->populate();
        return *this->opt;
    }
};

template <typename F> cached(F) -> cached<remove_cvref_t<F>>;

template <typename C> using cached_value_t = latched_value_t<C>;
} // namespace v1
} // namespace stdx

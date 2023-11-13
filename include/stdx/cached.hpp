#pragma once

#include <stdx/functional.hpp>
#include <stdx/type_traits.hpp>

#include <optional>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
template <typename F> class cached : with_result_of<F> {
    using lazy_t = with_result_of<F>;

  public:
    using value_type = stdx::remove_cvref_t<std::invoke_result_t<F>>;

    constexpr explicit cached(F const &f) : lazy_t{f} {}
    constexpr explicit cached(F &&f) : lazy_t{std::move(f)} {}

    auto has_value() const noexcept -> bool { return opt.has_value(); }
    explicit operator bool() const noexcept { return opt.has_value(); }
    explicit operator bool() noexcept { return opt.has_value(); }

    auto value() & -> value_type & {
        populate();
        return *opt;
    }
    auto value() const & -> value_type const & {
        populate();
        return *opt;
    }
    auto value() && -> value_type && {
        populate();
        return *std::move(opt);
    }
    auto value() const && -> value_type const && {
        populate();
        return *std::move(opt);
    }

    auto operator->() const -> value_type const * {
        populate();
        return opt.operator->();
    }
    auto operator->() -> value_type * {
        populate();
        return opt.operator->();
    }

    auto operator*() const & -> decltype(auto) { return value(); }
    auto operator*() & -> decltype(auto) { return value(); }
    auto operator*() const && -> decltype(auto) {
        return std::move(*this).value();
    }
    auto operator*() && -> decltype(auto) { return std::move(*this).value(); }

    auto reset() { opt.reset(); }
    auto refresh() {
        opt.reset();
        populate();
    }

  private:
    auto populate() const {
        if (not opt.has_value()) {
            opt.emplace(static_cast<lazy_t>(*this));
        }
    }

    mutable std::optional<value_type> opt{};
};

template <typename C>
using cached_value_t = typename stdx::remove_cvref_t<C>::value_type;
} // namespace v1
} // namespace stdx

#pragma once

#if __cplusplus >= 202002L

#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/ct_conversions.hpp>
#include <stdx/ct_string.hpp>
#include <stdx/tuple.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/utility.hpp>

#include <fmt/compile.h>
#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <string_view>
#include <utility>

template <std::size_t N>
struct fmt::formatter<stdx::ct_string<N>> : fmt::formatter<std::string_view> {
    template <typename Ctx>
    constexpr auto format(stdx::ct_string<N> const &s, Ctx &ctx) const {
        return fmt::formatter<std::string_view>::format(std::string_view{s},
                                                        ctx);
    }
};

namespace stdx {
inline namespace v1 {
template <typename Str, typename Args> struct format_result {
    [[no_unique_address]] Str str;
    [[no_unique_address]] Args args;

  private:
    friend constexpr auto operator==(format_result const &,
                                     format_result const &) -> bool = default;
};
template <typename Str, typename Args>
format_result(Str, Args) -> format_result<Str, Args>;

namespace detail {
template <typename It> CONSTEVAL auto find_spec(It first, It last) -> It {
    for (auto spec_start = std::find(first, last, '{'); spec_start != last;
         spec_start = std::find(spec_start, last, '{')) {
        if (spec_start + 1 != last) {
            if (*std::next(spec_start) != '{') {
                return spec_start;
            }
            ++spec_start;
        }
        ++spec_start;
    }
    return last;
}

CONSTEVAL auto count_specifiers(std::string_view fmt) -> std::size_t {
    auto count = std::size_t{};
    for (auto spec_start = find_spec(fmt.begin(), fmt.end());
         spec_start != fmt.end();
         spec_start = find_spec(++spec_start, fmt.end())) {
        ++count;
    }
    return count;
}

template <std::size_t N> CONSTEVAL auto split_specifiers(std::string_view fmt) {
    auto splits = std::array<std::string_view, N>{};
    auto count = std::size_t{};

    auto split_start = fmt.begin();
    auto spec_start = find_spec(fmt.begin(), fmt.end());
    while (spec_start != fmt.end()) {
        auto split_end = std::find(spec_start, fmt.end(), '}');
        if (split_end != fmt.end()) {
            ++split_end;
        }
        splits[count++] = std::string_view{split_start, split_end};
        split_start = split_end;
        spec_start = find_spec(split_start, fmt.end());
    }
    splits[count++] = std::string_view{split_start, spec_start};

    return splits;
}

template <typename T>
concept cx_value = requires { typename T::cx_value_t; } or
                   requires(T t) { ct_string_from_type(t); };

CONSTEVAL auto arg_value(auto a) { return a; }

template <typename T> CONSTEVAL auto arg_value(type_identity<T>) {
    return type_as_string<T>();
}

CONSTEVAL auto arg_value(cx_value auto a) {
    if constexpr (requires { ct_string_from_type(a); }) {
        return ct_string_from_type(a);
    } else if constexpr (std::is_enum_v<decltype(a())>) {
        return enum_as_string<a()>();
    } else {
        return arg_value(a());
    }
}

template <typename T, typename U, typename S>
constexpr auto operator+(format_result<T, U> r, S s) {
    return format_result{r.str + s, r.args};
}

template <typename S, typename T, typename U>
constexpr auto operator+(S s, format_result<T, U> r) {
    return format_result{s + r.str, r.args};
}

template <typename A, typename B, typename T, typename U>
constexpr auto operator+(format_result<A, B> r1, format_result<T, U> r2) {
    return format_result{r1.str + r2.str, tuple_cat(r1.args, r2.args)};
}

template <typename T, T...> struct null_output;

template <std::size_t Sz> CONSTEVAL auto to_ct_string(std::string_view s) {
    return ct_string<Sz + 1>{s.data(), s.size()};
}

CONSTEVAL auto convert_input(auto s) {
    if constexpr (requires { ct_string_from_type(s); }) {
        return ct_string_from_type(s);
    } else {
        return s;
    }
}

template <ct_string S,
          template <typename T, T...> typename Output = detail::null_output>
CONSTEVAL auto convert_output() {
    if constexpr (same_as<Output<char>, null_output<char>>) {
        return S;
    } else {
        return ct_string_to_type<S, Output>();
    }
}

template <ct_string Fmt,
          template <typename T, T...> typename Output = detail::null_output,
          typename Arg>
constexpr auto format1(Arg arg) {
    if constexpr (cx_value<Arg>) {
        constexpr auto result = [&] {
            constexpr auto fmtstr = FMT_COMPILE(std::string_view{Fmt});
            constexpr auto a = arg_value(arg);
            if constexpr (is_specialization_of_v<std::remove_cv_t<decltype(a)>,
                                                 format_result>) {
                constexpr auto s = convert_input(a.str);
                constexpr auto sz = fmt::formatted_size(fmtstr, s);
                ct_string<sz + 1> cts{};
                fmt::format_to(cts.begin(), fmtstr, s);
                return format_result{cts, a.args};
            } else {
                constexpr auto sz = fmt::formatted_size(fmtstr, a);
                ct_string<sz + 1> cts{};
                fmt::format_to(cts.begin(), fmtstr, a);
                return cts;
            }
        }();
        if constexpr (is_specialization_of_v<std::remove_cv_t<decltype(result)>,
                                             format_result>) {
            return format_result{convert_output<result.str, Output>(),
                                 result.args};
        } else {
            return convert_output<result, Output>();
        }
    } else {
        return format_result{convert_output<Fmt, Output>(), tuple{arg}};
    }
}

template <template <typename T, T...> typename Output>
concept ct_format_compatible = requires {
    {
        Output<char, 'A'>{} + Output<char, 'B'>{}
    } -> same_as<Output<char, 'A', 'B'>>;
};

template <ct_string Fmt> struct fmt_data {
    constexpr static auto fmt = std::string_view{Fmt};
    constexpr static auto N = detail::count_specifiers(fmt);
    constexpr static auto splits = detail::split_specifiers<N + 1>(fmt);
    constexpr static auto last_cts =
        detail::to_ct_string<splits[N].size()>(splits[N]);
};
} // namespace detail

template <ct_string Fmt,
          template <typename T, T...> typename Output = detail::null_output>
constexpr auto ct_format = [](auto &&...args) {
    if constexpr (not same_as<Output<char>, detail::null_output<char>>) {
        static_assert(detail::ct_format_compatible<Output>);
    }

    using data = detail::fmt_data<Fmt>;

    [[maybe_unused]] auto const format1 = [&]<std::size_t I>(auto &&arg) {
        constexpr auto cts =
            detail::to_ct_string<data::splits[I].size()>(data::splits[I]);
        return detail::format1<cts, Output>(FWD(arg));
    };

    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return (format1.template operator()<Is>(FWD(args)) + ... +
                detail::convert_output<data::last_cts, Output>());
    }(std::make_index_sequence<data::N>{});
};
} // namespace v1
} // namespace stdx

#endif

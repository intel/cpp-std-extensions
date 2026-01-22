#pragma once

#if __cplusplus >= 202002L

#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/ct_conversions.hpp>
#include <stdx/ct_string.hpp>
#include <stdx/detail/fmt.hpp>
#include <stdx/pp_map.hpp>
#include <stdx/tuple.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/utility.hpp>

#include <boost/mp11/algorithm.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <string_view>
#include <utility>

namespace stdx {
inline namespace v1 {
template <std::size_t N> constexpr auto format_as(stdx::ct_string<N> const &s) {
    return std::string_view{s};
}

template <std::size_t Begin, std::size_t End> struct format_span {
    constexpr static auto begin = Begin;
    constexpr static auto end = End;

  private:
    friend constexpr auto operator==(format_span const &, format_span const &)
        -> bool = default;
};

namespace detail {
template <std::size_t Offset> struct apply_span_offset_q {
    template <typename Span>
    using fn = format_span<Offset + Span::begin, Offset + Span::end>;
};

template <std::size_t Offset, typename L>
using apply_offset =
    boost::mp11::mp_transform_q<apply_span_offset_q<Offset>, L>;
} // namespace detail

template <typename T = void> struct ct_format_arg {
    using type_t = T;
    friend constexpr auto operator==(ct_format_arg const &,
                                     ct_format_arg const &) -> bool = default;
};

template <typename Arg>
using is_compile_time_arg =
    std::bool_constant<is_specialization_of<Arg, ct_format_arg>()>;

template <typename Str, typename Args, typename Spans> struct format_result {
    static_assert(Args::size() == boost::mp11::mp_size<Spans>::value);

    CONSTEVAL static auto ct_string_convertible()
        -> std::bool_constant<Args::size() == 0>;

    [[no_unique_address]] Str str;
    [[no_unique_address]] Args args{};
    using spans_t = Spans;

    friend constexpr auto operator+(format_result const &fr)
        requires(decltype(ct_string_convertible())::value)
    {
        return +fr.str;
    }

    friend constexpr auto operator+(format_result const &) {
        static_assert(always_false_v<format_result>,
                      "Unary operator+ can only be used on a format_result "
                      "without any runtime arguments");
    }

  private:
    friend constexpr auto operator==(format_result const &,
                                     format_result const &) -> bool = default;
};

template <typename Str, typename Args, typename Spans>
    requires(boost::mp11::mp_all_of<Args, is_compile_time_arg>::value and
             is_cx_value_v<Str>)
struct format_result<Str, Args, Spans> {
    static_assert(Args::size() == boost::mp11::mp_size<Spans>::value);

    CONSTEVAL static auto ct_string_convertible() -> std::true_type;

    [[no_unique_address]] Str str;
    [[no_unique_address]] Args args{};
    using spans_t = Spans;

    friend constexpr auto operator+(format_result const &fr) { return +fr.str; }

    constexpr auto operator()() const noexcept { return +(*this); }
    using cx_value_t [[maybe_unused]] = void;

  private:
    friend constexpr auto operator==(format_result const &,
                                     format_result const &) -> bool = default;
};

template <typename Spans = type_list<>, typename Str, typename Args = tuple<>>
constexpr auto make_format_result(Str s, Args args = {}) {
    return format_result<Str, Args, Spans>{s, std::move(args)};
}

inline namespace literals {
inline namespace ct_string_literals {
template <ct_string S> CONSTEVAL_UDL auto operator""_fmt_res() {
    return make_format_result(cts_t<S>{});
}
} // namespace ct_string_literals
} // namespace literals

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

struct split_spec {
    std::string_view view;
    std::size_t start{};

  private:
    friend constexpr auto operator==(split_spec const &, split_spec const &)
        -> bool = default;
};

template <std::size_t N>
CONSTEVAL auto split_specifiers(std::string_view fmt)
    -> std::array<split_spec, N> {
    auto splits = std::array<split_spec, N>{};
    auto count = std::size_t{};

    auto split_start = fmt.begin();
    auto spec_start = find_spec(fmt.begin(), fmt.end());
    while (spec_start != fmt.end()) {
        auto split_end = std::find(spec_start, fmt.end(), '}');
        if (split_end != fmt.end()) {
            ++split_end;
        }
        splits[count++] = {
            std::string_view{split_start, split_end},
            static_cast<std::size_t>(std::distance(split_start, spec_start))};
        split_start = split_end;
        spec_start = find_spec(split_start, fmt.end());
    }
    splits[count++] = {
        std::string_view{split_start, spec_start},
        static_cast<std::size_t>(std::distance(split_start, spec_start))};

    return splits;
}

template <typename T>
concept fmt_cx_value =
    is_cx_value_v<T> or requires(T t) { ct_string_from_type(t); };

template <typename T, T V>
CONSTEVAL auto arg_value(std::integral_constant<T, V>) {
    if constexpr (std::is_enum_v<T>) {
        return enum_as_string<V>();
    } else {
        return V;
    }
}

template <typename T> CONSTEVAL auto arg_value(type_identity<T>) {
    return type_as_string<T>();
}

template <ct_string S> CONSTEVAL auto arg_value(cts_t<S>) { return S; }

CONSTEVAL auto arg_value(fmt_cx_value auto a) {
    if constexpr (is_specialization_of_v<decltype(a), format_result>) {
        return a;
    } else if constexpr (requires { arg_value(a()); }) {
        return arg_value(a());
    } else if constexpr (requires { ct_string_from_type(a); }) {
        return ct_string_from_type(a);
    } else if constexpr (std::is_enum_v<decltype(a())>) {
        return enum_as_string<a()>();
    } else {
        return a();
    }
}

template <typename T> CONSTEVAL auto arg_type(T) -> T;

template <typename T, T V>
CONSTEVAL auto arg_type(std::integral_constant<T, V>) -> T;

CONSTEVAL auto arg_type(fmt_cx_value auto a) {
    if constexpr (requires { ct_string_from_type(a); }) {
        return ct_string_from_type(a);
    } else {
        return a();
    }
}

template <typename Str, typename Args, typename Spans, typename S>
constexpr auto operator+(format_result<Str, Args, Spans> r, S s) {
    return make_format_result<Spans>(r.str + s, std::move(r.args));
}

template <typename S, typename Str, typename Args, typename Spans>
constexpr auto operator+(S s, format_result<Str, Args, Spans> r) {
    return make_format_result<detail::apply_offset<s.size(), Spans>>(
        s + r.str, std::move(r.args));
}

template <typename Str1, typename Args1, typename Spans1, typename Str2,
          typename Args2, typename Spans2>
constexpr auto operator+(format_result<Str1, Args1, Spans1> r1,
                         format_result<Str2, Args2, Spans2> r2) {
    return make_format_result<boost::mp11::mp_append<
        Spans1, detail::apply_offset<r1.str.size(), Spans2>>>(
        r1.str + r2.str,
        stdx::tuple_cat(std::move(r1.args), std::move(r2.args)));
}

template <typename T, T...> struct null_output;

template <std::size_t Sz> CONSTEVAL auto to_ct_string(std::string_view s) {
    return ct_string<Sz + 1>{s.data(), s.size()};
}

CONSTEVAL auto convert_input(auto s) {
    if constexpr (requires { ct_string_from_type(s); }) {
        return ct_string_from_type(s);
    } else {
        return s.value;
    }
}

template <ct_string S,
          template <typename T, T...> typename Output = detail::null_output>
CONSTEVAL auto convert_output() {
    if constexpr (same_as<Output<char>, null_output<char>>) {
        return cts_t<S>{};
    } else {
        return ct_string_to_type<S, Output>();
    }
}

template <std::size_t N>
CONSTEVAL auto perform_format(auto s, auto const &v) -> ct_string<N + 1> {
    ct_string<N + 1> cts{};
    fmt::format_to(cts.begin(), s, v);
    return cts;
}

template <ct_string Fmt, std::size_t Start, typename Arg>
constexpr auto format1(Arg arg) {
    if constexpr (requires { arg_value(arg); }) {
        constexpr auto fmtstr = STDX_FMT_COMPILE(Fmt);
        constexpr auto a = arg_value(arg);
        if constexpr (is_specialization_of_v<std::remove_cv_t<decltype(a)>,
                                             format_result>) {
            constexpr auto s = convert_input(a.str);
            constexpr auto sz = fmt::formatted_size(fmtstr, s);
            constexpr auto cts = perform_format<sz>(fmtstr, s);
            using Spans = typename std::remove_cvref_t<decltype(a)>::spans_t;
            return make_format_result<detail::apply_offset<Start, Spans>>(
                cts_t<cts>{}, a.args);
        } else {
            using arg_t = stdx::remove_cvref_t<decltype(arg_type(arg))>;
            constexpr auto sz = fmt::formatted_size(fmtstr, a);
            constexpr auto cts = perform_format<sz>(fmtstr, a);
            using Spans = type_list<format_span<Start, sz>>;
            return make_format_result<Spans>(cts_t<cts>{},
                                             tuple{ct_format_arg<arg_t>{}});
        }
    } else if constexpr (is_specialization_of_v<Arg, format_result>) {
        auto const sub_result = format1<Fmt, Start>(arg.str);
        using Spans = typename Arg::spans_t;
        return make_format_result<detail::apply_offset<Start, Spans>>(
            sub_result.str, std::move(arg).args);
    } else {
        using Spans = type_list<format_span<Start, Fmt.size()>>;
        return make_format_result<Spans>(cts_t<Fmt>{}, tuple{std::move(arg)});
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
    constexpr static auto N = count_specifiers(fmt);
    constexpr static auto splits = split_specifiers<N + 1>(fmt);
    constexpr static auto last_cts =
        to_ct_string<splits[N].view.size()>(splits[N].view);
};

[[maybe_unused]] constexpr inline struct format_as_t {
    template <typename T>
        requires true
    constexpr auto operator()(T &&t) const
        noexcept(noexcept(ct_format_as(std::forward<T>(t))))
            -> decltype(ct_format_as(std::forward<T>(t))) {
        return ct_format_as(std::forward<T>(t));
    }

    template <typename T>
    constexpr auto operator()(T &&t) const -> decltype(auto) {
        return T(std::forward<T>(t));
    }
} format_as;
} // namespace detail

template <ct_string Fmt,
          template <typename T, T...> typename Output = detail::null_output>
constexpr auto ct_format = [](auto &&...args) {
    if constexpr (not same_as<Output<char>, detail::null_output<char>>) {
        static_assert(detail::ct_format_compatible<Output>);
    }

    using data = detail::fmt_data<Fmt>;

    static_assert(data::N == sizeof...(args),
                  "Format string has a mismatch between the number of format "
                  "specifiers and arguments.");

    [[maybe_unused]] auto const format1 = []<std::size_t I>(auto &&arg) {
        constexpr auto cts = detail::to_ct_string<data::splits[I].view.size()>(
            data::splits[I].view);
        return detail::format1<cts, data::splits[I].start>(FWD(arg));
    };

    auto result = [&]<std::size_t... Is>(std::index_sequence<Is...>,
                                         auto &&...as) {
        return (format1.template operator()<Is>(detail::format_as(FWD(as))) +
                ... + make_format_result(cts_t<data::last_cts>{}));
    }(std::make_index_sequence<data::N>{}, FWD(args)...);
    constexpr auto str = detail::convert_output<result.str.value, Output>();
    using Spans = typename std::remove_cvref_t<decltype(result)>::spans_t;
    return make_format_result<Spans>(str, std::move(result).args);
};

template <ct_string Fmt>
constexpr auto num_fmt_specifiers =
    detail::count_specifiers(std::string_view{Fmt});
} // namespace v1
} // namespace stdx

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define STDX_CT_FORMAT(S, ...)                                                 \
    stdx::ct_format<S>(STDX_MAP(CX_WRAP __VA_OPT__(, ) __VA_ARGS__))

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

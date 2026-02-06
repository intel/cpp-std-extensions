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

template <stdx::ct_string Name, typename T, int Begin, int End = Begin - 1>
struct named_arg {
    using type = T;
    constexpr static auto name = Name;
    constexpr static auto begin = Begin;
    constexpr static auto end = End;

    constexpr static std::integral_constant<bool, (End < Begin)> is_runtime{};

    template <int StringOffset, int ArgOffset>
    CONSTEVAL static auto apply_offset() {
        if constexpr (is_runtime) {
            return named_arg<Name, T, Begin + ArgOffset, End + ArgOffset>{};
        } else {
            return named_arg<Name, T, Begin + StringOffset,
                             End + StringOffset>{};
        }
    }

  private:
    friend constexpr auto operator==(named_arg const &, named_arg const &)
        -> bool = default;
};

namespace detail {
template <std::size_t StringOffset, std::size_t ArgOffset>
struct apply_span_offset_q {
    template <typename Arg>
    using fn = decltype(Arg::template apply_offset<StringOffset, ArgOffset>());
};

template <std::size_t StringOffset, std::size_t ArgOffset, typename L>
using apply_offset =
    boost::mp11::mp_transform_q<apply_span_offset_q<StringOffset, ArgOffset>,
                                L>;
} // namespace detail

template <typename Str, typename Args, typename NamedArgs>
struct format_result {
    CONSTEVAL static auto ct_string_convertible()
        -> std::bool_constant<Args::size() == 0>;

    [[no_unique_address]] Str str;
    [[no_unique_address]] Args args{};
    using named_args_t = NamedArgs;

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

template <typename Str, typename Args, typename NamedArgs>
    requires(Args::size() == 0 and is_cx_value_v<Str>)
struct format_result<Str, Args, NamedArgs> {
    CONSTEVAL static auto ct_string_convertible() -> std::true_type;

    [[no_unique_address]] Str str;
    [[no_unique_address]] Args args{};
    using named_args_t = NamedArgs;

    friend constexpr auto operator+(format_result const &fr) { return +fr.str; }

    constexpr auto operator()() const noexcept { return +(*this); }
    using cx_value_t [[maybe_unused]] = void;

  private:
    friend constexpr auto operator==(format_result const &,
                                     format_result const &) -> bool = default;
};

template <typename NamedArgs = type_list<>, typename Str,
          typename Args = tuple<>>
constexpr auto make_format_result(Str s, Args args = {}) {
    return format_result<Str, Args, NamedArgs>{s, std::move(args)};
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

template <ct_string S, std::size_t Start> CONSTEVAL auto extract_format1_str() {
    constexpr auto name_start = Start + 1;
    constexpr auto it = [] {
        for (auto i = S.value.cbegin() + name_start; i != S.value.cend(); ++i) {
            if (*i == ':') {
                return i;
            }
        }
        return S.value.cend();
    }();
    if constexpr (it == S.value.cend()) {
        if constexpr (Start == S.size() - 2) {
            // no name, empty fmt spec e.g. "abc{}"
            return std::pair{S, ct_string{""}};
        } else {
            // named arg, empty fmt spec, e.g. "abc{ghi}"
            constexpr auto suffix_start = S.size() - 1;
            constexpr auto prefix_size = name_start;
            constexpr auto name_size = suffix_start - name_start;
            constexpr auto suffix_size = 1;

            return std::pair{
                ct_string<prefix_size + 1U>{S.value.cbegin(), prefix_size} +
                    ct_string<suffix_size + 1U>{S.value.cbegin() + suffix_start,
                                                suffix_size},
                ct_string<name_size + 1U>{S.value.cbegin() + name_start,
                                          name_size}};
        }
    } else {
        // named arg, fmt spec, e.g. "abc{ghi:x}"
        constexpr auto suffix_start = it - S.value.cbegin();
        constexpr auto prefix_size = name_start;
        constexpr auto name_size = suffix_start - name_start;
        constexpr auto suffix_size = S.size() - suffix_start;

        return std::pair{
            ct_string<prefix_size + 1U>{S.value.cbegin(), prefix_size} +
                ct_string<suffix_size + 1U>{S.value.cbegin() + suffix_start,
                                            suffix_size},
            ct_string<name_size + 1U>{S.value.cbegin() + name_start,
                                      name_size}};
    }
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

template <typename Str, typename Args, typename NamedArgs, typename S>
constexpr auto operator+(format_result<Str, Args, NamedArgs> r, S s) {
    return make_format_result<NamedArgs>(r.str + s, std::move(r.args));
}

template <typename S, typename Str, typename Args, typename NamedArgs>
constexpr auto operator+(S s, format_result<Str, Args, NamedArgs> r) {
    constexpr auto sz = s.size();
    return make_format_result<detail::apply_offset<sz, 0, NamedArgs>>(
        s + r.str, std::move(r.args));
}

template <typename Str1, typename Args1, typename NamedArgs1, typename Str2,
          typename Args2, typename NamedArgs2>
constexpr auto operator+(format_result<Str1, Args1, NamedArgs1> r1,
                         format_result<Str2, Args2, NamedArgs2> r2) {
    constexpr auto sz = r1.str.size();
    using ShiftedNamedArgs2 =
        detail::apply_offset<sz, boost::mp11::mp_size<Args1>::value,
                             NamedArgs2>;

    return make_format_result<
        boost::mp11::mp_append<NamedArgs1, ShiftedNamedArgs2>>(
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

template <ct_string Fmt, ct_string Name, std::size_t Start, typename Arg>
constexpr auto format1(Arg arg) {
    if constexpr (requires { arg_value(arg); }) {
        constexpr auto fmtstr = STDX_FMT_COMPILE(Fmt);
        constexpr auto a = arg_value(arg);
        using a_t = std::remove_cv_t<decltype(a)>;
        if constexpr (is_specialization_of_v<a_t, format_result>) {
            constexpr auto s = convert_input(a.str);
            constexpr auto sz = fmt::formatted_size(fmtstr, s);
            constexpr auto cts = perform_format<sz>(fmtstr, s);
            using shifted_named_args_t =
                detail::apply_offset<Start, 0, typename a_t::named_args_t>;
            return make_format_result<shifted_named_args_t>(cts_t<cts>{},
                                                            a.args);
        } else {
            constexpr auto sz = fmt::formatted_size(fmtstr, a);
            constexpr auto cts = perform_format<sz>(fmtstr, a);
            if constexpr (not Name.empty()) {
                using name_info_t = named_arg<Name, a_t, Start, sz>;
                return make_format_result<type_list<name_info_t>>(cts_t<cts>{});
            } else {
                return make_format_result(cts_t<cts>{});
            }
        }
    } else if constexpr (is_specialization_of_v<Arg, format_result>) {
        auto const sub_result = format1<Fmt, "", Start>(arg.str);
        using shifted_named_args_t =
            detail::apply_offset<Start, 0, typename Arg::named_args_t>;
        return make_format_result<shifted_named_args_t>(sub_result.str,
                                                        std::move(arg).args);
    } else {
        if constexpr (not Name.empty()) {
            using name_info_t = named_arg<Name, Arg, 0>;
            return make_format_result<type_list<name_info_t>>(
                cts_t<Fmt>{}, tuple{std::move(arg)});
        } else {
            return make_format_result(cts_t<Fmt>{}, tuple{std::move(arg)});
        }
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
        constexpr auto fmt = detail::extract_format1_str<
            detail::to_ct_string<data::splits[I].view.size()>(
                data::splits[I].view),
            data::splits[I].start>();
        return detail::format1<fmt.first, fmt.second, data::splits[I].start>(
            FWD(arg));
    };

    auto result = [&]<std::size_t... Is>(std::index_sequence<Is...>,
                                         auto &&...as) {
        return (format1.template operator()<Is>(detail::format_as(FWD(as))) +
                ... + make_format_result(cts_t<data::last_cts>{}));
    }(std::make_index_sequence<data::N>{}, FWD(args)...);
    constexpr auto str = detail::convert_output<result.str.value, Output>();
    using NamedArgs =
        typename std::remove_cvref_t<decltype(result)>::named_args_t;
    return make_format_result<NamedArgs>(str, std::move(result).args);
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

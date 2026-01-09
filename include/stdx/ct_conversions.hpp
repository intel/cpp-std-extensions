#pragma once

#include <stdx/compiler.hpp>

#include <string_view>

namespace stdx {
inline namespace v1 {
template <typename...> constexpr bool always_false_v = false;

template <typename Tag>
CONSTEVAL static auto type_as_string() -> std::string_view {
#ifdef __clang__
    constexpr std::string_view function_name = __PRETTY_FUNCTION__;
    constexpr auto rhs = function_name.size() - 2;
#elif defined(__GNUC__) || defined(__GNUG__)
    constexpr std::string_view function_name = __PRETTY_FUNCTION__;
    constexpr auto rhs = function_name.size() - 51;
#else
    static_assert(always_false_v<Tag>,
                  "Unknown compiler, can't build type name.");
#endif

    constexpr auto lhs = function_name.rfind('=', rhs) + 2;
    return function_name.substr(lhs, rhs - lhs + 1);
}

template <typename T>
CONSTEVAL static auto template_base() -> std::string_view {
    constexpr auto t = stdx::type_as_string<T>();
    constexpr auto rhs = t.find('<');
    return t.substr(0, rhs);
}

template <auto Value>
CONSTEVAL static auto enum_as_string() -> std::basic_string_view<char> {
#ifdef __clang__
    constexpr std::string_view value_string = __PRETTY_FUNCTION__;
#elif defined(__GNUC__) || defined(__GNUG__)
    constexpr std::string_view value_string = __PRETTY_FUNCTION__;
#else
    static_assert(always_false_v<Tag>,
                  "Unknown compiler, can't build type name.");
#endif
    constexpr auto qual_str = [&]() -> std::string_view {
        constexpr auto rhs = value_string.size() - 2;
        if (auto const eq_pos = value_string.find_last_of('=');
            eq_pos != std::string_view::npos) {
            auto const lhs = eq_pos + 2;
            return value_string.substr(lhs, rhs - lhs + 1);
        }
        return value_string;
    }();

    constexpr auto cast_str = [&]() -> std::string_view {
        if (auto const close_paren_pos = qual_str.find_last_of(')');
            close_paren_pos != std::string_view::npos) {
            if (qual_str[close_paren_pos + 1] != ':') {
                return qual_str;
            }
        }

        if (auto const colon_pos = qual_str.find_last_of(':');
            colon_pos != std::string_view::npos) {
            return qual_str.substr(colon_pos + 1);
        }

        return qual_str;
    }();

    return cast_str;
}
} // namespace v1
} // namespace stdx

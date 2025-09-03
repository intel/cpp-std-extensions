#pragma once

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <cstddef>
#include <string_view>

namespace stdx {
inline namespace v1 {
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

template <auto Value>
CONSTEVAL static auto enum_as_string() -> std::basic_string_view<char> {
#ifdef __clang__
    constexpr std::string_view value_string = __PRETTY_FUNCTION__;
    constexpr auto rhs = value_string.size() - 2;
#elif defined(__GNUC__) || defined(__GNUG__)
    constexpr std::string_view value_string = __PRETTY_FUNCTION__;
    constexpr auto rhs = value_string.size() - 2;
#else
    static_assert(always_false_v<Tag>,
                  "Unknown compiler, can't build type name.");
#endif

    constexpr auto lhs = [&]() -> std::string_view::size_type {
        if (auto const colon_pos = value_string.find_last_of(':');
            colon_pos != std::string_view::npos) {
            return colon_pos + 1;
        }
        return 0;
    }();
    return value_string.substr(lhs, rhs - lhs + 1);
}
} // namespace v1
} // namespace stdx

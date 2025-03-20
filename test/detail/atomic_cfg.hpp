#pragma once

#include <cstdint>
#include <type_traits>

template <> struct atomic::atomic_type<std::uint8_t> {
    using type = std::uint32_t;
};

template <> struct atomic::atomic_type<bool> {
    using type = std::uint32_t;
};

template <typename T> struct atomic::atomic_type<T *> {
    using type = std::uintptr_t;
};

#if __cplusplus >= 202002L
template <typename T>
    requires(std::is_enum_v<T>)
struct atomic::atomic_type<T> : atomic::atomic_type<std::underlying_type_t<T>> {
};
#endif

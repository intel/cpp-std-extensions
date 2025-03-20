#pragma once

#include <cstdint>

template <> struct atomic::atomic_type<std::uint8_t> {
    using type = std::uint32_t;
};

template <> struct atomic::atomic_type<bool> {
    using type = std::uint32_t;
};

template <typename T> struct atomic::atomic_type<T *> {
    using type = std::uintptr_t;
};

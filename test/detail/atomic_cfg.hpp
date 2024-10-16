#pragma once

#include <cstdint>
#include <type_traits>

template <> struct atomic::atomic_type<std::uint8_t> {
    using type = std::uint32_t;
};

template <> struct atomic::atomic_type<bool> {
    using type = std::uint32_t;
};

#pragma once

#include <cstdint>
#include <type_traits>

template <>
struct atomic::atomic_type<std::uint8_t> : std::type_identity<std::uint32_t> {};

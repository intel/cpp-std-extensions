#pragma once

#include <iterator>

namespace stdx {
inline namespace v1 {

template <typename T>
concept range = requires(T &t) {
    std::begin(t);
    std::end(t);
};

} // namespace v1
} // namespace stdx

#pragma once

#include <stdx/bit.hpp>
#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
struct place_bits_t {};
constexpr inline auto place_bits = place_bits_t{};
struct all_bits_t {};
constexpr inline auto all_bits = all_bits_t{};

enum struct lsb_t : std::size_t {};
enum struct msb_t : std::size_t {};
enum struct length_t : std::size_t {};

namespace detail {
template <std::size_t N, typename StorageElem> class bitset {
    constexpr static auto storage_elem_size =
        std::numeric_limits<StorageElem>::digits;
    constexpr static auto storage_size =
        (N + storage_elem_size - 1) / storage_elem_size;
    constexpr static auto bit = StorageElem{1U};
    constexpr static auto allbits = std::numeric_limits<StorageElem>::max();

    std::array<StorageElem, storage_size> storage{};

    constexpr static auto lastmask = []() -> StorageElem {
        if constexpr (N % storage_elem_size != 0) {
            return allbits >> (storage_elem_size - N % storage_elem_size);
        } else {
            return allbits;
        }
    }();
    constexpr auto highbits() const -> StorageElem {
        return storage.back() & lastmask;
    }

    [[nodiscard]] constexpr static auto indices(std::size_t pos) {
        struct locator {
            std::size_t index;
            std::size_t offset;
        };
        return locator{pos / storage_elem_size, pos % storage_elem_size};
    }

    [[nodiscard]] friend constexpr auto operator==(bitset const &lhs,
                                                   bitset const &rhs) -> bool {
        for (auto i = std::size_t{}; i < storage_size - 1; ++i) {
            if (lhs.storage[i] != rhs.storage[i]) {
                return false;
            }
        }
        return lhs.highbits() == rhs.highbits();
    }

#if __cpp_impl_three_way_comparison < 201907L
    [[nodiscard]] friend constexpr auto operator!=(bitset const &lhs,
                                                   bitset const &rhs) -> bool {
        return not(lhs == rhs);
    }
#endif

    friend constexpr auto operator|(bitset lhs, bitset const &rhs) -> bitset {
        lhs |= rhs;
        return lhs;
    }

    friend constexpr auto operator&(bitset lhs, bitset const &rhs) -> bitset {
        lhs &= rhs;
        return lhs;
    }

    friend constexpr auto operator^(bitset lhs, bitset const &rhs) -> bitset {
        lhs ^= rhs;
        return lhs;
    }

    friend constexpr auto operator<<(bitset lhs, std::size_t pos) -> bitset {
        lhs <<= pos;
        return lhs;
    }
    friend constexpr auto operator>>(bitset lhs, std::size_t pos) -> bitset {
        lhs >>= pos;
        return lhs;
    }

    template <typename F> constexpr auto for_each(F &&f) const -> F {
        std::size_t i = 0;
        for (auto e : storage) {
            while (e != 0) {
                auto const offset = static_cast<std::size_t>(countr_zero(e));
                e &= static_cast<StorageElem>(~(bit << offset));
                f(i + offset);
            }
            i += std::numeric_limits<StorageElem>::digits;
        }
        return std::forward<F>(f);
    }

    template <typename F, std::size_t M, typename... S>
    friend constexpr auto for_each(F &&f, bitset<M, S> const &...bs) -> F;

  public:
    constexpr bitset() = default;
    constexpr explicit bitset(std::uint64_t value) {
        if constexpr (std::is_same_v<StorageElem, std::uint64_t>) {
            storage[0] = value;
        } else {
            for (auto &elem : storage) {
                if (value == 0) {
                    break;
                }
                elem = value & allbits;
                value >>= storage_elem_size;
            }
        }
    }

    template <typename... Bs>
    constexpr explicit bitset(place_bits_t, Bs... bs) {
        static_assert((std::is_integral_v<Bs> and ...),
                      "Bit places must be integral!");
        (set(static_cast<std::size_t>(bs)), ...);
    }

    constexpr explicit bitset(all_bits_t) {
        for (auto &elem : storage) {
            elem = allbits;
        }
        storage.back() &= lastmask;
    }

    constexpr explicit bitset(std::string_view str, std::size_t pos = 0,
                              std::size_t n = std::string_view::npos,
                              char one = '1') {
        auto const len = std::min(n, str.size() - pos);
        auto i = std::size_t{};
        auto const s = str.substr(pos, std::min(len, N));
        for (auto it = std::rbegin(s); it != std::rend(s); ++it) {
            set(i++, *it == one);
        }
    }

    [[nodiscard]] constexpr auto to_uint64_t() const -> std::uint64_t {
        if constexpr (std::is_same_v<StorageElem, std::uint64_t>) {
            return storage[0] & lastmask;
        } else {
            static_assert(N <= std::numeric_limits<std::uint64_t>::digits,
                          "Bitset too big for conversion to std::uint64_t");
            std::uint64_t result{};
            for (auto i = std::size_t{}; i < storage_size - 1; ++i) {
                result <<= storage_elem_size;
                result |= storage[i];
            }
            result <<= storage_elem_size;
            result |= highbits();
            return result;
        }
    }

    constexpr static std::integral_constant<std::size_t, N> size{};

    [[nodiscard]] constexpr auto operator[](std::size_t pos) const -> bool {
        auto const [index, offset] = indices(pos);
        return (storage[index] & (bit << offset)) != 0;
    }

    constexpr auto set(std::size_t pos, bool value = true) -> bitset & {
        auto const [index, offset] = indices(pos);
        if (value) {
            storage[index] |= static_cast<StorageElem>(bit << offset);
        } else {
            storage[index] &= static_cast<StorageElem>(~(bit << offset));
        }
        return *this;
    }

    constexpr auto set(lsb_t lsb, msb_t msb, bool value = true) -> bitset & {
        auto const l = to_underlying(lsb);
        auto const m = to_underlying(msb);
        auto [l_index, l_offset] = indices(l);
        auto const [m_index, m_offset] = indices(m);

        using setfn = auto (*)(StorageElem *, StorageElem)->void;
        auto const fn = [&]() -> setfn {
            if (value) {
                return [](StorageElem *ptr, StorageElem val) { *ptr |= val; };
            }
            return [](StorageElem *ptr, StorageElem val) { *ptr &= ~val; };
        }();

        auto l_mask = std::numeric_limits<StorageElem>::max() << l_offset;
        if (l_index != m_index) {
            fn(&storage[l_index++], static_cast<StorageElem>(l_mask));
            l_mask = std::numeric_limits<StorageElem>::max();
        }
        while (l_index != m_index) {
            fn(&storage[l_index++], static_cast<StorageElem>(l_mask));
        }
        auto const m_mask = std::numeric_limits<StorageElem>::max() >>
                            (storage_elem_size - m_offset - 1);
        fn(&storage[l_index], static_cast<StorageElem>(l_mask & m_mask));
        return *this;
    }

    constexpr auto set(lsb_t lsb, length_t len, bool value = true) -> bitset & {
        auto const l = to_underlying(lsb);
        auto const length = to_underlying(len);
        return set(lsb, static_cast<msb_t>(l + length - 1), value);
    }

    constexpr auto set() -> bitset & {
        for (auto &elem : storage) {
            elem = allbits;
        }
        return *this;
    }

    constexpr auto reset(std::size_t pos) -> bitset & {
        auto const [index, offset] = indices(pos);
        storage[index] &= static_cast<StorageElem>(~(bit << offset));
        return *this;
    }

    constexpr auto reset() -> bitset & {
        for (auto &elem : storage) {
            elem = {};
        }
        return *this;
    }

    constexpr auto reset(lsb_t lsb, msb_t msb) -> bitset & {
        return set(lsb, msb, false);
    }

    constexpr auto reset(lsb_t lsb, length_t len) -> bitset & {
        return set(lsb, len, false);
    }

    constexpr auto flip(std::size_t pos) -> bitset & {
        auto const [index, offset] = indices(pos);
        storage[index] ^= static_cast<StorageElem>(bit << offset);
        return *this;
    }

    constexpr auto flip() -> bitset & {
        for (auto &elem : storage) {
            elem ^= allbits;
        }
        return *this;
    }

    [[nodiscard]] constexpr auto all() const -> bool {
        for (auto i = std::size_t{}; i < storage_size - 1; ++i) {
            if (storage[i] != allbits) {
                return false;
            }
        }
        return highbits() == lastmask;
    }

    [[nodiscard]] constexpr auto any() const -> bool {
        for (auto i = std::size_t{}; i < storage_size - 1; ++i) {
            if (storage[i] != 0) {
                return true;
            }
        }
        return highbits() != 0;
    }

    [[nodiscard]] constexpr auto none() const -> bool { return not any(); }

    [[nodiscard]] constexpr auto count() const -> std::size_t {
        std::size_t n{};
        for (auto i = std::size_t{}; i < storage_size - 1; ++i) {
            n += static_cast<std::size_t>(popcount(storage[i]));
        }
        return n + static_cast<std::size_t>(popcount(highbits()));
    }

    [[nodiscard]] constexpr auto lowest_unset() const -> std::size_t {
        std::size_t i = 0;
        for (auto e : storage) {
            if (auto offset = static_cast<std::size_t>(countr_one(e));
                offset != std::numeric_limits<StorageElem>::digits) {
                return i + offset;
            }
            i += std::numeric_limits<StorageElem>::digits;
        }
        return i;
    }

    [[nodiscard]] constexpr auto operator~() const -> bitset {
        bitset result{};
        for (auto i = std::size_t{}; i < storage_size; ++i) {
            result.storage[i] = ~storage[i];
        }
        return result;
    }

    constexpr auto operator|=(bitset const &rhs) -> bitset & {
        for (auto i = std::size_t{}; i < storage_size; ++i) {
            storage[i] |= rhs.storage[i];
        }
        return *this;
    }
    constexpr auto operator&=(bitset const &rhs) -> bitset & {
        for (auto i = std::size_t{}; i < storage_size; ++i) {
            storage[i] &= rhs.storage[i];
        }
        return *this;
    }
    constexpr auto operator^=(bitset const &rhs) -> bitset & {
        for (auto i = std::size_t{}; i < storage_size; ++i) {
            storage[i] ^= rhs.storage[i];
        }
        return *this;
    }

    constexpr auto operator<<=(std::size_t pos) -> bitset & {
        auto dst = storage_size - 1;
        auto const start = dst - pos / storage_elem_size;
        pos %= storage_elem_size;

        if (pos == 0) {
            for (auto i = start; i > std::size_t{}; --i) {
                storage[dst] = storage[i];
                --dst;
            }
        } else {
            auto const borrow_shift = storage_elem_size - pos;
            for (auto i = start; i > std::size_t{}; --i) {
                storage[dst] = static_cast<StorageElem>(storage[i] << pos);
                storage[dst] |=
                    static_cast<StorageElem>(storage[i - 1] >> borrow_shift);
                --dst;
            }
        }
        storage[dst] = static_cast<StorageElem>(storage.front() << pos);
        while (dst > std::size_t{}) {
            storage[--dst] = 0;
        }
        return *this;
    }

    constexpr auto operator>>=(std::size_t pos) -> bitset & {
        auto dst = std::size_t{};
        auto const start = pos / storage_elem_size;
        pos %= storage_elem_size;

        if (pos == 0) {
            for (auto i = start; i < storage_size - 1; ++i) {
                storage[dst] = storage[i];
                ++dst;
            }
        } else {
            auto const borrow_shift = storage_elem_size - pos;
            for (auto i = start; i < storage_size - 1; ++i) {
                storage[dst] = static_cast<StorageElem>(storage[i] >> pos);
                storage[dst] |=
                    static_cast<StorageElem>(storage[i + 1] << borrow_shift);
                ++dst;
            }
        }
        storage[dst++] = static_cast<StorageElem>(storage.back() >> pos);
        while (dst < storage_size) {
            storage[dst++] = 0;
        }
        return *this;
    }
};

template <typename F, std::size_t M, typename... S>
constexpr auto for_each(F &&f, bitset<M, S> const &...bs) -> F {
    if constexpr (sizeof...(bs) == 1) {
        return (bs.for_each(std::forward<F>(f)), ...);
    } else {
        static_assert(stdx::always_false_v<F>, "unimplemented");
        return f;
    }
}

template <std::size_t N, typename S> CONSTEVAL auto select_storage() {
    if constexpr (not std::is_same_v<S, void>) {
        static_assert(std::is_unsigned_v<S>,
                      "Underlying storage of bitset must be an unsigned type");
        return S{};
    } else if constexpr (N <= std::numeric_limits<std::uint8_t>::digits) {
        return std::uint8_t{};
    } else if constexpr (N <= std::numeric_limits<std::uint16_t>::digits) {
        return std::uint16_t{};
    } else if constexpr (N <= std::numeric_limits<std::uint32_t>::digits) {
        return std::uint32_t{};
    } else {
        return std::uint64_t{};
    }
}
} // namespace detail

template <std::size_t N, typename StorageElem = void>
using bitset =
    detail::bitset<N, decltype(detail::select_storage<N, StorageElem>())>;

namespace literals {
// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto operator""_lsb(unsigned long long int n) -> lsb_t {
    return static_cast<lsb_t>(n);
}
CONSTEVAL auto operator""_msb(unsigned long long int n) -> msb_t {
    return static_cast<msb_t>(n);
}
CONSTEVAL auto operator""_len(unsigned long long int n) -> length_t {
    return static_cast<length_t>(n);
}
// NOLINTEND(google-runtime-int)
} // namespace literals
} // namespace v1
} // namespace stdx

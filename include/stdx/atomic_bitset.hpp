#pragma once

#include <stdx/bit.hpp>
#include <stdx/bitset.hpp>
#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/detail/bitset_common.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/udls.hpp>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string_view>

namespace stdx {
inline namespace v1 {
namespace detail {
template <std::size_t N, typename StorageElem> class atomic_bitset {
    constexpr static auto bit = StorageElem{1U};

    static_assert(N <= std::numeric_limits<StorageElem>::digits,
                  "atomic_bitset is limited to a single storage element");
    static_assert(std::atomic<StorageElem>::is_always_lock_free,
                  "atomic_bitset must always be lock free");
    std::atomic<StorageElem> storage{};

    constexpr static auto mask = bit_mask<StorageElem, N - 1>();
    StorageElem salient_value(std::memory_order order) const {
        return storage.load(order) & mask;
    }

    [[nodiscard]] static constexpr auto
    value_from_string(std::string_view str, std::size_t pos, std::size_t n,
                      char one) -> StorageElem {
        StorageElem ret{};
        auto const len = std::min(n, str.size() - pos);
        auto const s = str.substr(pos, std::min(len, N));
        auto i = bit;
        for (auto it = std::rbegin(s); it != std::rend(s); ++it) {
            if (*it == one) {
                ret |= i;
            }
            i = static_cast<StorageElem>(i << 1u);
        }
        return ret;
    }

    using bitset_t = bitset<N, StorageElem>;

  public:
    constexpr atomic_bitset() = default;
    constexpr explicit atomic_bitset(std::uint64_t value)
        : storage{static_cast<StorageElem>(value & mask)} {}

    template <typename... Bs>
    constexpr explicit atomic_bitset(place_bits_t, Bs... bs)
        : storage{static_cast<StorageElem>(
              (StorageElem{} | ... |
               static_cast<StorageElem>(bit << to_underlying(bs))))} {}

    constexpr explicit atomic_bitset(all_bits_t) : storage{mask} {}

    constexpr explicit atomic_bitset(std::string_view str, std::size_t pos = 0,
                                     std::size_t n = std::string_view::npos,
                                     char one = '1')
        : storage{value_from_string(str, pos, n, one)} {}

    template <typename T>
    [[nodiscard]] auto
    to(std::memory_order order = std::memory_order_seq_cst) const -> T {
        using U = underlying_type_t<T>;
        static_assert(
            unsigned_integral<U>,
            "Conversion must be to an unsigned integral type or enum!");
        static_assert(N <= std::numeric_limits<U>::digits,
                      "atomic_bitset must fit within T");
        return static_cast<T>(storage.load(order));
    }

    [[nodiscard]] auto
    to_natural(std::memory_order order = std::memory_order_seq_cst) const {
        return storage.load(order);
    }

    operator bitset_t() const { return bitset_t{storage.load()}; }

    auto load(std::memory_order order = std::memory_order_seq_cst) const
        -> bitset_t {
        return bitset_t{storage.load(order)};
    }
    auto store(bitset_t b,
               std::memory_order order = std::memory_order_seq_cst) {
        storage.store(b.template to<StorageElem>(), order);
    }

    constexpr static std::integral_constant<std::size_t, N> size{};

    constexpr static std::bool_constant<
        std::atomic<StorageElem>::is_always_lock_free>
        is_always_lock_free{};

    template <typename T> [[nodiscard]] auto operator[](T idx) const -> bool {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        return (salient_value(std::memory_order_seq_cst) & (bit << pos)) != 0;
    }

    template <typename T>
    auto set(T idx, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        if (value) {
            return bitset_t{
                storage.fetch_or(static_cast<StorageElem>(bit << pos), order)};
        }
        return bitset_t{
            storage.fetch_and(static_cast<StorageElem>(~(bit << pos)), order)};
    }

    auto set(lsb_t lsb, msb_t msb, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const m = to_underlying(msb);
        auto const shifted_value = bit_mask<StorageElem>(m, l);
        if (value) {
            return bitset_t{storage.fetch_or(shifted_value, order)};
        }
        return bitset_t{storage.fetch_and(~shifted_value, order)};
    }

    auto set(lsb_t lsb, length_t len, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const length = to_underlying(len);
        return set(lsb, static_cast<msb_t>(l + length - 1), value, order);
    }

    auto set(std::memory_order order = std::memory_order_seq_cst)
        LIFETIMEBOUND -> atomic_bitset & {
        storage.store(mask, order);
        return *this;
    }

    template <typename T> auto reset(T idx) -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        return bitset_t{
            storage.fetch_and(static_cast<StorageElem>(~(bit << pos)))};
    }

    auto reset(std::memory_order order = std::memory_order_seq_cst)
        LIFETIMEBOUND -> atomic_bitset & {
        storage.store(StorageElem{}, order);
        return *this;
    }

    auto
    reset(lsb_t lsb, msb_t msb,
          std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        return set(lsb, msb, false, order);
    }

    auto
    reset(lsb_t lsb, length_t len,
          std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        return set(lsb, len, false, order);
    }

    template <typename T>
    auto flip(T idx,
              std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        return bitset_t{
            storage.fetch_xor(static_cast<StorageElem>(bit << pos), order)};
    }

    auto flip(std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        return bitset_t{storage.fetch_xor(mask, order)};
    }

    [[nodiscard]] auto
    all(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) == mask;
    }
    [[nodiscard]] auto
    any(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) != 0;
    }
    [[nodiscard]] auto
    none(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) == 0;
    }

    [[nodiscard]] auto
    count(std::memory_order order = std::memory_order_seq_cst) const
        -> std::size_t {
        return static_cast<std::size_t>(popcount(salient_value(order)));
    }
};
} // namespace detail

template <auto N, typename StorageElem = void>
using atomic_bitset = detail::atomic_bitset<
    to_underlying(N), decltype(smallest_uint<to_underlying(N), StorageElem>())>;
} // namespace v1
} // namespace stdx

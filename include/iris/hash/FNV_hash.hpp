#ifndef IRIS_HASH_FNV_HASH_HPP
#define IRIS_HASH_FNV_HASH_HPP

// SPDX-License-Identifier: MIT

#include <bit>
#include <memory>

#include <cstddef>

// https://datatracker.ietf.org/doc/html/draft-eastlake-fnv-03#section-2
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59406

namespace iris {

namespace detail {

template<std::size_t Base>
struct FNV_hash_numbers;

template<>
struct FNV_hash_numbers<4>
{
    static constexpr std::size_t FNV_offset_basis = 2166136261uz;
    static constexpr std::size_t FNV_prime = 16777619uz;
};

template<>
struct FNV_hash_numbers<8>
{
    static constexpr std::size_t FNV_offset_basis = 14695981039346656037uz;
    static constexpr std::size_t FNV_prime = 1099511628211uz;
};

template<std::size_t Base>
class FNV_hash_impl
{
    using N = FNV_hash_numbers<Base>;

public:
    [[nodiscard]] static constexpr std::size_t FNV1a(unsigned char const* const data, std::size_t count) noexcept
    {
        std::size_t hash = N::FNV_offset_basis;
        for (std::size_t i = 0; i < count; ++i) {
            hash ^= static_cast<std::size_t>(data[i]);
            hash *= N::FNV_prime;
        }
        return hash;
    }

    template<std::size_t count>
    [[nodiscard]] static constexpr std::size_t FNV1a(unsigned char const* const data) noexcept
    {
        std::size_t hash = N::FNV_offset_basis;
        for (std::size_t i = 0; i < count; ++i) {
            hash ^= static_cast<std::size_t>(data[i]);
            hash *= N::FNV_prime;
        }
        return hash;
    }

    template<class T>
    [[nodiscard]] static constexpr std::size_t hash(T const& t) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if consteval {
            struct Storage
            {
                alignas(T) unsigned char data[sizeof(T)];
            };
            auto const storage = std::bit_cast<Storage>(t);
            return FNV_hash_impl::FNV1a<sizeof(T)>(storage.data);

        } else {
            return FNV_hash_impl::FNV1a<sizeof(T)>(reinterpret_cast<unsigned char const*>(std::addressof(t)));
        }
    }
};

} // detail

template<std::size_t Base = sizeof(std::size_t)>
using FNV_hash = detail::FNV_hash_impl<Base>;

}  // iris

#endif

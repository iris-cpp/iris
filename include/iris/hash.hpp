#ifndef IRIS_HASH_HPP
#define IRIS_HASH_HPP

// SPDX-License-Identifier: MIT

#include <iris/hash/FNV_hash.hpp>

#include <iris/requirements.hpp>
#include <iris/type_traits.hpp>

#include <functional>
#include <bit>

#include <cstddef>
#include <cstdint>

namespace iris {

namespace detail {

template<std::size_t Base>
constexpr std::size_t hash_mix(std::size_t) noexcept = delete;

// https://jonkagstrom.com/mx3/mx3_rev2.html

template<>
[[nodiscard]] constexpr std::size_t hash_mix<4>(std::size_t x) noexcept
{
    x ^= x >> 16;
    x *= 0x21f0aaaduz;
    x ^= x >> 15;
    x *= 0x735a2d97uz;
    x ^= x >> 15;
    return x;
}

template<>
[[nodiscard]] constexpr std::size_t hash_mix<8>(std::size_t x) noexcept
{
    x ^= x >> 32;
    x *= 0xe9846af9b1a615duz;
    x ^= x >> 32;
    x *= 0xe9846af9b1a615duz;
    x ^= x >> 28;
    return x;
}

} // detail

// https://github.com/boostorg/container_hash/blob/5d8b8ac2b9d9d7cb3818f88fd7e6372e5f072ff5/include/boost/container_hash/hash.hpp#L472C53-L472C63
// https://softwareengineering.stackexchange.com/questions/402542/where-do-magic-hashing-constants-like-0x9e3779b9-and-0x9e3779b1-come-from

template<class T>
[[nodiscard]] constexpr std::size_t hash_combine(std::size_t const seed, T const& v) noexcept
{
    static_assert(is_hash_enabled_v<T>);
    return detail::hash_mix<sizeof(std::size_t)>(
        seed + 0x9e3779b97f4a7c55uz + std::hash<T>{}(v)
    );
}

} // iris

#endif

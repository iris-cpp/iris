#ifndef IRIS_ZZ_NGRAM_GRAM_HPP
#define IRIS_ZZ_NGRAM_GRAM_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <array>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <compare>

#include <cassert>
#include <cstdint>

namespace iris::ngram {

namespace detail {

template<std::size_t Bytes> struct gram_value;
template<> struct gram_value<1> { using type = std::uint8_t; };
template<> struct gram_value<2> { using type = std::uint16_t; };
template<> struct gram_value<4> { using type = std::uint32_t; };
template<> struct gram_value<8> { using type = std::uint64_t; };

template<std::size_t Bytes>
using gram_value_t = gram_value<Bytes>::type;

} // detail

template<std::size_t N, class CharT>
struct gram
{
    static_assert(N >= 3);

    std::array<CharT, N> data;

    template<std::forward_iterator It>
    constexpr void copy_n(It it)
        noexcept(noexcept(*it++))
    {
        std::ranges::copy_n(it, N, data.begin());
    }
    template<std::forward_iterator It>
    [[nodiscard]] static constexpr gram from_copy_n(It it)
        noexcept(noexcept(std::declval<gram&>().copy_n(std::move(it))))
    {
        gram ng;
        ng.copy_n(std::move(it));
        return ng;
    }

    template<std::forward_iterator It>
    constexpr void shift_copy(It it, int const remaining_chars)
        noexcept(
            noexcept(std::shift_left(data.begin(), data.end(), remaining_chars)) &&
            noexcept(std::ranges::copy_n(it, remaining_chars, data.begin() + (N - remaining_chars)))
        )
    {
        assert(remaining_chars < int(N));
        std::shift_left(data.begin(), data.end(), remaining_chars);
        std::ranges::copy_n(it, remaining_chars, data.begin() + (N - remaining_chars));
    }

    template<std::size_t Len>
    [[nodiscard]] static constexpr gram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        static_assert(Len == N + 1);
        assert(chars[Len - 1] == static_cast<CharT>(0));
        return gram::from_copy_n(std::ranges::begin(chars));
    }

    [[nodiscard]] constexpr bool operator==(gram const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram const&) const noexcept = default;
};

template<class CharT>
struct gram<1, CharT>
{
    CharT data;

    template<std::forward_iterator It>
    constexpr void copy_n(It it)
        noexcept(noexcept(*it))
    {
        data = *it;
    }
    template<std::forward_iterator It>
    [[nodiscard]] static constexpr gram from_copy_n(It it)
        noexcept(noexcept(std::declval<gram&>().copy_n(std::move(it))))
    {
        gram ng;
        ng.copy_n(std::move(it));
        return ng;
    }

    template<std::size_t Len>
    [[nodiscard]] static constexpr gram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        static_assert(Len == 1 + 1);
        assert(chars[Len - 1] == static_cast<CharT>(0));
        return gram::from_copy_n(std::ranges::begin(chars));
    }

    [[nodiscard]] constexpr bool operator==(gram const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram const&) const noexcept = default;
};

template<class CharT>
struct gram<2, CharT>
{
    using value_type = detail::gram_value_t<sizeof(CharT) * 2>;
    value_type data;

    template<std::forward_iterator It>
    constexpr void copy_n(It it)
        noexcept(noexcept(*it++))
    {
        using uchar = std::make_unsigned_t<CharT>;
        data  = value_type(static_cast<uchar>(*it++)) << (sizeof(CharT) * 8);
        data |= value_type(static_cast<uchar>(*it));
    }
    template<std::forward_iterator It>
    [[nodiscard]] static constexpr gram from_copy_n(It it)
        noexcept(noexcept(std::declval<gram&>().copy_n(std::move(it))))
    {
        gram ng;
        ng.copy_n(std::move(it));
        return ng;
    }

    template<std::forward_iterator It>
    constexpr void shift_copy(It it, int const remaining_chars)
        noexcept(noexcept(*it))
    {
        assert(remaining_chars == 1);
        (void)remaining_chars;
        data = (data << (sizeof(CharT) * 8)) | value_type(static_cast<std::make_unsigned_t<CharT>>(*it));
    }

    template<std::size_t Len>
    [[nodiscard]] static constexpr gram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        static_assert(Len == 2 + 1);
        assert(chars[Len - 1] == static_cast<CharT>(0));
        return gram::from_copy_n(std::ranges::begin(chars));
    }

    [[nodiscard]] constexpr bool operator==(gram const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(gram const&) const noexcept = default;
};

} // iris::ngram

namespace iris {

template<class CharT, std::size_t N>
[[nodiscard]] ngram::gram<N - 1, CharT> to_ngram(CharT const (&chars)[N]) noexcept
{
    return ngram::gram<N - 1, CharT>::from_c_array(chars);
}

} // iris

#endif

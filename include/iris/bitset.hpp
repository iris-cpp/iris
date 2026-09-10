#ifndef IRIS_ZZ_BITSET_HPP
#define IRIS_ZZ_BITSET_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/error/throwf.hpp>
#include <iris/type_traits.hpp>
#include <iris/hash.hpp>

#include <initializer_list>
#include <iterator>
#include <concepts>
#include <format>
#include <memory> // std::allocator
#include <string>
#include <string_view>
#include <bit>
#include <ranges>
#include <span>
#include <functional>
#include <algorithm>
#include <compare>
#include <type_traits>
#include <utility>
#include <stdexcept>

#include <cstring> // memset
#include <cstddef> // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep
#include <climits>

namespace iris {

namespace detail {

template<class BitsetT>
class bitset_positions_view;

template<std::size_t N>
[[nodiscard]] consteval auto bitset_default_word() noexcept
{
    if constexpr (N <= 8) {
        return std::type_identity<std::uint_least8_t>{};
    } else if constexpr (N <= 16) {
        return std::type_identity<std::uint_least16_t>{};
    } else if constexpr (N <= 32) {
        return std::type_identity<std::uint_least32_t>{};
    } else {
        return std::type_identity<std::uint_least64_t>{};
    }
}

} // detail

template<std::size_t N, class T = decltype(detail::bitset_default_word<N>())::type>
class bitset
{
    static_assert(iris::unsigned_numeric_integral<T>);

    friend class detail::bitset_positions_view<bitset>;

    static constexpr std::ptrdiff_t bits_per_word = sizeof(T) * CHAR_BIT;
    static constexpr std::ptrdiff_t words = N == 0 ? 0 : (N - 1) / bits_per_word; // -1

    static constexpr T last_word_mask = N == 0 || N % bits_per_word != 0
        ? static_cast<T>((T{1u} << (N % bits_per_word)) - 1)
        : static_cast<T>(~T{});

public:
    static constexpr std::size_t bits = N;
    using word_type = T;

    template<std::ranges::input_range R>
    [[nodiscard]] static constexpr bitset from_positions(R&& positions)
    {
        return bitset::from_positions_impl(std::ranges::begin(positions), std::ranges::end(positions));
    }

    template<class U>
    [[nodiscard]] static constexpr bitset from_positions(std::initializer_list<U> positions)
    {
        return bitset::from_positions_impl(positions.begin(), positions.end());
    }

private:
    template<std::input_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bitset from_positions_impl(It first, Se last)
    {
        if constexpr (N == 0) {
            if (first != last) {
                throwf<std::out_of_range>("you cannot construct a 0-bit bitset from a non-empty range");
            }
            return bitset{};

        } else {
            bitset tmp;
            for (; first != last; ++first) {
                tmp.set(static_cast<std::size_t>(*first));
            }
            return tmp;
        }
    }

public:
    constexpr bitset() noexcept = default;

    constexpr bitset(unsigned long long value) noexcept
    {
        constexpr std::ptrdiff_t ull_bits = sizeof(unsigned long long) * CHAR_BIT;
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] = static_cast<T>(value);
            if constexpr (bits_per_word < ull_bits) {
                value >>= bits_per_word;
            } else {
                value = 0;
            }
        }
        trim();
    }

    template<class CharT, class Traits>
    constexpr explicit bitset(
        std::basic_string_view<CharT, Traits> str,
        std::size_t pos = 0,
        std::size_t n = std::basic_string_view<CharT, Traits>::npos,
        CharT zero = CharT('0'),
        CharT one = CharT('1')
    )
    {
        if (pos > str.size()) throwf<std::out_of_range>("pos is out of range");
        str = str.substr(pos, n);
        this->template assign_from_chars<Traits>(str.data(), str.size(), zero, one);
    }

    template<class CharT, class Traits, class Allocator>
    constexpr explicit bitset(
        std::basic_string<CharT, Traits, Allocator> const& str,
        std::size_t pos = 0,
        std::size_t n = std::basic_string<CharT, Traits, Allocator>::npos,
        CharT zero = CharT('0'),
        CharT one = CharT('1')
    )
        : bitset(std::basic_string_view<CharT, Traits>(str), pos, n, zero, one)
    {}

    template<class CharT>
    constexpr explicit bitset(
        CharT const* str,
        std::size_t n = std::basic_string_view<CharT>::npos,
        CharT zero = CharT('0'),
        CharT one = CharT('1')
    )
        : bitset(
            n == std::basic_string_view<CharT>::npos
                ? std::basic_string_view<CharT>(str)
                : std::basic_string_view<CharT>(str, n),
            0, std::basic_string_view<CharT>::npos, zero, one
        )
    {}

    // --------------------------------------------------------

    [[nodiscard]] constexpr bool operator[](std::size_t pos) const noexcept
    {
        return this->subscript(pos);
    }

    [[nodiscard]] constexpr bool test(std::size_t pos) const
    {
        if (pos >= N) throwf<std::out_of_range>("pos is out of range");
        return this->subscript(pos);
    }

    [[nodiscard]] constexpr bool all() const noexcept
    {
        if constexpr (words > 0) {
            for (std::ptrdiff_t word_pos = 0; word_pos < words; ++word_pos) {
                if (data_[word_pos] != static_cast<T>(~T{})) return false;
            }
        }
        return data_[words] == last_word_mask;
    }

    [[nodiscard]] constexpr bool any() const noexcept
    {
        return !this->none();
    }

    [[nodiscard]] constexpr bool none() const noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            if (data_[word_pos]) return false;
        }
        return true;
    }

    // Returns the pop count
    [[nodiscard]] constexpr std::size_t count() const noexcept
    {
        std::size_t c = 0;
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            c += std::popcount(data_[word_pos]);
        }
        return c;
    }

    // Returns the number of bits that the bitset holds
    [[nodiscard]] static constexpr std::size_t size() noexcept
    {
        return N;
    }

    // ----------------------------------------------------

    constexpr bitset& set() noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] = static_cast<T>(~T{});
        }
        trim();
        return *this;
    }

    constexpr bitset& set(std::size_t pos, bool value = true)
    {
        if (pos >= N) throwf<std::out_of_range>("pos is out of range");
        return this->set_unchecked(pos, value);
    }

    constexpr bitset& reset() noexcept
    {
        if consteval {
            std::ranges::fill(data_, T{});
        } else {
            std::memset(&data_, 0, sizeof(data_));
        }
        return *this;
    }

    constexpr bitset& reset(std::size_t pos)
    {
        if (pos >= N) throwf<std::out_of_range>("pos is out of range");
        return this->set_unchecked(pos, false);
    }

    constexpr bitset& flip() noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] = static_cast<T>(~data_[word_pos]);
        }
        trim();
        return *this;
    }

    constexpr bitset& flip(std::size_t pos)
    {
        if (pos >= N) throwf<std::out_of_range>("pos is out of range");
        data_[pos / bits_per_word] ^= bit_mask(pos);
        return *this;
    }

    // ----------------------------------------------------

    constexpr bitset& operator&=(bitset const& other) noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] &= other.data_[word_pos];
        }
        return *this;
    }

    constexpr bitset& operator|=(bitset const& other) noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] |= other.data_[word_pos];
        }
        return *this;
    }

    constexpr bitset& operator^=(bitset const& other) noexcept
    {
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            data_[word_pos] ^= other.data_[word_pos];
        }
        return *this;
    }

    // ----------------------------------------------------

    [[nodiscard]] constexpr bitset operator~() const noexcept
    {
        bitset tmp = *this;
        tmp.flip();
        return tmp;
    }

    // ----------------------------------------------------

    constexpr bitset& operator<<=(std::size_t pos) noexcept
    {
        auto const word_shift = static_cast<std::ptrdiff_t>(pos / bits_per_word);
        if (word_shift != 0) {
            for (std::ptrdiff_t word_pos = words; 0 <= word_pos; --word_pos) {
                data_[word_pos] = word_shift <= word_pos ? data_[word_pos - word_shift] : 0;
            }
        }
        if ((pos %= bits_per_word) != 0) {
            if constexpr (words > 0) {
                for (std::ptrdiff_t word_pos = words; 0 < word_pos; --word_pos) {
                    data_[word_pos] = (data_[word_pos] << pos) | (data_[word_pos - 1] >> (bits_per_word - pos));
                }
            }
            data_[0] <<= pos;
        }
        trim();
        return *this;
    }

    [[nodiscard]] constexpr bitset operator<<(std::size_t pos) const noexcept
    {
        bitset tmp = *this;
        tmp <<= pos;
        return tmp;
    }

    constexpr bitset& operator>>=(std::size_t pos) noexcept
    {
        auto const word_shift = static_cast<std::ptrdiff_t>(pos / bits_per_word);
        if (word_shift != 0) {
            for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
                data_[word_pos] = word_shift <= words - word_pos ? data_[word_pos + word_shift] : 0;
            }
        }
        if ((pos %= bits_per_word) != 0) {
            if constexpr (words > 0) {
                for (std::ptrdiff_t word_pos = 0; word_pos < words; ++word_pos) {
                    data_[word_pos] = (data_[word_pos] >> pos) | (data_[word_pos + 1] << (bits_per_word - pos));
                }
            }
            data_[words] >>= pos;
        }
        return *this;
    }

    [[nodiscard]] constexpr bitset operator>>(std::size_t pos) const noexcept
    {
        bitset tmp = *this;
        tmp >>= pos;
        return tmp;
    }

    // ----------------------------------------------------

    [[nodiscard]] constexpr bool operator==(bitset const&) const noexcept = default;

    [[nodiscard]] constexpr std::strong_ordering operator<=>(bitset const& other) const noexcept
    {
        for (std::ptrdiff_t word_pos = words; 0 <= word_pos; --word_pos) {
            if (auto const c = data_[word_pos] <=> other.data_[word_pos]; c != 0) {
                return c;
            }
        }
        return std::strong_ordering::equal;
    }

    // ----------------------------------------------------

    [[nodiscard]] constexpr unsigned long to_ulong() const
    {
        return this->to_unsigned<unsigned long>();
    }

    [[nodiscard]] constexpr unsigned long long to_ullong() const
    {
        return this->to_unsigned<unsigned long long>();
    }

    template<class CharT = char, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    [[nodiscard]] constexpr std::basic_string<CharT, Traits, Allocator>
    to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const
    {
        std::basic_string<CharT, Traits, Allocator> str(N, zero);
        if constexpr (N != 0) {
            for (std::size_t pos = 0; pos < N; ++pos) {
                if ((*this)[pos]) {
                    str[N - 1 - pos] = one;
                }
            }
        }
        return str;
    }

    // ---------------------------------------------------

    [[nodiscard]] constexpr std::span<T const, words + 1> data() const noexcept
    {
        return data_;
    }

    [[nodiscard]] constexpr detail::bitset_positions_view<bitset> positions() const noexcept;

private:
    friend struct std::hash<bitset>;

    [[nodiscard]] static constexpr T bit_mask(std::size_t pos) noexcept
    {
        return static_cast<T>(T{1u} << (pos % bits_per_word));
    }

    [[nodiscard]] constexpr bool subscript(std::size_t pos) const noexcept
    {
        return (data_[pos / bits_per_word] & bitset::bit_mask(pos)) != 0;
    }

    constexpr bitset& set_unchecked(std::size_t pos, bool value) noexcept
    {
        T& word = data_[pos / bits_per_word];
        if (value) {
            word |= bitset::bit_mask(pos);
        } else {
            word &= static_cast<T>(~bitset::bit_mask(pos));
        }
        return *this;
    }

    constexpr void trim() noexcept
    {
        if constexpr (last_word_mask != static_cast<T>(~T{})) {
            data_[words] &= last_word_mask;
        }
    }

    template<class Traits, class CharT>
    constexpr void assign_from_chars(CharT const* chars, std::size_t len, CharT zero, CharT one)
    {
        for (std::size_t i = 0; i < len; ++i) {
            if (!Traits::eq(chars[i], zero) && !Traits::eq(chars[i], one)) {
                throwf<std::invalid_argument>("character is neither zero nor one");
            }
        }
        std::size_t const m = std::min(N, len);
        for (std::size_t i = 0; i < m; ++i) {
            if (Traits::eq(chars[m - 1 - i], one)) {
                this->set_unchecked(i, true);
            }
        }
    }

    // std::overflow_error if any set bit does not fit into U.
    template<std::unsigned_integral U>
    [[nodiscard]] constexpr U to_unsigned() const
    {
        constexpr std::ptrdiff_t u_bits = sizeof(U) * CHAR_BIT;
        U result = 0;
        for (std::ptrdiff_t word_pos = 0; word_pos <= words; ++word_pos) {
            std::ptrdiff_t const shift = word_pos * bits_per_word;
            if (shift >= u_bits) {
                if (data_[word_pos] != 0) {
                    throwf<std::overflow_error>("bitset does not fit into the target type");
                }
                continue;
            }
            if (shift + bits_per_word > u_bits) {
                if ((data_[word_pos] >> (u_bits - shift)) != 0) {
                    throwf<std::overflow_error>("bitset does not fit into the target type");
                }
            }
            result |= static_cast<U>(static_cast<U>(data_[word_pos]) << shift);
        }
        return result;
    }

    T data_[words + 1]{};
};

template<std::size_t N, class T>
[[nodiscard]] constexpr bitset<N, T> operator&(bitset<N, T> const& a, bitset<N, T> const& b) noexcept
{
    bitset<N, T> tmp = a;
    tmp &= b;
    return tmp;
}

template<std::size_t N, class T>
[[nodiscard]] constexpr bitset<N, T> operator|(bitset<N, T> const& a, bitset<N, T> const& b) noexcept
{
    bitset<N, T> tmp = a;
    tmp |= b;
    return tmp;
}

template<std::size_t N, class T>
[[nodiscard]] constexpr bitset<N, T> operator^(bitset<N, T> const& a, bitset<N, T> const& b) noexcept
{
    bitset<N, T> tmp = a;
    tmp ^= b;
    return tmp;
}

namespace detail {

template<std::size_t N, class T>
class bitset_positions_view<bitset<N, T>>
    : public std::ranges::view_interface<bitset_positions_view<bitset<N, T>>>
{
public:
    class iterator
    {
    public:
        using iterator_concept = std::bidirectional_iterator_tag;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr iterator() noexcept = default;

        [[nodiscard]] constexpr std::size_t operator*() const noexcept
        {
            return static_cast<std::size_t>(word_pos_) * bitset<N, T>::bits_per_word + static_cast<std::size_t>(std::countr_zero(cur_));
        }

        constexpr iterator& operator++() noexcept
        {
            cur_ &= static_cast<T>(cur_ - 1); // clear the lowest set bit
            this->skip_zero_words();
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--() noexcept
        {
            T below = 0;
            if (word_pos_ <= bitset<N, T>::words) {
                below = static_cast<T>(data_[word_pos_] ^ cur_);
            }
            while (below == 0) {
                --word_pos_;
                below = data_[word_pos_];
                cur_ = 0;
            }
            cur_ |= static_cast<T>(T{1u} << (std::bit_width(below) - 1));
            return *this;
        }

        constexpr iterator operator--(int) noexcept
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] constexpr bool operator==(iterator const&) const noexcept = default;

    private:
        friend bitset_positions_view;

        struct end_tag {};

        constexpr explicit iterator(T const* data) noexcept
            : data_(data)
            , word_pos_(0)
            , cur_(data[0])
        {
            this->skip_zero_words();
        }

        constexpr iterator(T const* data, end_tag) noexcept
            : data_(data)
        {}

        constexpr void skip_zero_words() noexcept
        {
            while (cur_ == 0) {
                if (++word_pos_ > bitset<N, T>::words) return;
                cur_ = data_[word_pos_];
            }
        }

        T const* data_ = nullptr;
        std::ptrdiff_t word_pos_ = bitset<N, T>::words + 1;
        T cur_ = 0;
    };

    constexpr bitset_positions_view() noexcept = default;

    constexpr explicit bitset_positions_view(bitset<N, T> const& bs) noexcept
        : bs_(bs)
    {}

    [[nodiscard]] constexpr iterator begin() const noexcept
    {
        return iterator(bs_.data_);
    }

    [[nodiscard]] constexpr iterator end() const noexcept
    {
        return iterator(bs_.data_, typename iterator::end_tag{});
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
        return bs_.count();
    }

private:
    bitset<N, T> bs_;
};

} // detail

template<std::size_t N, class T>
constexpr detail::bitset_positions_view<bitset<N, T>> bitset<N, T>::positions() const noexcept
{
    return detail::bitset_positions_view<bitset<N, T>>{*this};
}

} // iris

template<std::size_t N, class T>
struct std::hash<iris::bitset<N, T>>
{
    [[nodiscard]] static std::size_t operator()(iris::bitset<N, T> const& bs) noexcept
    {
        return iris::hash_value(bs.data_);
    }
};

template<std::size_t N, class T, class CharT>
struct std::formatter<iris::bitset<N, T>, CharT>
    : std::formatter<std::basic_string<CharT>, CharT>
{
    template<class FormatContext>
    FormatContext::iterator format(iris::bitset<N, T> const& bs, FormatContext& ctx) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(bs.template to_string<CharT>(), ctx);
    }
};

#endif

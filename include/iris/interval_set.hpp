#ifndef IRIS_ZZ_INTERVAL_SET_HPP
#define IRIS_ZZ_INTERVAL_SET_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/iterator.hpp>

#include <format>
#include <concepts>
#include <map>
#include <iterator>
#include <ranges>
#include <utility>
#include <compare>

#include <cstddef> // IWYU pragma: keep

namespace iris {

template<
    class IntervalT,
    class MapT = std::map<typename IntervalT::value_type, typename IntervalT::value_type>
>
class interval_set
{
public:
    using interval_type = IntervalT;
    using map_type = MapT;
    using offset_type = IntervalT::value_type;

    class const_iterator : public iterator_base<typename map_type::const_iterator>
    {
        using typename iterator_base<typename map_type::const_iterator>::iterator_base_type;
        static_assert(std::bidirectional_iterator<iterator_base_type>);
        iterator_base_type it_;

    public:
        using value_type = IntervalT;
        using pointer    = IntervalT const*;
        using reference  = IntervalT;

        constexpr const_iterator() noexcept = default;

        constexpr explicit const_iterator(iterator_base_type it) noexcept
            : it_(std::move(it))
        {}

        [[nodiscard]] constexpr IntervalT operator*() const noexcept
        {
            return {it_->first, it_->second};
        }

        constexpr const_iterator& operator++() noexcept
        {
            ++it_;
            return *this;
        }

        [[nodiscard]] constexpr const_iterator operator++(int) noexcept
        {
            auto temp{*this};
            ++it_;
            return temp;
        }

        constexpr const_iterator& operator--() noexcept
        {
            --it_;
            return *this;
        }

        [[nodiscard]] constexpr const_iterator operator--(int) noexcept
        {
            auto temp{*this};
            --it_;
            return temp;
        }

        [[nodiscard]] constexpr bool operator==(const_iterator const&) const noexcept = default;
        [[nodiscard]] constexpr auto operator<=>(const_iterator const&) const noexcept = default;
    };

    using iterator = const_iterator;

    constexpr interval_set() = default;

    constexpr explicit interval_set(std::initializer_list<IntervalT> il)
    {
        auto it = il.begin();
        if (it == il.end()) return;
        if (!it->empty()) {
            map_.emplace(it->lower, it->upper);
        }
        for (++it; it != il.end(); ++it) {
            this->insert(*it);
        }
    }

    template<std::input_iterator It, std::sentinel_for<It> Se>
        requires std::convertible_to<std::iter_value_t<It>, IntervalT>
    constexpr interval_set(It it, Se se)
    {
        if (it == se) return;
        if (IntervalT const iv = *it; !iv.empty()) {
            map_.emplace(iv.lower, iv.upper);
        }
        for (++it; it != se; ++it) {
            this->insert(*it);
        }
    }

    template<std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, IntervalT>
    constexpr interval_set(std::from_range_t, R&& r)
        : interval_set(std::ranges::begin(r), std::ranges::end(r))
    {}

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator{map_.begin()};
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator{map_.end()};
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return map_.empty(); }
    [[nodiscard]] constexpr std::size_t size() const noexcept { return map_.size(); }

    constexpr void clear() noexcept
    {
        map_.clear();
    }

    // Total number of covered offsets (sum of lengths)
    constexpr offset_type coverage() const noexcept
    {
        offset_type total = 0;
        for (auto const& [lower, upper] : map_) {
            total += upper - lower;
        }
        return total;
    }

    // O(1)
    [[nodiscard]] constexpr IntervalT extent() const noexcept
    {
        if (map_.empty()) return {};
        return {map_.begin()->first, std::prev(map_.end())->second};
    }

    // Insert [iv.lower, iv.upper), merging with any interval it overlaps or touches
    constexpr void insert(IntervalT iv)
    {
        if (iv.empty()) return;

        auto it = map_.lower_bound(iv.lower);
        if (it != map_.begin()) {
            auto prev = std::prev(it);
            if (prev->second >= iv.lower) {
                it = prev;
            }
        }

        // Absorb every interval that overlaps or touches [iv.lower, iv.upper)
        while (it != map_.end() && it->first <= iv.upper) {
            if (it->first < iv.lower) iv.lower = it->first;
            if (it->second > iv.upper) iv.upper  = it->second;
            it = map_.erase(it);
        }

        map_.emplace(iv.lower, iv.upper);
    }

    constexpr void insert(offset_type lower, offset_type upper)
    {
        this->insert(IntervalT{lower, upper});
    }

    // --------------------------------------

    [[nodiscard]] constexpr bool intersects(IntervalT const iv) const
    {
        if (iv.empty()) return false;
        auto const it = map_.upper_bound(iv.lower);
        if (it != map_.begin() && std::prev(it)->second > iv.lower) return true;
        return it != map_.end() && it->first < iv.upper;
    }

    [[nodiscard]] constexpr bool covers(IntervalT const iv) const
    {
        if (iv.empty()) return true;
        auto const it = map_.upper_bound(iv.lower);
        if (it == map_.begin()) return false;
        auto const& [lower, upper] = *std::prev(it);
        return lower <= iv.lower && iv.upper <= upper;
    }

    [[nodiscard]] constexpr bool contains(offset_type p) const
    {
        auto const it = map_.upper_bound(p);
        if (it == map_.begin()) return false;
        return std::prev(it)->second > p;
    }

    [[nodiscard]] constexpr bool operator==(interval_set const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(interval_set const&) const noexcept = default;

    constexpr void swap(interval_set& other) noexcept
    {
        using std::swap;
        swap(map_, other.map_);
    }

private:
    MapT map_;
};

template<class IntervalT, class MapT>
constexpr void swap(interval_set<IntervalT, MapT>& a, interval_set<IntervalT, MapT>& b) noexcept
{
    a.swap(b);
}

} // iris

template<class IntervalT, class MapT, class CharT>
struct std::formatter<iris::interval_set<IntervalT, MapT>, CharT>
{
    [[nodiscard]] constexpr std::basic_format_parse_context<CharT>::const_iterator
    parse(std::basic_format_parse_context<CharT>& ctx)
    {
        return iv_fmt_.parse(ctx);
    }

    template<class Ctx>
    Ctx::iterator format(iris::interval_set<IntervalT, MapT> const& ivs, Ctx& ctx) const
    {
        ctx.advance_to(std::format_to(ctx.out(), "{{"));
        bool is_first = true;
        for (auto const& iv : ivs) {
            if (is_first) {
                ctx.advance_to(iv_fmt_.format(iv, ctx));
                is_first = false;
            } else {
                ctx.advance_to(std::format_to(ctx.out(), " "));
                ctx.advance_to(iv_fmt_.format(iv, ctx));
            }
        }
        return std::format_to(ctx.out(), "}}");
    }

private:
    std::formatter<IntervalT, CharT> iv_fmt_;
};

#endif

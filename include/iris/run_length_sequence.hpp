#ifndef IRIS_ZZ_RUN_LENGTH_SEQUENCE_HPP
#define IRIS_ZZ_RUN_LENGTH_SEQUENCE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/indexed_value.hpp>
#include <iris/interval.hpp>

#include <iris/error/throwf.hpp>

#include <iris/default_init_allocator.hpp>
#include <iris/ranges.hpp>
#include <iris/container_traits.hpp>
#include <iris/requirements.hpp>
#include <iris/type_traits.hpp>

#include <algorithm>
#include <vector>
#include <ranges>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <concepts>
#include <type_traits>

#include <cstddef> // IWYU pragma: keep
#include <cassert>

namespace iris::detail {
struct run_length_sequence_comp;
} // iris::detail

namespace iris {

template<class T, class IndexT>
struct run_length_run_ref
{
    T const& value;
    interval<IndexT> span;
};

// A compressed bidirectional container that holds only one instance of `T`
// per each adjacent equivalent elements.
//
// Invariant: adjacent runs never compare equal. This makes the representation
// canonical, which `operator==` relies on.
//
// Iterator invalidation does not follow `RunContainerT`: any insertion that starts a
// new run may invalidate all iterators, regardless of `RunContainerT`. Reference
// stability of the elements follows `RunContainerT`.
//
// Precondition for heterogeneous comparison: `e == value` must hold iff
// `e == T(value)`; otherwise the run structure would depend on which overload
// of `emplace_back` (or relevant insertion members) was chosen.
//
// When inserting an element, `run_length_sequence` does lazy construction of the
// actual element object iff heterogeneous comparison is supported for the given
// type `U`; otherwise,
//   - If `RunContainerT` supports `runs_.pop_back()` or `runs_.erase(iterator to the
//     last inserted element)`, the element is first constructed in-place and
//     the resulting object is used for comparison; when the comparison holds, the
//     last element is popped back or erased. Otherwise,
//
//   - The element is first constructed as a local variable `temp` and is used for
//     comparison; when the comparison does not hold, the object is move-inserted
//     into `RunContainerT` as if by `runs_.emplace_back(std::forward<T>(temp))`.
//
// Internal structure is maintained with two independent containers, where the first
// container holds one element instance per each run and the other one holds the
// *offsets* that represent each insertion of elements.
//
// For example, when items are emplaced in this order:
//   A A A B B C C C C
//
// then the internal state is:
//   runs_:
//     [A] [B] [C]
//
//   offsets_:
//     [0] [3] [5] [9 /* sentinel */]
template<
    class T,
    class IndexT = unsigned,
    template<class, class> class IndexedValuePairTT = indexed_value,
    class RunContainerT = std::vector<T>
>
class run_length_sequence
{
public:
    static_assert(!std::is_const_v<T>);
    static_assert(unsigned_numeric_integral<IndexT> && !std::is_const_v<IndexT>);
    static_assert(sizeof(IndexT) >= sizeof(int), "Small index type has no practical benefit");
    static_assert(!std::is_const_v<RunContainerT>);

    using size_type = std::common_type_t<std::size_t, IndexT>;
    using difference_type = std::ptrdiff_t;

    [[nodiscard]] static constexpr size_type max_size() noexcept
    {
        constexpr auto a = std::numeric_limits<IndexT>::max();
        constexpr auto b = std::numeric_limits<difference_type>::max();
        return std::cmp_less(b, a) ? static_cast<size_type>(b) : static_cast<size_type>(a);
    }

private:
    using offsets_type = std::vector<IndexT, default_init_allocator<IndexT>>;

    struct iterator_impl
    {
    private:
        using value_iterator = std::ranges::iterator_t<RunContainerT const>;
        using offset_iterator = offsets_type::const_iterator;

    public:
        using iterator_concept = std::bidirectional_iterator_tag;
        using iterator_category = std::input_iterator_tag; // Our reference type is proxy, so this can only be input

        using value_type = IndexedValuePairTT<
            IndexT,
            std::iter_value_t<value_iterator>
        >;
        using reference = IndexedValuePairTT<
            IndexT,
            std::iter_reference_t<value_iterator>
        >;
        using difference_type = run_length_sequence::difference_type;

        constexpr iterator_impl() noexcept = default;

        constexpr explicit iterator_impl(value_iterator value_it, offset_iterator ofs_it) noexcept
            : value_it_(std::move(value_it))
            , ofs_it_(std::move(ofs_it))
        {}

        constexpr explicit iterator_impl(value_iterator value_it, offset_iterator ofs_it, IndexT pos) noexcept
            : value_it_(std::move(value_it))
            , ofs_it_(std::move(ofs_it))
            , rel_pos_(pos)
        {}

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return {index(), *value_it_};
        }

        constexpr iterator_impl& operator++() noexcept
        {
            auto ofs_next = std::next(ofs_it_);
            if (++rel_pos_ == *ofs_next - *ofs_it_) {
                ofs_it_ = std::move(ofs_next);
                rel_pos_ = static_cast<IndexT>(0u);
                ++value_it_;
            }
            return *this;
        }

        [[nodiscard]] constexpr iterator_impl operator++(int) noexcept
        {
            auto temp{*this};
            ++*this;
            return temp;
        }

        constexpr iterator_impl& operator--() noexcept
        {
            if (rel_pos_ != static_cast<IndexT>(0u)) {
                --rel_pos_;
                return *this;
            }

            auto const current_ofs = *ofs_it_--;
            rel_pos_ = current_ofs - *ofs_it_ - static_cast<IndexT>(1u);
            --value_it_;
            return *this;
        }

        [[nodiscard]] constexpr iterator_impl operator--(int) noexcept
        {
            auto temp{*this};
            --*this;
            return temp;
        }

        [[nodiscard]] constexpr bool operator==(iterator_impl const&) const noexcept = default;

        [[nodiscard]] friend constexpr difference_type operator-(iterator_impl const& a, iterator_impl const& b) noexcept
        {
            if (a == b) return 0;
            return static_cast<difference_type>(a.index()) - static_cast<difference_type>(b.index());
        }

    private:
        [[nodiscard]] constexpr IndexT index() const noexcept
        {
            return static_cast<IndexT>(*ofs_it_ + rel_pos_);
        }

        value_iterator value_it_{};
        offset_iterator ofs_it_{};
        IndexT rel_pos_{};
    };

#ifdef NDEBUG
# define IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD
#else
# define IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD [[maybe_unused]] check_invariant_guard invariant_guard_{this};
#endif

public:
    using item_type = T;
    using index_type = IndexT;
    using run_container_type = RunContainerT;
    using allocator_type = run_container_type::allocator_type;
    using run_ref = run_length_run_ref<T, IndexT>;

    using value_type = IndexedValuePairTT<IndexT, T>;
    using const_reference = IndexedValuePairTT<IndexT, T const&>;
    using reference = const_reference;

    using const_iterator = iterator_impl;
    using iterator = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr run_length_sequence() = default;

    constexpr explicit run_length_sequence(allocator_type const& alloc) noexcept
        : runs_(alloc)
    {}

    template<container::compatible_range<T> R>
    constexpr run_length_sequence(std::from_range_t, R&& r, allocator_type const& alloc = allocator_type())
        : runs_(alloc)
    {
        for (auto&& elem : r) {
            this->emplace_back(std::forward<decltype(elem)>(elem));
        }
    }

    template<container::compatible_iterator<T> It, std::sentinel_for<It> Se>
    constexpr run_length_sequence(It it, Se se, allocator_type const& alloc = allocator_type())
        : runs_(alloc)
    {
        for (; it != se; ++it) {
            this->emplace_back(*it);
        }
    }

    constexpr run_length_sequence(std::initializer_list<T> il, allocator_type const& alloc = allocator_type())
        : run_length_sequence(il.begin(), il.end(), alloc)
    {}

    [[nodiscard]] constexpr const_iterator begin() const noexcept { check_range_concepts(); return const_iterator{std::ranges::begin(runs_), offsets_.begin()}; }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        check_range_concepts();
        if (offsets_.empty()) {
            assert(std::ranges::empty(runs_));
            return const_iterator{std::ranges::end(runs_), offsets_.end()};
        } else {
            assert(offsets_.size() >= 2);
            return const_iterator{std::ranges::end(runs_), std::prev(offsets_.end())};
        }
    }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        static_assert(std::ranges::sized_range<RunContainerT>);
        assert(std::ranges::empty(runs_) == offsets_.empty());
        return offsets_.empty();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        static_assert(std::ranges::sized_range<RunContainerT>);
        if (offsets_.empty()) return 0uz;
        assert(!std::ranges::empty(runs_));
        assert(offsets_.size() >= 2);
        return static_cast<size_type>(offsets_.back());
    }

    constexpr void clear() noexcept
        requires requires(RunContainerT& runs) { runs.clear(); }
    {
        static_assert(req::Cpp17Destructible<T>);
        IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD
        runs_.clear();
        offsets_.clear();
    }

    [[nodiscard]] constexpr const_reference front() const noexcept IRIS_LIFETIMEBOUND
    {
        assert(!this->empty());
        return {static_cast<IndexT>(0u), container::front(runs_)};
    }
    [[nodiscard]] constexpr const_reference back() const noexcept IRIS_LIFETIMEBOUND
    {
        assert(!this->empty());
        return {static_cast<IndexT>(offsets_.back() - static_cast<IndexT>(1u)), container::back(runs_)};
    }

    template<class U>
        requires std::constructible_from<T, U> && req::half_equality_comparable<T, U>
    constexpr const_reference emplace_back(U&& value) IRIS_LIFETIMEBOUND
    {
        check_range_concepts();
        static_assert(std::equality_comparable<T>);
        IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD

        if (offsets_.empty()) {
            return this->emplace_back_on_empty(std::forward<U>(value));

        } else {
            assert(!std::ranges::empty(runs_));
            assert(offsets_.size() >= 2);
            if (offsets_.back() == max_size()) {
                throwf<std::length_error>("run_length_sequence capacity exceeded");
            }

            if (container::back(std::as_const(runs_)) == std::as_const(value)) {
                // Equivalent element already exists; no need to insert.
                return {offsets_.back()++, container::back(runs_)};
            }
            // Need to insert new element
            auto const new_pos = offsets_.back();
            offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
            [[maybe_unused]] ofs_insertion_guard<false> ofs_insertion_guard{this};
            auto& elem = container::append_return(runs_, std::forward<U>(value));
            ofs_insertion_guard.clear();
            return {new_pos, elem};
        }
    }

    template<class... Args>
        requires std::constructible_from<T, Args...>
    constexpr const_reference emplace_back(Args&&... args) IRIS_LIFETIMEBOUND
    {
        check_range_concepts();
        static_assert(std::equality_comparable<T>);
        IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD

        if (offsets_.empty()) {
            return this->emplace_back_on_empty(std::forward<Args>(args)...);

        } else {
            assert(!std::ranges::empty(runs_));
            assert(offsets_.size() >= 2);
            if (offsets_.back() == max_size()) {
                throwf<std::length_error>("run_length_sequence capacity exceeded");
            }
            if constexpr (requires { container::erase_back(runs_); }) {
                auto& elem = container::append_return(runs_, std::forward<Args>(args)...);
                [[maybe_unused]] elem_insertion_guard elem_insertion_guard{this};

                auto const prev_it = std::ranges::prev(std::ranges::end(runs_), 2);

                if (std::as_const(*prev_it) == std::as_const(elem)) {
                    // Equivalent element already exists; no need to insert.
                    elem_insertion_guard.clear();
                    container::erase_back(runs_);
                    return {offsets_.back()++, container::back(runs_)};
                }

                // Need to insert new element
                auto const new_pos = offsets_.back();
                offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
                elem_insertion_guard.clear();
                return {new_pos, elem};

            } else {
                T temp(std::forward<Args>(args)...);
                if (container::back(std::as_const(runs_)) == std::as_const(temp)) {
                    // Equivalent element already exists; no need to insert.
                    return {offsets_.back()++, container::back(runs_)};
                }
                // Need to insert new element
                auto const new_pos = offsets_.back();
                offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
                [[maybe_unused]] ofs_insertion_guard<false> ofs_insertion_guard{this};
                auto& elem = container::append_return(runs_, std::move(temp));
                ofs_insertion_guard.clear();
                return {new_pos, elem};
            }
        }
    }

    // Offers heterogeneous comparison benefit
    template<class U>
        requires
            (!std::same_as<std::remove_cvref_t<U>, T>) &&
            std::constructible_from<T, U>
    constexpr const_reference push_back(U&& value_like) IRIS_LIFETIMEBOUND
    {
        return this->emplace_back(std::forward<U>(value_like));
    }
    constexpr const_reference push_back(T const& value) IRIS_LIFETIMEBOUND
    {
        return this->emplace_back(value);
    }
    constexpr const_reference push_back(T&& value) IRIS_LIFETIMEBOUND
    {
        return this->emplace_back(std::move(value));
    }

    constexpr void pop_back()
        noexcept(noexcept(container::erase_back(runs_)))
        requires requires(RunContainerT& runs) { container::erase_back(runs); }
    {
        static_assert(req::Cpp17Destructible<T>);
        IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD
        assert(!this->empty());
        assert(offsets_.size() >= 2);

        auto const n = offsets_.size();
        if (offsets_[n - 1] - offsets_[n - 2] == static_cast<IndexT>(1u)) {
            container::erase_back(runs_);
            offsets_.pop_back();
            if (offsets_.size() == 1) {
                offsets_.clear(); // remove sentinel
            }

        } else {
            --offsets_.back();
        }
    }

    constexpr void shrink_to_fit()
    {
        if constexpr (container::has_shrink_to_fit<RunContainerT>) {
            runs_.shrink_to_fit();
        }
        offsets_.shrink_to_fit();
    }

    template<container::compatible_range<T> R>
    constexpr void append_range(R&& r)
    {
        for (auto it = std::ranges::begin(r); it != std::ranges::end(r); ++it) {
            this->emplace_back(*it);
        }
    }

    // ---------------------------------------------------------------------

    [[nodiscard]] constexpr RunContainerT const& runs() const noexcept
    {
        return runs_;
    }

    [[nodiscard]] constexpr size_type run_count() const noexcept
    {
        static_assert(std::ranges::sized_range<RunContainerT>);
        return static_cast<size_type>(std::ranges::size(runs_));
    }

    [[nodiscard]] constexpr auto run_view() const noexcept
    {
        check_range_concepts();
        return std::views::zip_transform(
            [](T const& value, auto const& bounds) noexcept -> run_ref {
                return {value, interval<IndexT>{std::get<0>(bounds), std::get<1>(bounds)}};
            },
            runs_, offsets_ | std::views::pairwise
        );
    }

    // ---------------------------------------------------------------------

    // Returns an iterator to the element at logical position `pos`, or `end()`
    // when `pos >= size()`.
    //
    // Complexity: O(log run_count()) for locating the run, plus the cost of
    // advancing an iterator of `RunContainerT` by the run index (O(1) when
    // `RunContainerT` is random access).
    [[nodiscard]] constexpr const_iterator nth(IndexT pos) const noexcept
    {
        check_range_concepts();
        if (pos >= this->size()) return this->end();

        auto const ofs_next = std::ranges::upper_bound(offsets_, pos);
        assert(ofs_next != offsets_.begin());
        assert(ofs_next != offsets_.end());
        auto const ofs_it = std::ranges::prev(ofs_next);

        auto const run_index = ofs_it - offsets_.begin();
        auto const value_it = std::ranges::next(std::ranges::begin(runs_), run_index);

        return const_iterator{value_it, ofs_it, static_cast<IndexT>(pos - *ofs_it)};
    }

    // ---------------------------------------------------------------------

    constexpr void swap(run_length_sequence& other)
        noexcept(std::is_nothrow_swappable_v<RunContainerT> && std::is_nothrow_swappable_v<offsets_type>)
    {
        using std::swap;
        swap(runs_, other.runs_);
        swap(offsets_, other.offsets_);
    }

private:
    static constexpr void check_range_concepts() noexcept
    {
        static_assert(std::ranges::bidirectional_range<RunContainerT>);
        static_assert(std::same_as<std::ranges::range_value_t<RunContainerT>, T>);
        static_assert(std::same_as<std::ranges::range_reference_t<RunContainerT>, T&>);
        static_assert(std::same_as<std::ranges::range_reference_t<RunContainerT const>, T const&>);

        static_assert(container::back_pushable<RunContainerT, T>);
        static_assert(container::back_accessible<RunContainerT>);
        static_assert(container::front_accessible<RunContainerT>);

        static_assert(requires (RunContainerT& runs) {
            { container::back(runs) } -> std::same_as<T&>;
            { container::front(runs) } -> std::same_as<T&>;
        });
    }

    template<class... Args>
    constexpr const_reference emplace_back_on_empty(Args&&... args) IRIS_LIFETIMEBOUND
    {
        assert(this->empty());
        [[maybe_unused]] ofs_insertion_guard<true> ofs_insertion_guard{this};
        offsets_.emplace_back(static_cast<IndexT>(0u));
        offsets_.emplace_back(static_cast<IndexT>(1u)); // sentinel
        auto& elem = container::append_return(runs_, std::forward<Args>(args)...);
        ofs_insertion_guard.clear();
        return {static_cast<IndexT>(0u), elem};
    }

    template<bool WasEmpty>
    struct [[nodiscard]] ofs_insertion_guard
    {
        constexpr explicit ofs_insertion_guard(run_length_sequence* self) noexcept
            : self_(self)
        {}

        constexpr void clear() noexcept
        {
            self_ = nullptr;
        }

        constexpr ~ofs_insertion_guard() noexcept
        {
            if (!self_) return;
            if constexpr (WasEmpty) {
                self_->offsets_.clear();
            } else {
                self_->offsets_.pop_back();
            }
        }

    private:
        run_length_sequence* self_;
    };
    template<bool WasEmpty>
    friend struct ofs_insertion_guard;

    struct [[nodiscard]] elem_insertion_guard
    {
        constexpr explicit elem_insertion_guard(run_length_sequence* self) noexcept
            : self_(self)
        {}

        constexpr void clear() noexcept
        {
            self_ = nullptr;
        }

        constexpr ~elem_insertion_guard() noexcept
        {
            if (!self_) return;
            container::erase_back(self_->runs_);
        }

    private:
        run_length_sequence* self_;
    };
    friend struct elem_insertion_guard;

#ifndef NDEBUG
    struct [[nodiscard]] check_invariant_guard
    {
        constexpr explicit check_invariant_guard(run_length_sequence const* self) noexcept
            : self_(self)
        {}

        constexpr ~check_invariant_guard() noexcept
        {
            auto const& offsets = self_->offsets_;
            auto const& runs = self_->runs_;
            if (offsets.empty()) {
                assert(std::ranges::empty(runs));
                return;
            }
            assert(offsets.size() >= 2);
            assert(offsets.size() == std::ranges::size(runs) + 1);
            assert(offsets.front() == static_cast<IndexT>(0u));
            auto const n = offsets.size();
            assert(offsets[n - 2] < offsets[n - 1]);

            // Adjacent runs must differ
            if (n >= 3) {
                auto const last = std::ranges::prev(std::ranges::end(runs));
                auto const before_last = std::ranges::prev(last);
                assert(!(*before_last == *last));
            }
        }

    private:
        run_length_sequence const* self_;
    };
    friend struct check_invariant_guard;
#endif

    friend struct detail::run_length_sequence_comp;

    RunContainerT runs_;
    offsets_type offsets_;

#undef IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD
};

template<class T, class IndexT, template<class, class> class IndexedValuePairTT, class RunContainerT>
constexpr void swap(
    run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT>& a,
    run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT>& b
)
    noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}


namespace detail {

struct run_length_sequence_comp
{
    template<class T, class IndexT, template<class, class> class IndexedValuePairTT, class RunContainerT>
    [[nodiscard]] static constexpr bool
    equals(
        run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT> const& a,
        run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT> const& b
    )
        noexcept(noexcept(std::declval<T const&>() == std::declval<T const&>()))
    {
        static_assert(std::equality_comparable<T>);
        static_assert(std::equality_comparable<RunContainerT>);
        // Adjacent runs never compare equal (class invariant), so the representation
        // is canonical and representational equality is logical equality.
        return a.offsets_ == b.offsets_ && a.runs_ == b.runs_;
    }
};

} // detail

template<class T, class IndexT, template<class, class> class IndexedValuePairTT, class RunContainerT>
[[nodiscard]] constexpr bool
operator==(
    run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT> const& a,
    run_length_sequence<T, IndexT, IndexedValuePairTT, RunContainerT> const& b
)
    noexcept(noexcept(detail::run_length_sequence_comp::equals(a, b)))
{
    return detail::run_length_sequence_comp::equals(a, b);
}

} // iris

#endif

#ifndef IRIS_ZZ_RUN_LENGTH_SEQUENCE_HPP
#define IRIS_ZZ_RUN_LENGTH_SEQUENCE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/indexed_value.hpp>

#include <iris/error/throwf.hpp>

#include <iris/default_init_allocator.hpp>
#include <iris/ranges.hpp>
#include <iris/requirements.hpp>
#include <iris/type_traits.hpp>

#include <vector>
#include <ranges>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <concepts>
#include <type_traits>

#include <cstddef>
#include <cassert>

namespace iris::detail {
struct run_length_sequence_comp;
} // iris::detail

namespace iris {

// A compressed bidirectional container that holds only one instance of `T`
// per each adjacent equivalent elements.
//
// Iterator invalidation does not follow `RunContainerT`: any insertion that starts a
// new segment may invalidate all iterators, regardless of `RunContainerT`. Reference
// stability of the elements follows `RunContainerT`.
//
// `run_length_sequence` can be either mutable or immutable, depending on the user's
// preference. By default, `run_length_sequence` allows access to the mutable reference
// of the underlying elements. When the user chooses to modify the element,
// `run_length_sequence` inevitably becomes "uncompressed," as the adjacent equivalence
// would no longer hold. However, `run_length_sequence` still can be used even in such
// partially-altered state, since all other invariants shall remain intact.
//
// Supports heterogeneous comparison iff `T` supports heterogeneous comparison
// for arbitrary type `U`:
//   - Equivalence is checked by `operator==(T const& e, U const& value)`, where
//     `e` is the reference to the existing item and `value` is the object
//     that is passed to the relevant member function.
//
//   - For example, when `value` is a parameter given to `run_length_sequence<T>::
//     emplace_back(U&& value)`, the list directly compares `value` with the
//     existing element without creating a temporary `T` object. Then, if
//     the comparison failed, a new instance of `T` will be directly inserted
//     into `RunContainerT` via `std::forward<U>(value)`.
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
// The semantics described in the previous section applies similarly to `emplace_front`.
//
// Internal structure is maintained with two independent lists, where the first
// list holds one element instance per each segment and the other list holds the
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
    class RunContainerT = std::vector<T, default_init_allocator<T>>
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

    template<bool IsConst>
    struct iterator_impl
    {
    private:
        using value_iterator = std::ranges::iterator_t<std::conditional_t<IsConst, RunContainerT const, RunContainerT>>;
        using offset_iterator = offsets_type::const_iterator;

    public:
        using iterator_concept = std::bidirectional_iterator_tag;
        using iterator_category = std::input_iterator_tag; // Our reference type is proxy, so this can only be input

        using value_type = indexed_value<
            IndexT,
            std::iter_value_t<value_iterator>
        >;
        using reference = indexed_value<
            IndexT,
            std::iter_reference_t<value_iterator>
        >;
        using rvalue_reference = indexed_value<
            IndexT,
            std::iter_rvalue_reference_t<value_iterator>
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

        [[nodiscard]] constexpr operator iterator_impl<true>() const noexcept requires (IsConst == false)
        {
            return iterator_impl<true>{value_it_, ofs_it_, rel_pos_};
        }

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

        [[nodiscard]] friend constexpr auto iter_move(iterator_impl const& it)
            noexcept(noexcept(std::ranges::iter_move(it.value_it_)))
        {
            return rvalue_reference{
                it.index(),
                std::ranges::iter_move(it.value_it_)
            };
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

    using value_type = indexed_value<IndexT, T>;
    using reference = indexed_value<IndexT, T&>;
    using const_reference = indexed_value<IndexT, T const&>;

    using iterator = iterator_impl<false>;
    using const_iterator = iterator_impl<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[nodiscard]] constexpr iterator begin() noexcept { check_range_concepts(); return iterator{std::ranges::begin(runs_), offsets_.begin()}; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { check_range_concepts(); return const_iterator{std::ranges::begin(runs_), offsets_.begin()}; }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        check_range_concepts();
        if (offsets_.empty()) {
            assert(std::ranges::empty(runs_));
            return iterator{std::ranges::end(runs_), offsets_.end()};
        } else {
            assert(offsets_.size() >= 2);
            return iterator{std::ranges::end(runs_), std::prev(offsets_.end())};
        }
    }
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

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
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

    [[nodiscard]] constexpr size_type run_count() const noexcept
    {
        static_assert(std::ranges::sized_range<RunContainerT>);
        return static_cast<size_type>(std::ranges::size(runs_));
    }

    constexpr void clear() noexcept
        requires requires(RunContainerT& list) { list.clear(); }
    {
        static_assert(req::Cpp17Destructible<T>);
        IRIS_ZZ_RUN_LENGTH_SEQUENCE_INVARIANT_GUARD
        runs_.clear();
        offsets_.clear();
    }

    template<class U>
        requires std::constructible_from<T, U> && req::half_equality_comparable<T, U>
    constexpr reference emplace_back(U&& value) IRIS_LIFETIMEBOUND
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

            if (ranges::back(std::as_const(runs_)) == std::as_const(value)) {
                // Equivalent element already exists; no need to insert.
                return {offsets_.back()++, ranges::back(runs_)};
            }
            // Need to insert new element
            auto const new_pos = offsets_.back();
            offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
            [[maybe_unused]] ofs_insertion_guard<true, false> ofs_insertion_guard{this};
            auto& elem = ranges::emplace_back_ref(runs_, std::forward<U>(value));
            ofs_insertion_guard.clear();
            return {new_pos, elem};
        }
    }

    template<class... Args>
        requires std::constructible_from<T, Args...>
    constexpr reference emplace_back(Args&&... args) IRIS_LIFETIMEBOUND
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
            if constexpr (requires { ranges::weak_pop_back(runs_); }) {
                auto& elem = ranges::emplace_back_ref(runs_, std::forward<Args>(args)...);
                [[maybe_unused]] elem_insertion_guard<true> elem_insertion_guard{this};

                auto const prev_it = std::ranges::prev(std::ranges::end(runs_), 2);

                if (std::as_const(*prev_it) == std::as_const(elem)) {
                    // Equivalent element already exists; no need to insert.
                    elem_insertion_guard.clear();
                    ranges::weak_pop_back(runs_);
                    return {offsets_.back()++, ranges::back(runs_)};
                }

                // Need to insert new element
                auto const new_pos = offsets_.back();
                offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
                elem_insertion_guard.clear();
                return {new_pos, elem};

            } else {
                T temp(std::forward<Args>(args)...);
                if (ranges::back(std::as_const(runs_)) == std::as_const(temp)) {
                    // Equivalent element already exists; no need to insert.
                    return {offsets_.back()++, ranges::back(runs_)};
                }
                // Need to insert new element
                auto const new_pos = offsets_.back();
                offsets_.emplace_back(new_pos + static_cast<IndexT>(1u)); // new sentinel
                [[maybe_unused]] ofs_insertion_guard<true, false> ofs_insertion_guard{this};
                auto& elem = ranges::emplace_back_ref(runs_, std::move(temp));
                ofs_insertion_guard.clear();
                return {new_pos, elem};
            }
        }
    }

private:
    static constexpr void check_range_concepts() noexcept
    {
        static_assert(std::ranges::bidirectional_range<RunContainerT>);
        static_assert(std::same_as<std::ranges::range_value_t<RunContainerT>, T>);
        static_assert(std::same_as<std::ranges::range_reference_t<RunContainerT>, T&>);
        static_assert(std::same_as<std::ranges::range_reference_t<RunContainerT const>, T const&>);

        static_assert(requires (RunContainerT& list) {
            { list.back() } -> std::same_as<T&>;
            { list.front() } -> std::same_as<T&>;
        });
    }

    template<class... Args>
    constexpr reference emplace_back_on_empty(Args&&... args) IRIS_LIFETIMEBOUND
    {
        assert(this->empty());
        [[maybe_unused]] ofs_insertion_guard<true, true> ofs_insertion_guard{this};
        offsets_.emplace_back(static_cast<IndexT>(0u));
        offsets_.emplace_back(static_cast<IndexT>(1u)); // sentinel
        auto& elem = ranges::emplace_back_ref(runs_, std::forward<Args>(args)...);
        ofs_insertion_guard.clear();
        return {static_cast<IndexT>(0u), elem};
    }

    template<bool IsBack, bool WasEmpty>
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
            if constexpr (IsBack) {
                if constexpr (WasEmpty) {
                    self_->offsets_.clear();
                } else {
                    self_->offsets_.pop_back();
                }
            } else {
                static_assert(false, "sorry, not implemented");
            }
        }

    private:
        run_length_sequence* self_;
    };
    template<bool IsBack, bool WasEmpty>
    friend struct ofs_insertion_guard;

    template<bool IsBack>
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
            if constexpr (IsBack) {
                ranges::weak_pop_back(self_->runs_);
            } else {
                static_assert(false, "sorry, not implemented");
            }
        }

    private:
        run_length_sequence* self_;
    };
    template<bool IsBack>
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


namespace detail {

struct run_length_sequence_comp
{
    template<class T, class IndexT, class RunContainerT>
    [[nodiscard]] static constexpr bool equals(run_length_sequence<T, IndexT, RunContainerT> const& a, run_length_sequence<T, IndexT, RunContainerT> const& b)
        noexcept(noexcept(std::declval<T const&>() == std::declval<T const&>()))
    {
        static_assert(std::equality_comparable<T>);

        if (a.size() != b.size()) return false;
        if (a.empty()) return true;

        auto ra = std::ranges::begin(a.runs_);
        auto rb = std::ranges::begin(b.runs_);
        auto oa = a.offsets_.begin(); // current run's start; *(oa + 1) is its end
        auto ob = b.offsets_.begin();
        auto const ra_end = std::ranges::end(a.runs_);

        while (true) {
            if (!(*ra == *rb)) return false;

            auto const next_a = *(oa + 1);
            auto const next_b = *(ob + 1);

            if (next_a == next_b) {
                ++ra; ++oa;
                ++rb; ++ob;

            } else if (next_a < next_b) {
                ++ra; ++oa;

            } else {
                ++rb; ++ob;
            }

            if (ra == ra_end) {
                // Logical sizes are equal, so b must be exhausted here too.
                assert(rb == std::ranges::end(b.runs_));
                return true;
            }
        }
    }
};

} // detail

template<class T, class IndexT, class RunContainerT>
[[nodiscard]] constexpr bool operator==(run_length_sequence<T, IndexT, RunContainerT> const& a, run_length_sequence<T, IndexT, RunContainerT> const& b)
    noexcept(noexcept(detail::run_length_sequence_comp::equals(a, b)))
{
    return detail::run_length_sequence_comp::equals(a, b);
}

} // iris

#endif

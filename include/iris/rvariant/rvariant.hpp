#ifndef IRIS_ZZ_RVARIANT_RVARIANT_HPP
#define IRIS_ZZ_RVARIANT_RVARIANT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/rvariant/detail/variant_storage.hpp>
#include <iris/rvariant/detail/visit.hpp>
#include <iris/rvariant/detail/recursive_traits.hpp>
#include <iris/rvariant/variant_helper.hpp>  // IWYU pragma: export
#include <iris/rvariant/subset.hpp>
#include <iris/rvariant/rvariant_fwd.hpp>

#include <iris/hash/FNV_hash.hpp>

#include <iris/compare.hpp>
#include <iris/type_traits.hpp>
#include <iris/type_list.hpp>
#include <iris/hash.hpp>

#include <concepts>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <variant>
#include <memory>

#include <cstddef>
#include <cassert>

namespace iris {

namespace detail {

template<class T, class U>
struct check_recursive_wrapper_duplicate_impl : std::true_type {};

template<class T, class U>
    requires
        (!std::same_as<T, U>) &&
        (is_recursive_wrapper_v<T> || is_recursive_wrapper_v<U>) &&
        std::same_as<unwrap_recursive_t<T>, unwrap_recursive_t<U>>
struct check_recursive_wrapper_duplicate_impl<T, U>
    : std::false_type
{
    // ReSharper disable once CppStaticAssertFailure
    static_assert(
        false,
        "rvariant cannot contain both `T` and `recursive_wrapper` of `T` "
        "([rvariant.rvariant.general])."
    );
};

template<class T, class... Ts>
struct check_recursive_wrapper_duplicate : std::true_type {};

template<class T, class... Ts> requires (sizeof...(Ts) > 0)
struct check_recursive_wrapper_duplicate<T, T, Ts...>
    : std::conjunction<check_recursive_wrapper_duplicate_impl<T, Ts>...>
{};

template<class T, class List>
struct non_wrapped_exactly_once : exactly_once<T, List>
{
    static_assert(
        !is_recursive_wrapper_v<T>,
        "Constructing a `recursive_wrapper` alternative with its full type as the tag is "
        "prohibited to avoid confusion; just specify `T` instead."
    );
};

template<class T, class List>
constexpr bool non_wrapped_exactly_once_v = non_wrapped_exactly_once<T, List>::value;


template<class T, class Variant>
struct exactly_once_index
{
    static constexpr std::size_t value = find_index_exactly_once_v<T, typename Variant::unwrapped_types>;
    static_assert(value != find_npos, "`T` or `recursive_wrapper<T>` or `recursive_wrapper_alloca<T, A>` must occur exactly once in Ts...");
};

template<class T, class Variant>
inline constexpr std::size_t exactly_once_index_v = exactly_once_index<T, Variant>::value;


// The alternative `T` can be both constructed and assigned from `U`
// https://eel.is/c++draft/variant.assign
template<class T, class U>
concept rvariant_alternative_assignable = std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>;

template<class T, class U>
concept rvariant_alternative_nothrow_assignable = std::is_nothrow_constructible_v<T, U> && std::is_nothrow_assignable_v<T&, U>;


template<class R, class Compare, class... Ts>
struct relops_visitor;

// https://eel.is/c++draft/variant.ctor
// https://eel.is/c++draft/variant.dtor
// https://eel.is/c++draft/variant.assign
template<class... Ts>
concept rvariant_trivially_copy_constructible = (std::is_trivially_copy_constructible_v<Ts> && ...);

template<class... Ts>
concept rvariant_copy_constructible = (std::is_copy_constructible_v<Ts> && ...);

template<class... Ts>
concept rvariant_trivially_move_constructible = (std::is_trivially_move_constructible_v<Ts> && ...);

template<class... Ts>
concept rvariant_move_constructible = (std::is_move_constructible_v<Ts> && ...);

template<class... Ts>
concept rvariant_trivially_copy_assignable = ((std::is_trivially_destructible_v<Ts> && std::is_trivially_copy_constructible_v<Ts> && std::is_trivially_copy_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_copy_assignable = ((std::is_copy_constructible_v<Ts> && std::is_copy_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_trivially_move_assignable = ((std::is_trivially_destructible_v<Ts> && std::is_trivially_move_constructible_v<Ts> && std::is_trivially_move_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_move_assignable = ((std::is_move_constructible_v<Ts> && std::is_move_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_nothrow_copy_assignable = ((std::is_nothrow_copy_constructible_v<Ts> && std::is_nothrow_copy_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_nothrow_move_assignable = ((std::is_nothrow_move_constructible_v<Ts> && std::is_nothrow_move_assignable_v<Ts>) && ...);

template<class... Ts>
concept rvariant_trivially_destructible = (std::is_trivially_destructible_v<Ts> && ...);

// Selects the private constructor that every other constructor delegates to
struct primary_construct_t
{
    constexpr explicit primary_construct_t() = default;
};

inline constexpr primary_construct_t primary_construct{};

#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
// IntelliSense and ReSharper ignore the constraints of assignment operators in some places:
//   - A user-provided assignment operator is counted as eligible even when its constraints are not
//     satisfied, so `std::is_trivially_copyable` evaluates to false even when every alternative is
//     trivial. Swapping the parameter type for these placeholders makes such a declaration stop
//     being an assignment operator at all ([class.copy.assign]/1).
//   - A `= default` assignment operator whose constraints are not satisfied makes them declare an
//     implicit move assignment, and the implicit assignment operators of a derived class are
//     computed from it. An additional declaration that is selected exactly when neither of the
//     other two is prevents this: a deleted copy assignment, and a move assignment that forwards to
//     the copy assignment (or a deleted one if the copy assignment is not usable either), because
//     a move assignment that does not participate must fall back to the copy assignment.
// Constructors and destructors are not affected.
struct rvariant_not_copy_assignable { rvariant_not_copy_assignable() = delete; };
struct rvariant_not_move_assignable { rvariant_not_move_assignable() = delete; };
struct rvariant_not_move_assignable_fallback { rvariant_not_move_assignable_fallback() = delete; };
#endif

template<class... Ts>
[[nodiscard]] constexpr rvariant<Ts...> make_valueless() noexcept
{
    static_assert(!is_never_valueless_v<Ts...>);
    return rvariant<Ts...>{valueless};
}

}  // detail


template<class... Ts>
class rvariant
{
    static_assert(std::conjunction_v<detail::check_recursive_wrapper_duplicate<Ts, Ts...>...>);
    static_assert((req::Cpp17Destructible<Ts> && ...), "All types shall meet the Cpp17Destructible requirements ([variant.variant.general]).");
    static_assert(sizeof...(Ts) > 0, "A variant with no template arguments shall not be instantiated ([variant.variant.general]).");

    using unwrapped_types = type_list<unwrap_recursive_t<Ts>...>;

    static constexpr bool need_destructor_call = !std::conjunction_v<std::is_trivially_destructible<Ts>...>;

    using storage_type = detail::make_variadic_union_t<Ts...>;
    static constexpr bool never_valueless = storage_type::never_valueless;

    template<class Self>
    using like_rvariant_t = std::conditional_t<
        std::is_rvalue_reference_v<Self&&>,
        std::conditional_t<
            std::is_const_v<std::remove_reference_t<Self>>,
            rvariant<Ts...> const,
            rvariant<Ts...>
        >&&,
        std::conditional_t<
            std::is_const_v<std::remove_reference_t<Self>>,
            rvariant<Ts...> const,
            rvariant<Ts...>
        >&
    >;

IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
    // Primary constructor; every other constructor that holds a value delegates to this
    template<std::size_t I, class... Args>
        requires std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>
    constexpr explicit rvariant(detail::primary_construct_t, std::in_place_index_t<I>, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>)
        : storage_{} // valueless
        // Constructing the alternative inside this initializer stores the index only after the construction succeeds.
        // This sequential guarantee enables some important optimizations on certain compilers.
        , index_{(detail::alternative_constructor<I>::construct(storage_, std::forward<Args>(args)...), static_cast<detail::variant_index_t<sizeof...(Ts)>>(I))}
    {}
IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END

public:
    constexpr rvariant(rvariant const&) = default;

    constexpr rvariant(rvariant const& w)
        noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
        requires
            (!detail::rvariant_trivially_copy_constructible<Ts...>) &&
            detail::rvariant_copy_constructible<Ts...>
    {
        constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>;
        w.template raw_visit<is_noexcept>([this]<std::size_t j, class T>(std::in_place_index_t<j>, [[maybe_unused]] T const& alt)
            noexcept(is_noexcept)
        {
            if constexpr (j != std::variant_npos) {
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_BEGIN
                detail::alternative_constructor<j>::construct(storage_, alt);
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_END
                index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(j);
            } else {
                index_ = detail::variant_npos<sizeof...(Ts)>;
            }
        });
    }

    constexpr rvariant(rvariant&&) = default;

    constexpr rvariant(rvariant&& w)
        noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>)
        requires
            (!detail::rvariant_trivially_move_constructible<Ts...>) &&
            detail::rvariant_move_constructible<Ts...>
    {
        constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>;
        std::move(w).template raw_visit<is_noexcept>([this]<std::size_t j, class T>(std::in_place_index_t<j>, [[maybe_unused]] T&& alt)
            noexcept(is_noexcept)
        {
            if constexpr (j != std::variant_npos) {
                static_assert(std::is_rvalue_reference_v<T&&>);
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_BEGIN
                detail::alternative_constructor<j>::construct(storage_, std::move(alt)); // NOLINT(bugprone-move-forwarding-reference)
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_END
                index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(j);
            } else {
                index_ = detail::variant_npos<sizeof...(Ts)>;
            }
        });
    }

    constexpr rvariant& operator=(rvariant const&)
        requires detail::rvariant_trivially_copy_assignable<Ts...>
        = default;

    constexpr rvariant& operator=(
#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
        std::conditional_t<
            (!detail::rvariant_trivially_copy_assignable<Ts...>) &&
            detail::rvariant_copy_assignable<Ts...>,
            rvariant,
            detail::rvariant_not_copy_assignable
        > const& rhs
#else
        rvariant const& rhs
#endif
    )
        noexcept(detail::rvariant_nothrow_copy_assignable<Ts...>)
        requires
            (!detail::rvariant_trivially_copy_assignable<Ts...>) &&
            detail::rvariant_copy_assignable<Ts...>
    {
    IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
        constexpr bool is_noexcept = detail::rvariant_nothrow_copy_assignable<Ts...>;
        rhs.template raw_visit<is_noexcept>([this]<std::size_t j, class T>(std::in_place_index_t<j>, T const& rhs_alt)
            noexcept(is_noexcept)
        {
            if constexpr (j == std::variant_npos) {
                (void)rhs_alt;
                visit_reset();
            } else {
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_BEGIN
                if (index_ == j) {
                    detail::raw_get<j>(storage()) = rhs_alt;
                } else {
                    // CC(noexcept) && MC(throw)    => A
                    // CC(noexcept) && MC(noexcept) => A
                    // CC(throw)    && MC(throw)    => A
                    // CC(throw)    && MC(noexcept) => B
                    if constexpr (std::is_nothrow_copy_constructible_v<T> || !std::is_nothrow_move_constructible_v<T>) {
                        reset_construct<j>(rhs_alt);  // A
                    } else {
                        T tmp(rhs_alt);
                        reset_construct<j>(std::move(tmp)); // B
                    }
                }
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_END
            }
        });
    IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END
        return *this;
    }

#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
    constexpr rvariant& operator=(rvariant const&)
        requires
            (!detail::rvariant_trivially_copy_assignable<Ts...>) &&
            (!detail::rvariant_copy_assignable<Ts...>)
        = delete;
#endif

    constexpr rvariant& operator=(rvariant&&)
        requires detail::rvariant_trivially_move_assignable<Ts...>
        = default;

    constexpr rvariant& operator=(
#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
        std::conditional_t<
            (!detail::rvariant_trivially_move_assignable<Ts...>) &&
            detail::rvariant_move_assignable<Ts...>,
            rvariant,
            detail::rvariant_not_move_assignable
        >&& rhs
#else
        rvariant&& rhs
#endif
    )
        noexcept(detail::rvariant_nothrow_move_assignable<Ts...>)
        requires
            (!detail::rvariant_trivially_move_assignable<Ts...>) &&
            detail::rvariant_move_assignable<Ts...>
    {
        constexpr bool is_noexcept = detail::rvariant_nothrow_move_assignable<Ts...>;
        std::move(rhs).template raw_visit<is_noexcept>([this]<std::size_t j, class T>(std::in_place_index_t<j>, [[maybe_unused]] T&& rhs_alt)
            noexcept(is_noexcept)
        {
            if constexpr (j == std::variant_npos) {
                (void)rhs_alt;
                visit_reset();
            } else {
                static_assert(std::is_rvalue_reference_v<T&&>);

            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_BEGIN
                if (index_ == j) {
                    detail::raw_get<j>(storage()) = std::move(rhs_alt); // NOLINT(bugprone-move-forwarding-reference)
                } else {
                    reset_construct<j>(std::move(rhs_alt)); // NOLINT(bugprone-move-forwarding-reference)
                }
            IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_END
            }
        });
        return *this;
    }

#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
    constexpr rvariant& operator=(
        std::conditional_t<
            (!detail::rvariant_trivially_move_assignable<Ts...>) &&
            (!detail::rvariant_move_assignable<Ts...>) &&
            detail::rvariant_copy_assignable<Ts...>,
            rvariant,
            detail::rvariant_not_move_assignable_fallback
        >&& rhs
    )
        noexcept(detail::rvariant_nothrow_copy_assignable<Ts...>)
        requires
            (!detail::rvariant_trivially_move_assignable<Ts...>) &&
            (!detail::rvariant_move_assignable<Ts...>) &&
            detail::rvariant_copy_assignable<Ts...>
    {
        return *this = static_cast<rvariant const&>(rhs);
    }

    constexpr rvariant& operator=(rvariant&&)
        requires
            (!detail::rvariant_trivially_move_assignable<Ts...>) &&
            (!detail::rvariant_move_assignable<Ts...>) &&
            (!detail::rvariant_copy_assignable<Ts...>)
        = delete;
#endif

    constexpr ~rvariant() = default;

    constexpr ~rvariant() noexcept
        requires (!detail::rvariant_trivially_destructible<Ts...>)
    {
        visit_destroy();
    }

    [[nodiscard]] constexpr bool valueless_by_exception() const noexcept
    {
        if constexpr (never_valueless) {
            assert(index_ != detail::variant_npos<sizeof...(Ts)>);
            return false;
        } else {
            return index_ == detail::variant_npos<sizeof...(Ts)>;
        }
    }
    [[nodiscard]] constexpr std::size_t index() const noexcept { return static_cast<std::size_t>(index_); }

private:
    // internal
    template<std::size_t I>
    constexpr void destroy() noexcept
    {
        if constexpr (need_destructor_call) {
            // ReSharper disable once CppTypeAliasNeverUsed
            using T = IRIS_PACK_INDEXING(I, Ts...);
            auto&& alt = detail::raw_get<I>(storage_);
            alt.~T();
        }
    }

    // internal
    constexpr void visit_destroy() noexcept
    {
        if constexpr (need_destructor_call) {
            this->template raw_visit<true>([]<std::size_t i, class T>(std::in_place_index_t<i>, [[maybe_unused]] T& alt) static noexcept {
                if constexpr (i != std::variant_npos) {
                    alt.~T();
                }
            });
        }
    }

    // internal
    constexpr void visit_reset() noexcept
    {
        if constexpr (need_destructor_call) {
            this->template raw_visit<true>([this]<std::size_t i, class T>(std::in_place_index_t<i>, [[maybe_unused]] T& alt) noexcept {
                if constexpr (i != std::variant_npos) {
                    alt.~T();
                    index_ = detail::variant_npos<sizeof...(Ts)>;
                }
            });
        } else {
            index_ = detail::variant_npos<sizeof...(Ts)>;
        }
    }
    // internal
    template<std::size_t I>
    constexpr void reset() noexcept
    {
        assert(index_ == I);
        if constexpr (I != std::variant_npos) {
            // ReSharper disable once CppTypeAliasNeverUsed
            using T = IRIS_PACK_INDEXING(I, Ts...);
            auto&& alt = detail::raw_get<I>(storage_);
            alt.~T();
            index_ = detail::variant_npos<sizeof...(Ts)>;
        }
    }

IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
    template<std::size_t I, class... Args>
    constexpr void construct_on_valueless(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>)
    {
        static_assert(I != std::variant_npos);
        assert(index_ == detail::variant_npos<sizeof...(Ts)>);
        detail::alternative_constructor<I>::construct(storage_, std::forward<Args>(args)...);
        index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(I);
    }

    template<std::size_t I, class... Args>
    constexpr void reset_construct(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>)
    {
        static_assert(I != std::variant_npos);
        if constexpr (std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>) {
            visit_destroy(); // the index is overwritten below without being observed
        } else {
            visit_reset();
        }
        detail::alternative_constructor<I>::construct(storage_, std::forward<Args>(args)...);
        index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(I);
    }

    template<std::size_t i, std::size_t j, class... Args>
    constexpr void reset_construct(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(j, Ts...), Args...>)
    {
        if constexpr (i != std::variant_npos) {
            destroy<i>();
            if constexpr (!std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(j, Ts...), Args...>) {
                index_ = detail::variant_npos<sizeof...(Ts)>;
            }
        }
        static_assert(j != std::variant_npos);
        detail::alternative_constructor<j>::construct(storage_, std::forward<Args>(args)...);
        index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(j);
    }

    template<std::size_t I, class... Args>
    constexpr void reset_construct_never_valueless(Args&&... args) noexcept
    {
        static_assert(I != std::variant_npos);
        static_assert(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>);
        visit_destroy();
        detail::alternative_constructor<I>::construct(storage_, std::forward<Args>(args)...);
        index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(I);
    }

IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END

    // used in swap operation
    constexpr void reset_steal_from(rvariant&& rhs)
        noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>)
    {
        visit_reset();

        constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>;
        std::move(rhs).template raw_visit<is_noexcept>([this]<std::size_t i, class T>(std::in_place_index_t<i>, [[maybe_unused]] T&& alt)
            noexcept(is_noexcept)
        {
            if constexpr (i != std::variant_npos) {
                static_assert(std::is_rvalue_reference_v<T&&>);
                this->template construct_on_valueless<i>(std::move(alt)); // NOLINT(bugprone-move-forwarding-reference)
            }
        });
    }

    // -----------------------------------------------------------

IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
    template<std::size_t I, class... Args>
        requires std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>
    constexpr variant_alternative_t<I, rvariant<Ts...>>&
    emplace_impl(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>) IRIS_LIFETIMEBOUND
    {
        static_assert(I < sizeof...(Ts));
        using T = IRIS_PACK_INDEXING(I, Ts...);

#ifndef NDEBUG
        // Self-emplace on non-valueless instance ALWAYS leads to UB.
        //
        // Constructions, assignment or emplacement on a recursive
        // alternative is likely to be nested inside a deep context
        // (notably inside a parser combinator library's semantic
        // actions), which makes it challenging for users to debug
        // type-changing self-emplacement in application layer.
        //
        // Note that our assertions only check the starting address;
        // subobjects are not considered as it requires well-defined
        // constexpr access to the underlying bytes even for
        // non-trivially-copyable types. See also:
        //   - http://wg21.link/p1839
        //   - https://github.com/cplusplus/papers/issues/592
        //   - https://cplusplus.github.io/LWG/issue3069

        if constexpr (!never_valueless) {
            if (!this->valueless_by_exception()) {
                ((assert(
                    static_cast<void const*>(std::addressof(args)) != static_cast<void const*>(this) &&
                    "Self-emplacing `variant` will lead to undefined behavior because the standard specifies `emplace` to destruct the contained object *before* emplacing the new value ([variant.mod])."
                )), ...);

                ((assert(
                    static_cast<void const*>(std::addressof(args)) != static_cast<void const*>(std::addressof(this->storage_)) &&
                    "Self-emplacing `variant` will lead to undefined behavior because the standard specifies `emplace` to destruct the contained object *before* emplacing the new value ([variant.mod])."
                )), ...);
            }
        }
#endif

        // The paths below that construct a temporary first provide the strong exception-safety guarantee.
        // They are taken only when the difference from the specification is not observable.
        // See the comments on `detail::is_never_valueless_impl` for details.
        constexpr bool is_tmp_trivial =
            sizeof(T) <= detail::never_valueless_trivial_size_limit && std::is_trivially_destructible_v<T>;

        if constexpr (std::is_nothrow_constructible_v<T, Args...>) {
            this->template reset_construct_never_valueless<I>(std::forward<Args>(args)...);

        } else if constexpr (!need_destructor_call) {
            // Nothing needs to be destroyed, so neither the old alternative nor the valueless state matters
            // and no visit is needed. For the same alternative, the assignment of the temporary is replaced
            // by the construction, which is not observable because T is trivial.
            if constexpr (is_tmp_trivial && std::is_trivially_move_constructible_v<T>) {
                static_assert(std::is_nothrow_constructible_v<T, T&&>);
                if constexpr (sizeof...(Args) == 0) {
                    T tmp = T(); // may throw
                    detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                } else {
                    T tmp(std::forward<Args>(args)...); // may throw
                    detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                }

            } else if constexpr (is_tmp_trivial && std::is_trivially_copy_constructible_v<T>) { // strange type...
                static_assert(std::is_nothrow_constructible_v<T, T const&>);
                if constexpr (sizeof...(Args) == 0) {
                    T const tmp = T(); // may throw
                    detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                } else {
                    T const tmp(std::forward<Args>(args)...); // may throw
                    detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                }

            } else {
                static_assert(!never_valueless);
                this->index_ = detail::variant_npos<sizeof...(Ts)>;
                detail::alternative_constructor<I>::construct(this->storage_, std::forward<Args>(args)...); // may throw
            }
            this->index_ = I;

        } else {
            this->template raw_visit<false>([&, this]<std::size_t old_i, class T_old_i>(std::in_place_index_t<old_i>, T_old_i& t_old_i) {
                static_assert(!std::is_reference_v<T_old_i>);
                static_assert(!std::is_const_v<T_old_i>);

                if constexpr (old_i == std::variant_npos) {
                    (void)t_old_i;
                    this->template construct_on_valueless<I>(std::forward<Args>(args)...);

                } else if constexpr (old_i == I) { // same alternative
                    if constexpr (is_recursive_wrapper_v<T> || (is_tmp_trivial && std::is_trivially_move_assignable_v<T>)) {
                        static_assert(noexcept(t_old_i = std::declval<T&&>()));
                        if constexpr (sizeof...(Args) == 0) {
                            T tmp = T(); // may throw
                            t_old_i = std::move(tmp);
                        } else {
                            T tmp(std::forward<Args>(args)...); // may throw
                            t_old_i = std::move(tmp);
                        }

                    } else if constexpr (is_tmp_trivial && std::is_trivially_copy_assignable_v<T>) { // strange type...
                        static_assert(noexcept(t_old_i = std::declval<T const&>()));
                        if constexpr (sizeof...(Args) == 0) {
                            T const tmp = T(); // may throw
                            t_old_i = tmp;
                        } else {
                            T const tmp(std::forward<Args>(args)...); // may throw
                            t_old_i = tmp;
                        }

                    } else if constexpr (is_tmp_trivial && std::is_trivially_move_constructible_v<T>) { // not assignable
                        static_assert(std::is_nothrow_constructible_v<T, T&&>);
                        if constexpr (sizeof...(Args) == 0) {
                            T tmp = T(); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                        } else {
                            T tmp(std::forward<Args>(args)...); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                        }

                    } else if constexpr (is_tmp_trivial && std::is_trivially_copy_constructible_v<T>) { // not assignable, strange type...
                        static_assert(std::is_nothrow_constructible_v<T, T const&>);
                        if constexpr (sizeof...(Args) == 0) {
                            T const tmp = T(); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                        } else {
                            T const tmp(std::forward<Args>(args)...); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                        }

                    } else {
                        static_assert(!never_valueless);
                        t_old_i.~T_old_i();
                        this->index_ = detail::variant_npos<sizeof...(Ts)>;
                        static_assert(!std::is_nothrow_constructible_v<T, Args...>);
                        detail::alternative_constructor<I>::construct(this->storage_, std::forward<Args>(args)...); // may throw
                        this->index_ = I;
                    }

                } else { // another alternative, possibly of the same type
                    // The old alternative is destroyed after the temporary is constructed
                    constexpr bool is_old_destruction_delayable =
                        std::is_trivially_destructible_v<T_old_i> || is_recursive_wrapper_v<T_old_i>;

                    if constexpr (
                        is_recursive_wrapper_v<T> ||
                        (is_tmp_trivial && is_old_destruction_delayable && std::is_trivially_move_constructible_v<T>)
                    ) {
                        static_assert(std::is_nothrow_constructible_v<T, T&&>);
                        if constexpr (sizeof...(Args) == 0) {
                            T tmp = T(); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                        } else {
                            T tmp(std::forward<Args>(args)...); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, std::move(tmp)); // never throws
                        }
                        this->index_ = I;

                    } else if constexpr (
                        is_tmp_trivial && is_old_destruction_delayable && std::is_trivially_copy_constructible_v<T>
                    ) { // strange type...
                        static_assert(std::is_nothrow_constructible_v<T, T const&>);
                        if constexpr (sizeof...(Args) == 0) {
                            T const tmp = T(); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                        } else {
                            T const tmp(std::forward<Args>(args)...); // may throw
                            t_old_i.~T_old_i();
                            detail::alternative_constructor<I>::construct(this->storage_, tmp); // never throws
                        }
                        this->index_ = I;

                    } else {
                        static_assert(!never_valueless);
                        t_old_i.~T_old_i();
                        this->index_ = detail::variant_npos<sizeof...(Ts)>;
                        static_assert(!std::is_nothrow_constructible_v<T, Args...>);
                        detail::alternative_constructor<I>::construct(this->storage_, std::forward<Args>(args)...); // may throw
                        this->index_ = I;
                    }
                }
            });
        }
        return unwrap_recursive(detail::raw_get<I>(storage_));
    }
IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END

    // -----------------------------------------------------------

    [[nodiscard]] constexpr storage_type &       storage() &       noexcept { return storage_; }
    [[nodiscard]] constexpr storage_type const&  storage() const&  noexcept { return storage_; }
    [[nodiscard]] constexpr storage_type &&      storage() &&      noexcept { return std::move(storage_); }
    [[nodiscard]] constexpr storage_type const&& storage() const&& noexcept { return std::move(storage_); }

    // The caller passes the exception specification it already knows,
    // because deducing it from every alternative of the visitor is costly
    template<bool Noexcept, class Self, class Visitor>
    IRIS_FORCEINLINE constexpr auto
    raw_visit(this Self&& self, Visitor&& vis) noexcept(Noexcept)  // NOLINT(cppcoreguidelines-missing-std-forward)
        -> detail::raw_visit_result_t<Visitor, decltype(std::forward_like<Self>(self.storage_))>
    {
#if IRIS_CI
        static_assert(Noexcept == detail::raw_visit_noexcept_all<Visitor, decltype(std::forward_like<Self>(self.storage_))>);
#endif
        constexpr std::size_t N = detail::valueless_bias<never_valueless>(sizeof...(Ts));
        return detail::raw_visit_dispatch<never_valueless, detail::visit_strategy<N>>::template apply<
            N, Visitor, decltype(std::forward_like<Self>(self.storage_)), Noexcept
        >(
            detail::valueless_bias<never_valueless>(self.index_),
            std::forward<Visitor>(vis),
            std::forward_like<Self>(self.storage_)
        );
    }

    storage_type storage_{}; // valueless
    // No default member initializer; every constructor initializes this exactly once,
    // so that the copy/move constructors do not store `variant_npos` before storing the actual index
    detail::variant_index_t<sizeof...(Ts)> index_;

public:
    // Default constructor
    constexpr rvariant() noexcept(std::is_nothrow_default_constructible_v<IRIS_PACK_INDEXING(0, Ts...)>)
        requires std::is_default_constructible_v<IRIS_PACK_INDEXING(0, Ts...)>
        : rvariant(detail::primary_construct, std::in_place_index<0>) // value-initialized
    {}

    // --------------------------------------

    // Generic constructor
    // <https://eel.is/c++draft/variant.ctor#lib:variant,constructor___>
    template<class T>
        requires
            (sizeof...(Ts) > 0) &&
            (!std::is_same_v<std::remove_cvref_t<T>, rvariant>) &&
            (!is_ttp_specialization_of_v<std::remove_cvref_t<T>, std::in_place_type_t>) &&
            (!is_ctp_specialization_of_v<std::remove_cvref_t<T>, std::in_place_index_t>) &&
            std::is_constructible_v<typename no_narrowing_resolution<T, Ts...>::type, T>
    constexpr /* not explicit */ rvariant(T&& t)
        noexcept(std::is_nothrow_constructible_v<typename no_narrowing_resolution<T, Ts...>::type, T>)
        : rvariant(detail::primary_construct, std::in_place_index<no_narrowing_resolution<T, Ts...>::index>, std::forward<T>(t))
    {}

IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
    // Generic assignment operator
    // <https://eel.is/c++draft/variant.assign#lib:operator=,variant__>
    template<class T>
        requires
            (!std::is_same_v<std::remove_cvref_t<T>, rvariant>) &&
            detail::rvariant_alternative_assignable<typename no_narrowing_resolution<T, Ts...>::type, T>
    constexpr rvariant& operator=(T&& t)
        noexcept(detail::rvariant_alternative_nothrow_assignable<typename no_narrowing_resolution<T, Ts...>::type, T>)
    {
        using Tj = no_narrowing_resolution<T, Ts...>::type; // either plain type or wrapped with recursive_wrapper
        constexpr std::size_t j = no_narrowing_resolution<T, Ts...>::index;
        static_assert(j != std::variant_npos);

        // TC(noexcept) && MC(throw)    => A maybe valueless if | never |
        // TC(noexcept) && MC(noexcept) => A maybe valueless if | never |
        // TC(throw)    && MC(throw)    => A maybe valueless if | TC throws => yes | MC throws => yes |
        // TC(throw)    && MC(noexcept) => B maybe valueless if | never |
        //
        // If the variant is never valueless, "TC(throw) && MC(throw)" implies that Tj is not move constructible
        // (see `detail::is_never_valueless_impl`). In that case, Tj is trivially copy constructible, so a temporary
        // is copied instead => C maybe valueless if | never |
        constexpr bool is_tmp_copied =
            never_valueless && !std::is_nothrow_constructible_v<Tj, T> && !std::is_nothrow_move_constructible_v<Tj>;

        if constexpr (!need_destructor_call) {
            // Nothing needs to be destroyed, so a comparison of the index replaces the visit
            if (this->index_ == j) {
                detail::raw_get<j>(this->storage_) = std::forward<T>(t);

            } else if constexpr (is_tmp_copied) {
                static_assert(std::is_nothrow_constructible_v<Tj, Tj const&>);
                Tj const tmp(std::forward<T>(t)); // may throw
                detail::alternative_constructor<j>::construct(this->storage_, tmp); // C
                this->index_ = j;

            } else if constexpr (std::is_nothrow_constructible_v<Tj, T> || !std::is_nothrow_move_constructible_v<Tj>) {
#ifndef NDEBUG
                // Self-assign on non-valueless instance ALWAYS leads to UB.
                // For details, see the comments on `emplace`.
                assert(
                    (this->index_ == detail::variant_npos<sizeof...(Ts)> || (
                        static_cast<void const*>(std::addressof(t)) != static_cast<void const*>(this) &&
                        static_cast<void const*>(std::addressof(t)) != static_cast<void const*>(std::addressof(this->storage_))
                    )) &&
                    "Self-assigning `variant` will lead to undefined behavior because the standard specifies `emplace` to destruct the contained object *before* emplacing the new value ([variant.mod])."
                );
#endif
                static_assert(std::is_nothrow_constructible_v<Tj, T> || !never_valueless);
                if constexpr (!std::is_nothrow_constructible_v<Tj, T>) {
                    this->index_ = detail::variant_npos<sizeof...(Ts)>;
                }
                detail::alternative_constructor<j>::construct(this->storage_, std::forward<T>(t));
                this->index_ = j;

            } else {
                Tj tmp(std::forward<T>(t));
                detail::alternative_constructor<j>::construct(this->storage_, std::move(tmp)); // B
                this->index_ = j;
            }
            return *this;
        }

        constexpr bool is_noexcept = detail::rvariant_alternative_nothrow_assignable<Tj, T>;
        this->template raw_visit<is_noexcept>([this, &t]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti& ti)
            noexcept(is_noexcept)
        {
            if constexpr (i == j) {
                ti = std::forward<T>(t);
            } else {
                if constexpr (is_tmp_copied) {
                    static_assert(std::is_nothrow_constructible_v<Tj, Tj const&>);
                    Tj const tmp(std::forward<T>(t)); // may throw
                    this->template reset_construct<i, j>(tmp); // C

                } else if constexpr (std::is_nothrow_constructible_v<Tj, T> || !std::is_nothrow_move_constructible_v<Tj>) {
#ifndef NDEBUG
                    // Self-assign on non-valueless instance ALWAYS leads to UB.
                    // For details, see the comments on `emplace`.
                    if constexpr (i != std::variant_npos) {
                        assert(
                            static_cast<void const*>(std::addressof(t)) != static_cast<void const*>(this) &&
                            static_cast<void const*>(std::addressof(t)) != static_cast<void const*>(std::addressof(ti)) &&
                            "Self-assigning `variant` will lead to undefined behavior because the standard specifies `emplace` to destruct the contained object *before* emplacing the new value ([variant.mod])."
                        );
                    }
#endif
                    static_assert(std::is_nothrow_constructible_v<Tj, T> || !never_valueless);
                    this->template reset_construct<i, j>(std::forward<T>(t));

                } else {
                    Tj tmp(std::forward<T>(t));
                    this->template reset_construct<i, j>(std::move(tmp)); // B
                }
            }
        });
        return *this;
    }
IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END

    // --------------------------------------

    // Flexible copy constructor
    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant> &&
            (!std::is_same_v<rvariant<Us...>, unwrap_recursive_t<Ts>> && ...) &&
            (std::is_constructible_v<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&> && ...)
    constexpr rvariant(rvariant<Us...> const& w)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&>...>)
    {
        constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_constructible<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&>...>;
        w.template raw_visit<is_noexcept>([this]<std::size_t j, class Uj>(std::in_place_index_t<j>, [[maybe_unused]] Uj const& uj)
            noexcept(is_noexcept)
        {
            if constexpr (j != std::variant_npos) {
                using maybe_wrapped = detail::select_maybe_wrapped<unwrap_recursive_t<Uj>, Ts...>;
                using VT = maybe_wrapped::type;
                static_assert(std::is_same_v<unwrap_recursive_t<VT>, unwrap_recursive_t<Uj>>);
                detail::alternative_constructor<maybe_wrapped::index>::construct(this->storage_, uj);
                this->index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(maybe_wrapped::index);
            } else {
                this->index_ = detail::variant_npos<sizeof...(Ts)>;
            }
        });
    }

    // Flexible move constructor
    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant> &&
            (!std::is_same_v<rvariant<Us...>, unwrap_recursive_t<Ts>> && ...) &&
            (std::is_constructible_v<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&> && ...)
    constexpr rvariant(rvariant<Us...>&& w)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&>...>)
    {
        constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_constructible<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&>...>;
        std::move(w).template raw_visit<is_noexcept>([this]<std::size_t j, class Uj>(std::in_place_index_t<j>, [[maybe_unused]] Uj&& uj)
            noexcept(is_noexcept)
        {
            if constexpr (j != std::variant_npos) {
                using maybe_wrapped = detail::select_maybe_wrapped<unwrap_recursive_t<Uj>, Ts...>;
                using VT = maybe_wrapped::type;
                static_assert(std::is_same_v<unwrap_recursive_t<VT>, unwrap_recursive_t<Uj>>);
                static_assert(std::is_rvalue_reference_v<Uj&&>);
                detail::alternative_constructor<maybe_wrapped::index>::construct(this->storage_, std::move(uj)); // NOLINT(bugprone-move-forwarding-reference)
                this->index_ = static_cast<detail::variant_index_t<sizeof...(Ts)>>(maybe_wrapped::index);
            } else {
                this->index_ = detail::variant_npos<sizeof...(Ts)>;
            }
        });
    }

    // --------------------------------------

    // Flexible copy assignment operator
    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant> &&
            (!std::is_same_v<rvariant<Us...>, unwrap_recursive_t<Ts>> && ...) &&
            (detail::rvariant_alternative_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&> && ...)
    constexpr rvariant& operator=(rvariant<Us...> const& rhs)
        noexcept((detail::rvariant_alternative_nothrow_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&> && ...))
    {
        constexpr bool is_noexcept = (detail::rvariant_alternative_nothrow_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us const&> && ...);
        rhs.template raw_visit<is_noexcept>([this]<std::size_t j, class Uj>(std::in_place_index_t<j>, [[maybe_unused]] Uj const& uj)
            noexcept(is_noexcept)
        {
            if constexpr (j == std::variant_npos) {
                this->visit_reset();

            } else {
                using maybe_wrapped = detail::select_maybe_wrapped<unwrap_recursive_t<Uj>, Ts...>;
                using VT = maybe_wrapped::type;
                static_assert(std::is_same_v<unwrap_recursive_t<VT>, unwrap_recursive_t<Uj>>);

                this->template raw_visit<is_noexcept>([this, &uj]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti& ti)
                    noexcept(is_noexcept)
                {
                    constexpr std::size_t VTi = maybe_wrapped::index;
                    if constexpr (i == std::variant_npos) { // this is valueless, rhs holds value
                        this->template construct_on_valueless<VTi>(uj);

                    } else if constexpr (std::is_same_v<unwrap_recursive_t<Ti>, unwrap_recursive_t<Uj>>) {
                        ti = uj;

                    } else if constexpr (std::is_nothrow_constructible_v<VT, Uj const&> || !std::is_nothrow_move_constructible_v<VT>) {
                        this->template reset_construct<i, VTi>(uj);

                    } else {
                        VT tmp(uj); // may throw
                        this->template reset_construct<i, VTi>(std::move(tmp));
                    }
                });
            }
        });
        return *this;
    }

    // Flexible move assignment operator
    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant> &&
            (!std::is_same_v<rvariant<Us...>, unwrap_recursive_t<Ts>> && ...) &&
            (detail::rvariant_alternative_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&> && ...)
    constexpr rvariant& operator=(rvariant<Us...>&& rhs)
        noexcept((detail::rvariant_alternative_nothrow_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&> && ...))
    {
        constexpr bool is_noexcept = (detail::rvariant_alternative_nothrow_assignable<detail::select_maybe_wrapped_t<unwrap_recursive_t<Us>, Ts...>, Us&&> && ...);
        std::move(rhs).template raw_visit<is_noexcept>([this]<std::size_t j, class Uj>(std::in_place_index_t<j>, [[maybe_unused]] Uj&& uj)
            noexcept(is_noexcept)
        {
            if constexpr (j == std::variant_npos) {
                this->visit_reset();

            } else {
                using maybe_wrapped = detail::select_maybe_wrapped<unwrap_recursive_t<Uj>, Ts...>;
                using VT = maybe_wrapped::type;
                static_assert(std::is_same_v<unwrap_recursive_t<VT>, unwrap_recursive_t<Uj>>);

                this->template raw_visit<is_noexcept>([this, &uj]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti& ti)
                    noexcept(is_noexcept)
                {
                    static_assert(std::is_rvalue_reference_v<Uj&&>);
                    constexpr std::size_t VTi = maybe_wrapped::index;
                    if constexpr (i == std::variant_npos) { // this is valueless, rhs holds value
                        this->template construct_on_valueless<VTi>(std::move(uj));  // NOLINT(bugprone-move-forwarding-reference)

                    } else if constexpr (std::is_same_v<unwrap_recursive_t<Ti>, unwrap_recursive_t<Uj>>) {
                        ti = std::move(uj); // NOLINT(bugprone-move-forwarding-reference)

                    } else {
                        this->template reset_construct<i, VTi>(std::move(uj));  // NOLINT(bugprone-move-forwarding-reference)
                    }
                });
            }
        });
        return *this;
    }

    // ------------------------------------------------

    // in_place_type<T>, args...
    template<class T, class... Args>
        requires
            detail::non_wrapped_exactly_once_v<T, unwrapped_types> &&
            std::is_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, Args...>
    constexpr explicit rvariant(std::in_place_type_t<T>, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, Args...>)
        : rvariant(detail::primary_construct, std::in_place_index<detail::select_maybe_wrapped_index<T, Ts...>>, std::forward<Args>(args)...)
    {}

    // in_place_type<T>, il, args...
    template<class T, class U, class... Args>
        requires
            detail::non_wrapped_exactly_once_v<T, unwrapped_types> &&
            std::is_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, std::initializer_list<U>&, Args...>
    constexpr explicit rvariant(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, std::initializer_list<U>&, Args...>)
        : rvariant(detail::primary_construct, std::in_place_index<detail::select_maybe_wrapped_index<T, Ts...>>, il, std::forward<Args>(args)...)
    {}

    // in_place_index<I>, args...
    template<std::size_t I, class... Args>
        requires
            (I < sizeof...(Ts)) &&
            std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>
    constexpr explicit rvariant(std::in_place_index_t<I>, Args&&... args) // NOLINT
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>)
        : rvariant(detail::primary_construct, std::in_place_index<I>, std::forward<Args>(args)...)
    {}

    // in_place_index<I>, il, args...
    template<std::size_t I, class U, class... Args>
        requires
            (I < sizeof...(Ts)) &&
            std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), std::initializer_list<U>&, Args...>
    constexpr explicit rvariant(std::in_place_index_t<I>, std::initializer_list<U> il, Args&&... args) // NOLINT
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), std::initializer_list<U>&, Args...>)
        : rvariant(detail::primary_construct, std::in_place_index<I>, il, std::forward<Args>(args)...)
    {}

    // -------------------------------------------

    template<class T, class... Args>
        requires
            detail::non_wrapped_exactly_once_v<T, unwrapped_types> &&
            std::is_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, Args...>
    constexpr T& emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, Args...>) IRIS_LIFETIMEBOUND
    {
        return this->template emplace_impl<detail::select_maybe_wrapped_index<T, Ts...>>(std::forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
        requires
            detail::non_wrapped_exactly_once_v<T, unwrapped_types> &&
            std::is_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, std::initializer_list<U>&, Args...>
    constexpr T& emplace(std::initializer_list<U> il, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<detail::select_maybe_wrapped_t<T, Ts...>, std::initializer_list<U>&, Args...>) IRIS_LIFETIMEBOUND
    {
        return this->template emplace_impl<detail::select_maybe_wrapped_index<T, Ts...>>(il, std::forward<Args>(args)...);
    }

    template<std::size_t I, class... Args>
        requires std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>
    constexpr variant_alternative_t<I, rvariant>&
    emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), Args...>) IRIS_LIFETIMEBOUND
    {
        static_assert(I < sizeof...(Ts));
        return this->template emplace_impl<I>(std::forward<Args>(args)...);
    }

    template<std::size_t I, class U, class... Args>
        requires std::is_constructible_v<IRIS_PACK_INDEXING(I, Ts...), std::initializer_list<U>&, Args...>
    constexpr variant_alternative_t<I, rvariant>&
    emplace(std::initializer_list<U> il, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<IRIS_PACK_INDEXING(I, Ts...), std::initializer_list<U>&, Args...>) IRIS_LIFETIMEBOUND
    {
        static_assert(I < sizeof...(Ts));
        return this->template emplace_impl<I>(il, std::forward<Args>(args)...);
    }


    constexpr void swap(rvariant& rhs)
        noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_swappable<Ts>...>)
    {
        static_assert(std::conjunction_v<std::is_move_constructible<Ts>...>);
        static_assert(std::conjunction_v<std::is_swappable<Ts>...>);
        [[maybe_unused]] static constexpr bool all_nothrow_swappable = std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_swappable<Ts>...>;

        if constexpr (std::conjunction_v<is_trivially_swappable<Ts>...>) {
            static_assert(is_trivially_swappable_v<decltype(storage())>);
            std::swap(storage(), rhs.storage()); // no ADL
            std::swap(index_, rhs.index_);

        } else if constexpr (sizeof...(Ts) * sizeof...(Ts) < 1024) {
        IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
            this->template raw_visit<all_nothrow_swappable>([this, &rhs]<std::size_t i, class ThisAlt>(std::in_place_index_t<i>, [[maybe_unused]] ThisAlt& this_alt)
                noexcept(all_nothrow_swappable)
            {
                rhs.template raw_visit<all_nothrow_swappable>([this, &rhs, &this_alt]<std::size_t j, class RhsAlt>(std::in_place_index_t<j>, [[maybe_unused]] RhsAlt& rhs_alt)
                    noexcept(all_nothrow_swappable)
                {
                    if constexpr (i == j) {
                        if constexpr (i != std::variant_npos) {
                            using std::swap;
                            swap(this_alt, rhs_alt);
                        }

                    } else if constexpr (i == std::variant_npos) {
                        this->template construct_on_valueless<j>(std::move(rhs_alt));
                        rhs.template reset<j>();
                        (void)this_alt;

                    } else if constexpr (j == std::variant_npos) {
                        rhs.template construct_on_valueless<i>(std::move(this_alt));
                        this->template reset<i>();

                    } else {
                        auto tmp = std::move(this_alt);
                        this->template reset<i>();
                        this->template construct_on_valueless<j>(std::move(rhs_alt));
                        rhs.template reset<j>();
                        rhs.template construct_on_valueless<i>(std::move(tmp));
                    }
                });
            });
        IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END
        } else {
            if (index_ == rhs.index_) {
                constexpr bool is_noexcept = std::conjunction_v<std::is_nothrow_swappable<Ts>...>;
                rhs.template raw_visit<is_noexcept>([this]<std::size_t i, class RhsAlt>(std::in_place_index_t<i>, [[maybe_unused]] RhsAlt& rhs_alt)
                    noexcept(is_noexcept)
                {
                    if constexpr (i != std::variant_npos) {
                        using std::swap;
                        swap(detail::raw_get<i>(this->storage()), rhs_alt);
                    }
                });
            } else {
                auto tmp = std::move(*this);
                this->reset_steal_from(std::move(rhs));
                rhs.reset_steal_from(std::move(tmp));
            }
        }
    }

    friend constexpr void swap(rvariant& v, rvariant& w)
        noexcept(noexcept(v.swap(w)))
        requires ((std::is_move_constructible_v<Ts> && std::is_swappable_v<Ts>) && ...)
    {
        v.swap(w);
    }

    template<class... Us>
        requires std::is_same_v<rvariant<Us...>, rvariant>
    [[nodiscard]] constexpr rvariant subset() const& noexcept(std::is_nothrow_copy_constructible_v<rvariant>)
    {
        return *this;
    }

    template<class... Us>
        requires std::is_same_v<rvariant<Us...>, rvariant>
    [[nodiscard]] constexpr rvariant subset() && noexcept(std::is_nothrow_move_constructible_v<rvariant>)
    {
        return std::move(*this);
    }

    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant>
    [[nodiscard]] constexpr rvariant<Us...> subset() const&
        noexcept(
            rvariant_set::equivalent_to<rvariant<Us...>, rvariant> &&
            std::is_nothrow_constructible_v<rvariant<Us...>, rvariant const&> // equivalent to flexible copy constructor
        )
    {
        if constexpr (rvariant_set::equivalent_to<rvariant<Us...>, rvariant>) {
            constexpr bool is_noexcept = std::is_nothrow_constructible_v<rvariant<Us...>, rvariant const&>;
            return this->template raw_visit<is_noexcept>([]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti const& ti) static
                noexcept(is_noexcept) -> rvariant<Us...>
            {
                if constexpr (i == std::variant_npos) {
                    return rvariant<Us...>(detail::valueless);
                } else {
                    constexpr std::size_t j = detail::subset_reindex<rvariant, rvariant<Us...>>(i);
                    static_assert(j != find_npos);
                    return rvariant<Us...>(std::in_place_index<j>, ti);
                }
            });
        } else {
            return this->template raw_visit<false>([]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti const& ti) static
                /* not noexcept */ -> rvariant<Us...>
            {
                if constexpr (i == std::variant_npos) {
                    return rvariant<Us...>(detail::valueless);
                } else {
                    constexpr std::size_t j = detail::subset_reindex<rvariant, rvariant<Us...>>(i);
                    if constexpr (j == find_npos) {
                        detail::throw_bad_variant_access();
                    } else {
                        return rvariant<Us...>(std::in_place_index<j>, ti);
                    }
                }
            });
        }
    }

    template<class... Us>
        requires
            (!std::is_same_v<rvariant<Us...>, rvariant>) &&
            rvariant_set::subset_of<rvariant<Us...>, rvariant>
    [[nodiscard]] constexpr rvariant<Us...> subset() &&
        noexcept(
            rvariant_set::equivalent_to<rvariant<Us...>, rvariant> &&
            std::is_nothrow_constructible_v<rvariant<Us...>, rvariant&&> // equivalent to flexible move constructor
        )
    {
        if constexpr (rvariant_set::equivalent_to<rvariant<Us...>, rvariant>) {
            constexpr bool is_noexcept = std::is_nothrow_constructible_v<rvariant<Us...>, rvariant&&>;
            return std::move(*this).template raw_visit<is_noexcept>([]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti&& ti) static
                noexcept(is_noexcept) -> rvariant<Us...>
            {
                if constexpr (i == std::variant_npos) {
                    return rvariant<Us...>(detail::valueless);
                } else {
                    constexpr std::size_t j = detail::subset_reindex<rvariant, rvariant<Us...>>(i);
                    static_assert(j != find_npos);
                    static_assert(std::is_rvalue_reference_v<Ti&&>);
                    return rvariant<Us...>(std::in_place_index<j>, std::move(ti)); // NOLINT(bugprone-move-forwarding-reference)
                }
            });
        } else {
            return std::move(*this).template raw_visit<false>([]<std::size_t i, class Ti>(std::in_place_index_t<i>, [[maybe_unused]] Ti&& ti) static
                /* not noexcept */ -> rvariant<Us...>
            {
                if constexpr (i == std::variant_npos) {
                    return rvariant<Us...>(detail::valueless);
                } else {
                    constexpr std::size_t j = detail::subset_reindex<rvariant, rvariant<Us...>>(i);
                    if constexpr (j == find_npos) {
                        detail::throw_bad_variant_access();
                    } else {
                        static_assert(std::is_rvalue_reference_v<Ti&&>);
                        return rvariant<Us...>(std::in_place_index<j>, std::move(ti)); // NOLINT(bugprone-move-forwarding-reference)
                    }
                }
            });
        }
    }

    // NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
    // ReSharper disable CppCStyleCast

    // Member `.visit(...)`
    // <https://eel.is/c++draft/variant.visit#lib:visit,variant_>
    template<int = 0, class Self, class Visitor>
    constexpr decltype(auto) visit(this Self&& self, Visitor&& vis)
        noexcept(noexcept(iris::visit(
            std::forward<Visitor>(vis),
            (like_rvariant_t<Self>)self
        )))
    {
        return iris::visit(
            std::forward<Visitor>(vis),
            (like_rvariant_t<Self>)self
        );
    }

    // Member `.visit<R>(...)`
    // <https://eel.is/c++draft/variant.visit#lib:visit,variant__>
    template<class R, class Self, class Visitor>
    constexpr R visit(this Self&& self, Visitor&& vis)
        noexcept(noexcept(iris::visit<R>(
            std::forward<Visitor>(vis),
            (like_rvariant_t<Self>)self
        )))
    {
        return iris::visit<R>(
            std::forward<Visitor>(vis),
            (like_rvariant_t<Self>)self
        );
    }

    // ReSharper restore CppCStyleCast
    // NOLINTEND(cppcoreguidelines-missing-std-forward)
    template<class... Us>
    friend class rvariant;

    template<class T, class Variant>
    friend struct detail::exactly_once_index;

    template<class Variant>
    friend struct detail::forward_storage_t_impl;

    template<class Variant>
    friend constexpr detail::forward_storage_t<Variant>&& detail::forward_storage(std::remove_reference_t<Variant>& v IRIS_LIFETIMEBOUND) noexcept;

    template<class Variant>
    friend constexpr detail::forward_storage_t<Variant>&& detail::forward_storage(std::remove_reference_t<Variant>&& v IRIS_LIFETIMEBOUND) noexcept;

    template<class Variant, class T>
    friend constexpr std::size_t detail::valueless_bias(T) noexcept;

    template<class Variant, class T>
    friend constexpr std::size_t detail::valueless_unbias(T) noexcept;

    template<class R, class V, std::size_t... n>
    friend struct detail::visit_impl;

    template<class Variant, class Visitor>
    friend constexpr detail::raw_visit_result_t<Visitor, detail::forward_storage_t<Variant>>
    detail::raw_visit(Variant&&, Visitor&&)  // NOLINT(clang-diagnostic-microsoft-exception-spec)
        noexcept(detail::raw_visit_noexcept_all<Visitor, detail::forward_storage_t<Variant>>);

    template<class R, class Compare, class... Ts_>
    friend struct detail::relops_visitor;

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::equal_to<>, Ts_> && ...)
    friend constexpr bool operator==(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::equal_to<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::not_equal_to<>, Ts_> && ...)
    friend constexpr bool operator!=(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::not_equal_to<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::less<>, Ts_> && ...)
    friend constexpr bool operator<(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::less<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::greater<>, Ts_> && ...)
    friend constexpr bool operator>(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::greater<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::less_equal<>, Ts_> && ...)
    friend constexpr bool operator<=(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::less_equal<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (cmp::relop_bool_expr_v<std::greater_equal<>, Ts_> && ...)
    friend constexpr bool operator>=(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<bool, std::greater_equal<>, Ts_...>::is_noexcept);

    template<class... Ts_>
        requires (std::three_way_comparable<Ts_> && ...)
    friend constexpr std::common_comparison_category_t<std::compare_three_way_result_t<Ts_>...>
    operator<=>(rvariant<Ts_...> const&, rvariant<Ts_...> const&)
        noexcept(detail::relops_visitor<
            std::common_comparison_category_t<std::compare_three_way_result_t<Ts_>...>,
            std::compare_three_way,
            Ts_...
        >::is_noexcept);

    template<class... Ts_>
    friend constexpr rvariant<Ts_...> detail::make_valueless() noexcept;

private:
    // hack: reduce compile error by half on unrelated overloads
    template<std::same_as<detail::valueless_t> Valueless>
    constexpr explicit rvariant(Valueless const&) noexcept
        : index_{detail::variant_npos<sizeof...(Ts)>}
    {}

    template<class From, class To>
    friend consteval std::size_t detail::subset_reindex(std::size_t index) noexcept;
};

// -------------------------------------------------

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
[[nodiscard]] constexpr bool holds_alternative(rvariant<Ts...> const& v) noexcept = delete;

template<class T, class... Ts>
[[nodiscard]] constexpr bool holds_alternative(rvariant<Ts...> const& v) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return v.index() == I;
}

// -------------------------------------------------

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>>&
get(rvariant<Ts...>& v IRIS_LIFETIMEBOUND)
{
    static_assert(I < sizeof...(Ts));
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>>&&
get(rvariant<Ts...>&& v IRIS_LIFETIMEBOUND)  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    static_assert(I < sizeof...(Ts));
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>> const&
get(rvariant<Ts...> const& v IRIS_LIFETIMEBOUND)
{
    static_assert(I < sizeof...(Ts));
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>> const&&
get(rvariant<Ts...> const&& v IRIS_LIFETIMEBOUND)
{
    static_assert(I < sizeof...(Ts));
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<class T, class... Ts>
[[nodiscard]] constexpr T&
get(rvariant<Ts...>& v IRIS_LIFETIMEBOUND)
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<class T, class... Ts>
[[nodiscard]] constexpr T&&
get(rvariant<Ts...>&& v IRIS_LIFETIMEBOUND)  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<class T, class... Ts>
[[nodiscard]] constexpr T const&
get(rvariant<Ts...> const& v IRIS_LIFETIMEBOUND)
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<class T, class... Ts>
[[nodiscard]] constexpr T const&&
get(rvariant<Ts...> const&& v IRIS_LIFETIMEBOUND)
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    if (v.index() == I) {
        return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&&>(v)));
    }
    detail::throw_bad_variant_access();
}

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T& get(rvariant<Ts...>&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T&& get(rvariant<Ts...>&&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T const& get(rvariant<Ts...> const&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T const&& get(rvariant<Ts...> const&&) = delete;

// -------------------------------------------------

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>>&
unsafe_get(rvariant<Ts...>& v IRIS_LIFETIMEBOUND) noexcept
{
    static_assert(I < sizeof...(Ts));
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&>(v)));
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>>&&
unsafe_get(rvariant<Ts...>&& v IRIS_LIFETIMEBOUND) noexcept  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    static_assert(I < sizeof...(Ts));
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&&>(v)));
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>> const&
unsafe_get(rvariant<Ts...> const& v IRIS_LIFETIMEBOUND) noexcept
{
    static_assert(I < sizeof...(Ts));
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&>(v)));
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr variant_alternative_t<I, rvariant<Ts...>> const&&
unsafe_get(rvariant<Ts...> const&& v IRIS_LIFETIMEBOUND) noexcept
{
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&&>(v)));
}

template<class T, class... Ts>
[[nodiscard]] constexpr T&
unsafe_get(rvariant<Ts...>& v IRIS_LIFETIMEBOUND) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&>(v)));
}

template<class T, class... Ts>
[[nodiscard]] constexpr T&&
unsafe_get(rvariant<Ts...>&& v IRIS_LIFETIMEBOUND) noexcept  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&&>(v)));
}

template<class T, class... Ts>
[[nodiscard]] constexpr T const&
unsafe_get(rvariant<Ts...> const& v IRIS_LIFETIMEBOUND) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&>(v)));
}

template<class T, class... Ts>
[[nodiscard]] constexpr T const&&
unsafe_get(rvariant<Ts...> const&& v IRIS_LIFETIMEBOUND) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&&>(v)));
}

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T& unsafe_get(rvariant<Ts...>&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T&& unsafe_get(rvariant<Ts...>&&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T const& unsafe_get(rvariant<Ts...> const&) = delete;

template<class T, class... Ts>
    requires is_recursive_wrapper_v<T>
constexpr T const&& unsafe_get(rvariant<Ts...> const&&) = delete;

// ---------------------------------------------

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr std::add_pointer_t<variant_alternative_t<I, rvariant<Ts...>>>
get_if(rvariant<Ts...>* v) noexcept
{
    static_assert(I < sizeof...(Ts));
    return v && v->index() == I
        ? std::addressof(unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...>&>(*v))))
        : nullptr;
}

template<std::size_t I, class... Ts>
[[nodiscard]] constexpr std::add_pointer_t<variant_alternative_t<I, rvariant<Ts...>> const>
get_if(rvariant<Ts...> const* v) noexcept
{
    static_assert(I < sizeof...(Ts));
    return v && v->index() == I
        ? std::addressof(unwrap_recursive(detail::raw_get<I>(detail::forward_storage<rvariant<Ts...> const&>(*v))))
        : nullptr;
}

template<class T, class... Ts>
[[nodiscard]] constexpr std::add_pointer_t<T>
get_if(rvariant<Ts...>* v) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return iris::get_if<I>(v);
}

template<class T, class... Ts>
[[nodiscard]] constexpr std::add_pointer_t<T const>
get_if(rvariant<Ts...> const* v) noexcept
{
    constexpr std::size_t I = detail::exactly_once_index_v<T, rvariant<Ts...>>;
    return iris::get_if<I>(v);
}

// -------------------------------------------

namespace detail {

template<class R, class Compare, class... Ts>
struct relops_visitor
{
    static_assert(sizeof...(Ts) > 0);

    // Exception specification of the whole visitation, shared with the comparison operators
    static constexpr bool is_noexcept = std::conjunction_v<is_nothrow_directly_invocable_r<R, Compare, Ts const&, Ts const&>...>;

    using Storage = make_variadic_union_t<Ts...>;
    Storage const& v_storage;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    template<std::size_t i, class T>
    [[nodiscard]] IRIS_FORCEINLINE constexpr R operator()(std::in_place_index_t<i>, T const& w_alt) const
        noexcept(std::disjunction_v<
            std::bool_constant<i == std::variant_npos>,
            is_nothrow_directly_invocable_r<R, Compare, T const&, T const&>
        >)
    {
        if constexpr (i != std::variant_npos) {
            return Compare{}(detail::raw_get<i>(v_storage), w_alt);
        } else {
            (void)w_alt;
            return Compare{}(0, 0);
        }
    }
};

} // detail


template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::equal_to<>, Ts> && ...)
[[nodiscard]] constexpr bool operator==(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::equal_to<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::equal_to<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    // Not `vi == wi && ...`; MSVC normalizes the result of `&&` to 0 or 1 once again
    if (vi != wi) return false;
    return detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_});
}

template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::not_equal_to<>, Ts> && ...)
[[nodiscard]] constexpr bool operator!=(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::not_equal_to<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::not_equal_to<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    // Not `vi != wi || ...`; MSVC normalizes the result of `||` to 0 or 1 once again
    if (vi != wi) return true;
    return detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_});
}

template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::less<>, Ts> && ...)
[[nodiscard]] constexpr bool operator<(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::less<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::less<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);

    // Optimization technique for the expression below.
    //   return (vi < wi) || (vi == wi && do_comp(v, w));
    //
    // Using `|` forces compiler to emit conditional move, reduces
    // branch count by 1, making it 2x faster on trivial types.
    // Note that `&&` cannot be `&` because doing so would make it
    // not short-circuit, violating the precondition on visitation
    // table access.
    //
    // Interestingly, MSVC's `std::variant` emits well-optimized
    // code even without this technique. However, surprisingly,
    // that's NOT because `std::variant` is well-optimized, but
    // instead, it's because it is NOT optimal.
    //
    // When the compiler sees access to MSVC's `std::variant`,
    // the compiler is smart enough to assume that `std::variant` is
    // some sort of *opaque* layout (because MSVC's implementation is
    // not standard-layout even for standard-layout alternatives, and
    // also having many other undesirable characteristics in asm level).
    //
    // Memory access to such opaque type leads to a rather
    // conservative control flow that preliminarily "guards" the
    // vi==wi case, effectively reducing the branch count by 1.
    //
    // However, our implementation has much better characteristics
    // where the compiler assumes it's some struct-like layout,
    // enabling more aggressive optimization, which actually
    // introduces extra branch (unfortunately).
    return (vi < wi) |
        ((vi == wi) && detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_}));
}

template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::greater<>, Ts> && ...)
[[nodiscard]] constexpr bool operator>(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::greater<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::greater<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    return (vi > wi) |
        ((vi == wi) && detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_}));
}

template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::less_equal<>, Ts> && ...)
[[nodiscard]] constexpr bool operator<=(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::less_equal<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::less_equal<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    return (vi < wi) |
        ((vi == wi) && detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_}));
}

template<class... Ts>
    requires (cmp::relop_bool_expr_v<std::greater_equal<>, Ts> && ...)
[[nodiscard]] constexpr bool operator>=(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<bool, std::greater_equal<>, Ts...>::is_noexcept)
{
    using Visitor = detail::relops_visitor<bool, std::greater_equal<>, Ts...>;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    return (vi > wi) |
        ((vi == wi) && detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_}));
}


template<class... Ts>
    requires (std::three_way_comparable<Ts> && ...)
[[nodiscard]] IRIS_FORCEINLINE constexpr std::common_comparison_category_t<std::compare_three_way_result_t<Ts>...>
operator<=>(rvariant<Ts...> const& v, rvariant<Ts...> const& w)
    noexcept(detail::relops_visitor<
        std::common_comparison_category_t<std::compare_three_way_result_t<Ts>...>,
        std::compare_three_way,
        Ts...
    >::is_noexcept)
{
    using Visitor = detail::relops_visitor<
        std::common_comparison_category_t<std::compare_three_way_result_t<Ts>...>,
        std::compare_three_way,
        Ts...
    >;
    auto const vi = detail::valueless_bias<rvariant<Ts...>>(v.index_);
    auto const wi = detail::valueless_bias<rvariant<Ts...>>(w.index_);
    auto const comp = vi <=> wi;
    return comp != 0 ? comp : detail::raw_visit_i<Visitor::is_noexcept>(wi, w, Visitor{v.storage_});
}

}  // iris


namespace std {

// https://eel.is/c++draft/variant.hash
template<class... Ts>
    requires (::iris::is_hash_enabled_v<std::remove_const_t<Ts>> && ...)
struct hash<::iris::rvariant<Ts...>>  // NOLINT(cert-dcl58-cpp)
{
    [[nodiscard]] static /* constexpr */ std::size_t operator()(::iris::rvariant<Ts...> const& v)
        noexcept(std::conjunction_v<::iris::is_nothrow_hashable<std::remove_const_t<Ts>>...>)
    {
        return ::iris::detail::raw_visit(v, []<std::size_t i, class T>(std::in_place_index_t<i>, T const& t) static
            noexcept(std::disjunction_v<
                std::bool_constant<i == std::variant_npos>,
                ::iris::is_nothrow_hashable<T>
            >)
        {
            if constexpr (i == std::variant_npos) {
                // Arbitrary value. Might be better to not use common values like
                // `0` or `-1`, because some hash implementations yield the re-interpreted
                // integral representation for fundamental types.
                (void)t;
                return 0xbaddeadbeefuz;

            } else {
                // Assume x64 for the description below. This assumption is solely for
                // demonstration, and the issue described below applies to any architecture.
                //
                // Let `Int` denote a strong typedef of `int` such that:
                //   -- The specialization `std::hash<Int>` is _enabled_ ([unord.hash]), and
                //   -- such specialization yields the same value as the underlying type.
                //
                // Statement 1.
                // For any standard library implementation,
                //   hash{}(variant<int, Int>{std::in_place_type<int>, 0}) ==
                //   hash{}(variant<int, Int>{std::in_place_type<Int>, 0})
                // yields false-positive `true` if `v.index()` is not hash-mixed.
                //
                // Statement 2.
                // Additionally, for any standard library implementation where
                //   hash{}(int(0)) == hash(unsigned(0)) // true in GCC/Clang/MSVC
                // is true, then:
                //   hash{}(variant<int, unsigned>{std::in_place_type<int>, 0}) ==
                //   hash{}(variant<int, unsigned>{std::in_place_type<unsigned>, 0})
                // yields false-positive `true` if `v.index()` is not hash-mixed.
                //
                // Statement 3.
                // Furthermore, for any standard library implementation where
                // the hash function does not consider the type's actual bit width, i.e.:
                //   hash{}(int(0)) == hash{}(long long(0)) // true in GCC/Clang, false in MSVC
                // the following expression:
                //   hash{}(variant<int, long long>{std::in_place_type<int>, 0}) ==
                //   hash{}(variant<int, long long>{std::in_place_type<long long>, 0})
                // yields false-positive `true` if `v.index()` is not hash-mixed.
                //
                // For the statements 1 and 2, one may embrace the status quo and just live
                // with hash collisions. However, for the statement 3, it is actually
                // HARMFUL because an end-user will face observable performance issues
                // just by switching their compiler from MSVC to GCC/Clang.
                //
                // Demo: https://godbolt.org/z/aKhs4vbco
                //
                // All above issues can be eliminated by simply returning
                //   `HC(v.index(), GET<v.index()>(v))`
                // where `HC` is an arbitrary hash mixer function.

                // We assume `hash_combine` is unnecessary here, since the collision
                // is very unlikely to occur as long as the `index_hash` is NOT
                // evaluated as the re-interpreted bit representation.
                constexpr std::size_t index_hash = ::iris::FNV_hash<>::hash(i);
                return index_hash + std::hash<T>{}(t);
            }
        });
    }
};

} // std


namespace iris {

template<class... Ts>
    requires (is_hash_enabled_v<std::remove_const_t<Ts>> && ...)
[[nodiscard]] std::size_t hash_value(rvariant<Ts...> const& v)
    noexcept(std::conjunction_v<is_nothrow_hashable<std::remove_const_t<Ts>>...>)
{
    return std::hash<rvariant<Ts...>>{}(v);
}

} // iris

#undef IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_BEGIN
#undef IRIS_RVARIANT_DISABLE_UNINITIALIZED_WARNING_END

#undef IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_BEGIN
#undef IRIS_RVARIANT_ALWAYS_THROWING_UNREACHABLE_END

#endif

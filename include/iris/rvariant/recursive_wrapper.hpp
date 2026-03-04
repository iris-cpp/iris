#ifndef IRIS_ZZ_RVARIANT_RECURSIVE_WRAPPER_HPP
#define IRIS_ZZ_RVARIANT_RECURSIVE_WRAPPER_HPP

// SPDX-License-Identifier: MIT

#include <iris/bits/specialization_of.hpp>

#include <iris/hash.hpp>
#include <iris/indirect.hpp>

#include <compare>
#include <initializer_list>
#include <memory>
#include <utility>

namespace iris {

template<class T>
class recursive_wrapper;

template<class T, class Allocator>
class recursive_wrapper_alloca;

// recursive_wrapper (fixed to `std::allocator<T>`)
// This class covers 99.99% of recursive-variant use cases.
template<class T>
class recursive_wrapper
    : private detail::indirect_base<T, std::allocator<T>>
{
    static_assert(
        !is_ttp_specialization_of_v<T, recursive_wrapper> && !is_ttp_specialization_of_v<T, recursive_wrapper_alloca>,
        "recursive wrapper of recursive wrapper is not allowed"
    );

    // Note: this implementation is copied from `recursive_wrapper_alloca` below.
    // If any changes are required, modify `recursive_wrapper_alloca` first.
    // See https://github.com/iris-cpp/iris/issues/43 for rationale

    using base_type = detail::indirect_base<T, std::allocator<T>>;

public:
    using typename base_type::allocator_type;
    using typename base_type::const_pointer;
    using typename base_type::pointer;
    using typename base_type::value_type;

    // Don't do this; it breaks third-party analyzer like ReSharper on MSVC
    //using base_type::base_type;

    constexpr /* not explicit */ recursive_wrapper() = default;

    // Required for combination with defaulted assignment operators
    constexpr recursive_wrapper(recursive_wrapper const&) = default;
    constexpr recursive_wrapper(recursive_wrapper&&) noexcept = default;

    // Converting constructor
    template<class U = T>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, recursive_wrapper>) &&
            (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
            //std::is_constructible_v<T, U> // UNIMPLEMENTABLE for recursive types; instantiates infinitely
            std::is_convertible_v<U, T>
    constexpr /* not explicit */ recursive_wrapper(U&& u)
        // This overload is never `noexcept` as it always allocates.
        // Note that conditionally enabling `noexcept` will lead to
        // recursive instantiation in some situations.
        : base_type(std::forward<U>(u))
    {}

    template<class... Us>
        requires
            std::is_constructible_v<T, Us...>
    constexpr explicit recursive_wrapper(std::in_place_t, Us&&... us)
        noexcept(noexcept(base_type(std::in_place, std::forward<Us>(us)...)))
        : base_type(std::in_place, std::forward<Us>(us)...)
    {}

    template<class I, class... Us>
        requires
            std::is_constructible_v<T, std::initializer_list<I>&, Us...>
    constexpr explicit recursive_wrapper(std::in_place_t, std::initializer_list<I> il, Us&&... us)
        noexcept(noexcept(base_type(std::in_place, il, std::forward<Us>(us)...)))
        : base_type(std::in_place, il, std::forward<Us>(us)...)
    {}

    constexpr ~recursive_wrapper() = default;

    // Don't do this; it will lead to surprising result that
    // MSVC attempts to instantiate move assignment operator of *rvariant*
    // when a user just *defines* a struct that contains a rvariant.
    // I don't know why it happens, but MSVC is certainly doing something weird
    // so that it eagerly instantiates unrelated member functions.
    //using base_type::operator=;

    constexpr recursive_wrapper& operator=(recursive_wrapper const&) = default;

    constexpr recursive_wrapper& operator=(recursive_wrapper&&) noexcept = default;

    // This is required for proper delegation; otherwise constructor will be called
    template<class U = T>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, recursive_wrapper>) &&
            std::is_constructible_v<T, U> &&
            std::is_assignable_v<T&, U>
    constexpr recursive_wrapper& operator=(U&& u)
    {
        base_type::operator=(std::forward<U>(u));
        return *this;
    }

    using base_type::operator*;
    using base_type::operator->;
    using base_type::valueless_after_move;
    using base_type::get_allocator;

    using base_type::swap;

    friend constexpr void swap(recursive_wrapper& lhs, recursive_wrapper& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }
};

// "Allocator-aware" recursive_wrapper
template<class T, class Allocator = std::allocator<T>>
class recursive_wrapper_alloca
    : private detail::indirect_base<T, Allocator>
{
    static_assert(
        !is_ttp_specialization_of_v<T, recursive_wrapper> && !is_ttp_specialization_of_v<T, recursive_wrapper_alloca>,
        "recursive wrapper of recursive wrapper is not allowed"
    );

    using base_type = detail::indirect_base<T, Allocator>;

public:
    using typename base_type::allocator_type;
    using typename base_type::const_pointer;
    using typename base_type::pointer;
    using typename base_type::value_type;

    // Don't do this; it breaks third-party analyzer like ReSharper on MSVC
    //using base_type::base_type;

    constexpr /* not explicit */ recursive_wrapper_alloca()
        requires std::is_default_constructible_v<Allocator>
    = default;

    // Required for combination with defaulted assignment operators
    constexpr recursive_wrapper_alloca(recursive_wrapper_alloca const&) = default;
    constexpr recursive_wrapper_alloca(recursive_wrapper_alloca&&) noexcept = default;

    constexpr explicit recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a)
        noexcept(noexcept(base_type(std::allocator_arg, a)))
        : base_type(std::allocator_arg, a)
    {}

    constexpr recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a, recursive_wrapper_alloca const& other)
        noexcept(noexcept(base_type(std::allocator_arg, a, other)))
        : base_type(std::allocator_arg, a, other)
    {}

    constexpr recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a, recursive_wrapper_alloca&& other)
        noexcept(noexcept(base_type(std::allocator_arg, a, std::move(other))))
        : base_type(std::allocator_arg, a, std::move(other))
    {}

    // Converting constructor
    template<class U = T>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, recursive_wrapper_alloca>) &&
            (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
            std::is_default_constructible_v<Allocator> &&
            //std::is_constructible_v<T, U> // UNIMPLEMENTABLE for recursive types; instantiates infinitely
            std::is_convertible_v<U, T>
    constexpr /* not explicit */ recursive_wrapper_alloca(U&& u)
        // This overload is never `noexcept` as it always allocates.
        // Note that conditionally enabling `noexcept` will lead to
        // recursive instantiation in some situations.
        : base_type(std::forward<U>(u))
    {}

    template<class U = T>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, recursive_wrapper_alloca>) &&
            (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
            std::is_constructible_v<T, U>
    constexpr explicit recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a, U&& u)
        noexcept(noexcept(base_type(std::allocator_arg, a, std::forward<U>(u))))
        : base_type(std::allocator_arg, a, std::forward<U>(u))
    {}

    template<class... Us>
        requires
            std::is_constructible_v<T, Us...> &&
            std::is_default_constructible_v<Allocator>
    constexpr explicit recursive_wrapper_alloca(std::in_place_t, Us&&... us)
        noexcept(noexcept(base_type(std::in_place, std::forward<Us>(us)...)))
        : base_type(std::in_place, std::forward<Us>(us)...)
    {}

    template<class... Us>
        requires std::is_constructible_v<T, Us...>
    constexpr explicit recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a, std::in_place_t, Us&&... us)
        noexcept(noexcept(base_type(std::allocator_arg, a, std::in_place, std::forward<Us>(us)...)))
        : base_type(std::allocator_arg, a, std::in_place, std::forward<Us>(us)...)
    {}

    template<class I, class... Us>
        requires
            std::is_constructible_v<T, std::initializer_list<I>&, Us...> &&
            std::is_default_constructible_v<Allocator>
    constexpr explicit recursive_wrapper_alloca(std::in_place_t, std::initializer_list<I> il, Us&&... us)
        noexcept(noexcept(base_type(std::in_place, il, std::forward<Us>(us)...)))
        : base_type(std::in_place, il, std::forward<Us>(us)...)
    {}

    template<class I, class... Us>
        requires std::is_constructible_v<T, std::initializer_list<I>&, Us...>
    constexpr explicit recursive_wrapper_alloca(std::allocator_arg_t, Allocator const& a, std::in_place_t, std::initializer_list<I> il, Us&&... us)
        noexcept(noexcept(base_type(std::allocator_arg, a, std::in_place, il, std::forward<Us>(us)...)))
        : base_type(std::allocator_arg, a, std::in_place, il, std::forward<Us>(us)...)
    {}

    constexpr ~recursive_wrapper_alloca() = default;

    // Don't do this; it will lead to surprising result that
    // MSVC attempts to instantiate move assignment operator of *rvariant*
    // when a user just *defines* a struct that contains a rvariant.
    // I don't know why it happens, but MSVC is certainly doing something weird
    // so that it eagerly instantiates unrelated member functions.
    //using base_type::operator=;

    constexpr recursive_wrapper_alloca& operator=(recursive_wrapper_alloca const&) = default;

    constexpr recursive_wrapper_alloca& operator=(recursive_wrapper_alloca&&) noexcept(
        std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
        std::allocator_traits<Allocator>::is_always_equal::value
    ) = default;

    // This is required for proper delegation; otherwise constructor will be called
    template<class U = T>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, recursive_wrapper_alloca>) &&
            std::is_constructible_v<T, U> &&
            std::is_assignable_v<T&, U>
    constexpr recursive_wrapper_alloca& operator=(U&& u)
    {
        base_type::operator=(std::forward<U>(u));
        return *this;
    }

    using base_type::operator*;
    using base_type::operator->;
    using base_type::valueless_after_move;
    using base_type::get_allocator;

    using base_type::swap;

    friend constexpr void swap(recursive_wrapper_alloca& lhs, recursive_wrapper_alloca& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }
};

// ---------------------------------------------------
// recursive_wrapper

template<class Value>
recursive_wrapper(Value)
    -> recursive_wrapper<Value>;

template<class T, class U>
[[nodiscard]] constexpr bool operator==(recursive_wrapper<T> const& lhs, recursive_wrapper<U> const& rhs)
    noexcept(noexcept(*lhs == *rhs))
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return lhs.valueless_after_move() == rhs.valueless_after_move();
    } else [[likely]] {
        return *lhs == *rhs;
    }
}

template<class T, class U>
[[nodiscard]] constexpr bool operator==(recursive_wrapper<T> const& lhs, U const& rhs)
    noexcept(noexcept(*lhs == rhs))
{
    if (lhs.valueless_after_move()) [[unlikely]] {
        return false;
    } else [[likely]] {
        return *lhs == rhs;
    }
}

// ---------------------------------------------------
// recursive_wrapper_alloca

template<class Value>
recursive_wrapper_alloca(Value)
    -> recursive_wrapper_alloca<Value>;

template<class Allocator, class Value>
recursive_wrapper_alloca(std::allocator_arg_t, Allocator, Value)
    -> recursive_wrapper_alloca<Value, typename std::allocator_traits<Allocator>::template rebind_alloc<Value>>;

template<class T, class TA, class U, class UA>
[[nodiscard]] constexpr bool operator==(recursive_wrapper_alloca<T, TA> const& lhs, recursive_wrapper_alloca<U, UA> const& rhs)
    noexcept(noexcept(*lhs == *rhs))
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return lhs.valueless_after_move() == rhs.valueless_after_move();
    } else [[likely]] {
        return *lhs == *rhs;
    }
}

template<class T, class A, class U>
[[nodiscard]] constexpr bool operator==(recursive_wrapper_alloca<T, A> const& lhs, U const& rhs)
    noexcept(noexcept(*lhs == rhs))
{
    if (lhs.valueless_after_move()) [[unlikely]] {
        return false;
    } else [[likely]] {
        return *lhs == rhs;
    }
}

namespace detail {

// These cannot be overloaded with the same function name, as it
// breaks MSVC's overload resolution on recursive types (possibly bug)

// ---------------------------------------------------
// recursive_wrapper

template<class T, class U>
[[nodiscard]] constexpr auto rw_three_way_impl_00(recursive_wrapper<T> const& lhs, recursive_wrapper<U> const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return !lhs.valueless_after_move() <=> !rhs.valueless_after_move();
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, *rhs);
    }
}

template<class T, class U>
[[nodiscard]] constexpr auto rw_three_way_impl_01(recursive_wrapper<T> const& lhs, U const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move()) [[unlikely]] {
        return std::strong_ordering::less;
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, rhs);
    }
}

// ---------------------------------------------------
// recursive_wrapper_alloca

template<class T, class TA, class U, class UA>
[[nodiscard]] constexpr auto rw_three_way_impl_00(recursive_wrapper_alloca<T, TA> const& lhs, recursive_wrapper_alloca<U, UA> const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return !lhs.valueless_after_move() <=> !rhs.valueless_after_move();
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, *rhs);
    }
}

template<class T, class A, class U>
[[nodiscard]] constexpr auto rw_three_way_impl_01(recursive_wrapper_alloca<T, A> const& lhs, U const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move()) [[unlikely]] {
        return std::strong_ordering::less;
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, rhs);
    }
}

} // detail


// ---------------------------------------------------
// recursive_wrapper

template<class T, class U>
[[nodiscard]] constexpr auto operator<=>(recursive_wrapper<T> const& lhs, recursive_wrapper<U> const& rhs)
    // no explicit return type
{
    return detail::rw_three_way_impl_00(lhs, rhs);
}

template<class T, class U>
[[nodiscard]] constexpr auto operator<=>(recursive_wrapper<T> const& lhs, U const& rhs)
    // no explicit return type
{
    return detail::rw_three_way_impl_01(lhs, rhs);
}

// ---------------------------------------------------
// recursive_wrapper_alloca

template<class T, class TA, class U, class UA>
[[nodiscard]] constexpr auto operator<=>(recursive_wrapper_alloca<T, TA> const& lhs, recursive_wrapper_alloca<U, UA> const& rhs)
    // no explicit return type
{
    return detail::rw_three_way_impl_00(lhs, rhs);
}

template<class T, class A, class U>
[[nodiscard]] constexpr auto operator<=>(recursive_wrapper_alloca<T, A> const& lhs, U const& rhs)
    // no explicit return type
{
    return detail::rw_three_way_impl_01(lhs, rhs);
}

}  // iris

namespace std {

// ---------------------------------------------------
// recursive_wrapper

template<class T>
    requires ::iris::is_hash_enabled_v<T>
struct hash<::iris::recursive_wrapper<T>>  // NOLINT(cert-dcl58-cpp)
{
    [[nodiscard]] static size_t operator()(::iris::recursive_wrapper<T> const& obj)
        noexcept(::iris::is_nothrow_hashable_v<T>)
    {
        if (obj.valueless_after_move()) [[unlikely]] {
            return 0xbaddeadbeefuz;
        } else [[likely]] {
            return std::hash<T>{}(*obj);
        }
    }
};

// ---------------------------------------------------
// recursive_wrapper_alloca

template<class T, class Allocator>
    requires ::iris::is_hash_enabled_v<T>
struct hash<::iris::recursive_wrapper_alloca<T, Allocator>>  // NOLINT(cert-dcl58-cpp)
{
    [[nodiscard]] static size_t operator()(::iris::recursive_wrapper_alloca<T, Allocator> const& obj)
        noexcept(::iris::is_nothrow_hashable_v<T>)
    {
        if (obj.valueless_after_move()) [[unlikely]] {
            return 0xbaddeadbeefuz;
        } else [[likely]] {
            return std::hash<T>{}(*obj);
        }
    }
};

} // std


namespace iris {

// ---------------------------------------------------
// recursive_wrapper

template<class T>
    requires is_hash_enabled_v<T>
[[nodiscard]] std::size_t hash_value(recursive_wrapper<T> const& obj)
    noexcept(is_nothrow_hashable_v<T>)
{
    return std::hash<recursive_wrapper<T>>{}(obj);
}

// ---------------------------------------------------
// recursive_wrapper_alloca

template<class T, class Allocator>
    requires is_hash_enabled_v<T>
[[nodiscard]] std::size_t hash_value(recursive_wrapper_alloca<T, Allocator> const& obj)
    noexcept(is_nothrow_hashable_v<T>)
{
    return std::hash<recursive_wrapper_alloca<T, Allocator>>{}(obj);
}

} // iris

#endif

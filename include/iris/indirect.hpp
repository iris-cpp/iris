#ifndef IRIS_ZZ_INDIRECT_HPP
#define IRIS_ZZ_INDIRECT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/bits/indirect_base.hpp>
#include <iris/compare.hpp>
#include <iris/hash.hpp>

#include <compare>
#include <memory>
#include <type_traits>
#include <utility>

namespace iris {

// Polyfill for the C++26 `std::indirect`
// https://eel.is/c++draft/indirect
template<class T, class Allocator = std::allocator<T>>
class indirect : public detail::indirect_base<indirect<T, Allocator>, T, Allocator>
{
public:
    using detail::indirect_base<indirect, T, Allocator>::indirect_base;
};

template<class Value>
indirect(Value)
    -> indirect<Value>;

template<class Value, class Allocator>
indirect(std::allocator_arg_t, Allocator, Value)
    -> indirect<Value, typename std::allocator_traits<Allocator>::template rebind_alloc<Value>>;


template<class T, class Allocator, class U, class AA>
[[nodiscard]] constexpr bool operator==(indirect<T, Allocator> const& lhs, indirect<U, AA> const& rhs)
    noexcept(noexcept(*lhs == *rhs))
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return lhs.valueless_after_move() == rhs.valueless_after_move();
    } else [[likely]] {
        return *lhs == *rhs;
    }
}

template<class T, class Allocator, class U>
[[nodiscard]] constexpr bool operator==(indirect<T, Allocator> const& lhs, U const& rhs)
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

template<class T, class Allocator, class U, class AA>
[[nodiscard]] constexpr auto indirect_three_way_impl_00(indirect<T, Allocator> const& lhs, indirect<U, AA> const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move() || rhs.valueless_after_move()) [[unlikely]] {
        return !lhs.valueless_after_move() <=> !rhs.valueless_after_move();
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, *rhs);
    }
}

template<class T, class A, class U>
[[nodiscard]] constexpr auto indirect_three_way_impl_01(indirect<T, A> const& lhs, U const& rhs)
    -> cmp::synth_three_way_result<T, U>
{
    if (lhs.valueless_after_move()) [[unlikely]] {
        return std::strong_ordering::less;
    } else [[likely]] {
        return cmp::synth_three_way{}(*lhs, rhs);
    }
}

} // detail

template<class T, class Allocator, class U, class AA>
[[nodiscard]] constexpr auto operator<=>(indirect<T, Allocator> const& lhs, indirect<U, AA> const& rhs)
    // no explicit return type
{
    return detail::indirect_three_way_impl_00(lhs, rhs);
}

template<class T, class A, class U>
[[nodiscard]] constexpr auto operator<=>(indirect<T, A> const& lhs, U const& rhs)
    // no explicit return type
{
    return detail::indirect_three_way_impl_01(lhs, rhs);
}

}  // iris

template<class T, class Allocator>
    requires ::iris::is_hash_enabled_v<T>
struct std::hash<::iris::indirect<T, Allocator>>
{
    [[nodiscard]] static size_t operator()(::iris::indirect<T, Allocator> const& obj)
        noexcept(::iris::is_nothrow_hashable_v<T>)
    {
        if (obj.valueless_after_move()) [[unlikely]] {
            return 0xbaddeadbeefuz;
        } else [[likely]] {
            return std::hash<T>{}(*obj);
        }
    }
};

#endif

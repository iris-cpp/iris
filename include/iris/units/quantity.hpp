#ifndef IRIS_ZZ_UNITS_QUANTITY_HPP
#define IRIS_ZZ_UNITS_QUANTITY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_traits.hpp>

#include <format>
#include <numeric>
#include <algorithm>
#include <functional>
#include <limits>
#include <type_traits>
#include <concepts>
#include <compare> // IWYU pragma: keep
#include <utility>

#include <cmath> // IWYU pragma: keep
#include <cstddef> // IWYU pragma: keep

namespace iris::units {

template<class T>
class quantity;

template<class Derived>
struct quantity_traits;

template<template<class...> class DerivedTT, numeric_arithmetic T, class... Rest>
    requires
        std::derived_from<DerivedTT<T, Rest...>, quantity<T>> &&
        (!std::same_as<DerivedTT<T, Rest...>, quantity<T>>)
struct quantity_traits<DerivedTT<T, Rest...>>
{
    using value_type = T;

    template<class U>
    using rebind = DerivedTT<U, Rest...>;
};

template<class Q>
concept quantity_like =
    requires { typename quantity_traits<Q>::value_type; } &&
    std::derived_from<Q, quantity<typename quantity_traits<Q>::value_type>>;

namespace detail {

template<class First, class... Rest>
concept same_quantity_family =
    quantity_like<First> &&
    (std::same_as<
        typename quantity_traits<First>::template rebind<typename quantity_traits<Rest>::value_type>,
        Rest
    > && ...);

template<class A, class B>
using common_quantity_value_t = std::common_type_t<
    typename quantity_traits<A>::value_type,
    typename quantity_traits<B>::value_type
>;

} // detail

// Declares the deduction guide for a quantity family, so that `DerivedClass{value}`
// deduces `DerivedClass<decltype(value)>`.
// Not required on compilers that implement <http://wg21.link/P2582>.
#define IRIS_QUANTITY_DEDUCTION_GUIDE(class_name, ...) \
    template<::iris::numeric_arithmetic T> \
    class_name(T) -> class_name<T __VA_OPT__(,) __VA_ARGS__>

// Base class for defining a strongly-typed "quantity" class.
//
// The derived class must be a class template whose first parameter is the
// representation type (`T`), and must derive publicly from `quantity<T>`.
//
// Marking the derived class `final` is strongly advised, since deriving from
// a derived quantity has no meaning in this model.
//
// For practical ergonomics, the derived class should provide the following:
//
// 1. `using iris::units::quantity<T>::quantity;`
//    Required. Without it, the class has no usable constructor and the
//    operators fail to instantiate.
//
// 2. `IRIS_QUANTITY_DEDUCTION_GUIDE(class_name);`
//    Enables construction with the syntax `DerivedClass{value}`.
//    Not required on compilers that implement <http://wg21.link/P2582>.
template<class T>
class quantity
{
    static_assert(numeric_arithmetic<T>);
    static_assert(std::same_as<std::remove_cv_t<T>, T>);

public:
    using value_type = T;

    T value{};

    constexpr quantity() noexcept = default;

protected:
    // Using `quantity<T>` without deriving is prohibited as a "quantity" without
    // target domain is meaningless
    constexpr ~quantity() noexcept = default;

public:
    // We need to resurrect defaulted special members since we declare destructor
    constexpr quantity(quantity const&) noexcept = default;
    constexpr quantity(quantity&&) noexcept = default;
    constexpr quantity& operator=(quantity const&) noexcept = default;
    constexpr quantity& operator=(quantity&&) noexcept = default;

    // ----------------------------------------------------

    constexpr explicit quantity(T value) noexcept
        : value(value)
    {}

    [[nodiscard]] constexpr explicit operator T() const noexcept
    {
        return value;
    }

    template<class Self, class Target>
        requires detail::same_quantity_family<Self, Target> && (!std::same_as<Self, Target>)
    [[nodiscard]] constexpr
    explicit(!std::same_as<
        detail::common_quantity_value_t<Self, Target>,
        typename quantity_traits<Target>::value_type
    >)
    operator Target(this Self const& self) noexcept
    {
        return Target{static_cast<quantity_traits<Target>::value_type>(self.value)};
    }

    // ----------------------------------------------------

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other>
    [[nodiscard]] constexpr bool
    operator==(this Self const& self, Other const& other) noexcept
    {
        using common = detail::common_quantity_value_t<Self, Other>;
        return static_cast<common>(self.value) == static_cast<common>(other.value);
    }

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other>
    [[nodiscard]] constexpr auto
    operator<=>(this Self const& self, Other const& other) noexcept
    {
        using common = detail::common_quantity_value_t<Self, Other>;
        return static_cast<common>(self.value) <=> static_cast<common>(other.value);
    }

    // ----------------------------------------------------

    template<class Self>
    [[nodiscard]] constexpr Self operator+(this Self const& self) noexcept
    {
        return self;
    }

    template<class Self>
    [[nodiscard]] constexpr Self operator-(this Self const& self) noexcept
    {
        return Self{static_cast<quantity_traits<Self>::value_type>(-self.value)};
    }

    template<class Self>
        requires std::integral<typename quantity_traits<Self>::value_type>
    constexpr Self& operator++(this Self& self) noexcept
    {
        ++self.value;
        return self;
    }

    template<class Self>
        requires std::integral<typename quantity_traits<Self>::value_type>
    constexpr Self operator++(this Self& self, int) noexcept
    {
        return Self{self.value++};
    }

    template<class Self>
        requires std::integral<typename quantity_traits<Self>::value_type>
    constexpr Self& operator--(this Self& self) noexcept
    {
        --self.value;
        return self;
    }

    template<class Self>
        requires std::integral<typename quantity_traits<Self>::value_type>
    constexpr Self operator--(this Self& self, int) noexcept
    {
        return Self{self.value--};
    }

    // ----------------------------------------------------

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator+(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<quantity_traits<result>::value_type>(self.value + other.value)};
    }

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other> &&
                 std::same_as<detail::common_quantity_value_t<Self, Other>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator+=(this Self& self, Other const& other) noexcept
    {
        self.value += other.value;
        return self;
    }

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator-(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<quantity_traits<result>::value_type>(self.value - other.value)};
    }

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other> &&
                 std::same_as<detail::common_quantity_value_t<Self, Other>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator-=(this Self& self, Other const& other) noexcept
    {
        self.value -= other.value;
        return self;
    }

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
        requires std::same_as<std::common_type_t<typename quantity_traits<Self>::value_type, U>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator*=(this Self& self, U scalar) noexcept
    {
        self.value *= scalar;
        return self;
    }

    template<class Self, numeric_arithmetic U>
        requires std::same_as<std::common_type_t<typename quantity_traits<Self>::value_type, U>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator/=(this Self& self, U scalar) noexcept
    {
        self.value /= scalar;
        return self;
    }

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
    [[nodiscard]] constexpr auto operator*(this Self const& self, U scalar) noexcept
        -> quantity_traits<Self>::template rebind<std::common_type_t<typename quantity_traits<Self>::value_type, U>>
    {
        using common = std::common_type_t<typename quantity_traits<Self>::value_type, U>;
        return typename quantity_traits<Self>::template rebind<common>{static_cast<common>(self.value * scalar)};
    }

    // Scalar on the left (cannot use explicit object parameter)
    template<numeric_arithmetic U, class Q>
        requires std::derived_from<Q, quantity>
    [[nodiscard]] friend constexpr auto operator*(U scalar, Q const& q) noexcept
        -> quantity_traits<Q>::template rebind<std::common_type_t<typename quantity_traits<Q>::value_type, U>>
    {
        using common = std::common_type_t<typename quantity_traits<Q>::value_type, U>;
        return typename quantity_traits<Q>::template rebind<common>{static_cast<common>(scalar * q.value)};
    }

    template<class Self, quantity_like Other>
    void operator*(this Self const&, Other const&) = delete;

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
    [[nodiscard]] constexpr auto operator/(this Self const& self, U scalar) noexcept
        -> quantity_traits<Self>::template rebind<std::common_type_t<typename quantity_traits<Self>::value_type, U>>
    {
        using common = std::common_type_t<typename quantity_traits<Self>::value_type, U>;
        return typename quantity_traits<Self>::template rebind<common>{static_cast<common>(self.value / scalar)};
    }

    template<class Self, class Other>
        requires detail::same_quantity_family<Self, Other>
    [[nodiscard]] constexpr detail::common_quantity_value_t<Self, Other>
    operator/(this Self const& self, Other const& other) noexcept
    {
        return static_cast<detail::common_quantity_value_t<Self, Other>>(self.value / other.value);
    }

    template<numeric_arithmetic U, class Q>
        requires std::derived_from<Q, quantity>
    friend void operator/(U, Q const&) = delete;

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
        requires
            std::integral<typename quantity_traits<Self>::value_type> && std::integral<U> &&
            std::same_as<std::common_type_t<typename quantity_traits<Self>::value_type, U>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator%=(this Self& self, U scalar) noexcept
    {
        self.value %= scalar;
        return self;
    }

    template<class Self, class Other>
        requires
            detail::same_quantity_family<Self, Other> &&
            std::integral<typename quantity_traits<Self>::value_type> &&
            std::integral<typename quantity_traits<Other>::value_type> &&
            std::same_as<detail::common_quantity_value_t<Self, Other>, typename quantity_traits<Self>::value_type>
    constexpr Self& operator%=(this Self& self, Other const& other) noexcept
    {
        self.value %= other.value;
        return self;
    }

    template<class Self, numeric_arithmetic U>
        requires std::integral<typename quantity_traits<Self>::value_type> && std::integral<U>
    [[nodiscard]] constexpr auto operator%(this Self const& self, U scalar) noexcept
        -> quantity_traits<Self>::template rebind<std::common_type_t<typename quantity_traits<Self>::value_type, U>>
    {
        using common = std::common_type_t<typename quantity_traits<Self>::value_type, U>;
        return typename quantity_traits<Self>::template rebind<common>{static_cast<common>(self.value % scalar)};
    }

    template<class Self, class Other>
        requires
            detail::same_quantity_family<Self, Other> &&
            std::integral<typename quantity_traits<Self>::value_type> &&
            std::integral<typename quantity_traits<Other>::value_type>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator%(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<quantity_traits<result>::value_type>(self.value % other.value)};
    }

    template<numeric_arithmetic U, class Q>
        requires std::derived_from<Q, quantity>
    friend void operator%(U, Q const&) = delete;
};

} // iris::units

template<
    template<class...> class DerivedTT,
    iris::numeric_arithmetic T, iris::numeric_arithmetic U, class... Rest
>
    requires
        iris::units::quantity_like<DerivedTT<T, Rest...>> &&
        iris::units::quantity_like<DerivedTT<U, Rest...>>
struct std::common_type<DerivedTT<T, Rest...>, DerivedTT<U, Rest...>>
{
    using type = DerivedTT<std::common_type_t<T, U>, Rest...>;
};

// A plain quantity and a derived quantity have no common type
template<class T, class Derived>
    requires
        iris::units::quantity_like<Derived> &&
        std::derived_from<Derived, iris::units::quantity<T>> &&
        (!std::same_as<Derived, iris::units::quantity<T>>)
struct std::common_type<iris::units::quantity<T>, Derived>
{
    // No `::type`
};

// A plain quantity and a derived quantity have no common type
template<class T, class Derived>
    requires
        iris::units::quantity_like<Derived> &&
        std::derived_from<Derived, iris::units::quantity<T>> &&
        (!std::same_as<Derived, iris::units::quantity<T>>)
struct std::common_type<Derived, iris::units::quantity<T>>
{
    // No `::type`
};

template<template<class...> class DerivedTT, iris::numeric_arithmetic T, class... Rest>
    requires iris::units::quantity_like<DerivedTT<T, Rest...>>
class std::numeric_limits<DerivedTT<T, Rest...>> : public std::numeric_limits<T>
{
public:
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> (min)() noexcept
    {
        return DerivedTT<T, Rest...>{(std::numeric_limits<T>::min)()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> (max)() noexcept
    {
        return DerivedTT<T, Rest...>{(std::numeric_limits<T>::max)()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> lowest() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::lowest()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> epsilon() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::epsilon()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> round_error() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::round_error()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> infinity() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::infinity()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> quiet_NaN() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::quiet_NaN()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> signaling_NaN() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::signaling_NaN()};
    }
    [[nodiscard]] static constexpr DerivedTT<T, Rest...> denorm_min() noexcept
    {
        return DerivedTT<T, Rest...>{std::numeric_limits<T>::denorm_min()};
    }
};

namespace iris::units {

template<quantity_like Q>
    requires std::floating_point<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q trunc(Q const& q) noexcept
{
    return Q{std::trunc(q.value)};
}

template<quantity_like Q>
    requires std::integral<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q trunc(Q const& q) noexcept
{
    return q;
}

template<quantity_like Q>
    requires std::floating_point<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q floor(Q const& q) noexcept
{
    return Q{std::floor(q.value)};
}

template<quantity_like Q>
    requires std::integral<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q floor(Q const& q) noexcept
{
    return q;
}

template<quantity_like Q>
    requires std::floating_point<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q ceil(Q const& q) noexcept
{
    return Q{std::ceil(q.value)};
}

template<quantity_like Q>
    requires std::integral<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q ceil(Q const& q) noexcept
{
    return q;
}

template<quantity_like Q>
    requires std::floating_point<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q round(Q const& q) noexcept
{
    return Q{std::round(q.value)};
}

template<quantity_like Q>
    requires std::integral<typename quantity_traits<Q>::value_type>
[[nodiscard]] constexpr Q round(Q const& q) noexcept
{
    return q;
}

template<quantity_like Q>
[[nodiscard]] constexpr Q abs(Q const& q) noexcept
{
    if constexpr (std::unsigned_integral<typename quantity_traits<Q>::value_type>) {
        return q;
    } else {
        return Q{static_cast<quantity_traits<Q>::value_type>(std::abs(q.value))};
    }
}

// --------------------------------------------------

// Returns by value, unlike `std::min`, since the result is in the common
// representation and may not be any of the arguments. `std::min` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like A, quantity_like B>
    requires detail::same_quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
(min)(A const& a, B const& b) noexcept
{
    using result = std::common_type_t<A, B>;
    return (std::min)(static_cast<result>(a), static_cast<result>(b));
}

// Returns by value, unlike `std::min`, since the result is in the common
// representation and may not be any of the arguments. `std::min` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like A, quantity_like B, class Comp>
    requires
        detail::same_quantity_family<A, B> &&
        std::strict_weak_order<Comp&, std::common_type_t<A, B> const&, std::common_type_t<A, B> const&>
[[nodiscard]] constexpr std::common_type_t<A, B>
(min)(A const& a, B const& b, Comp&& comp)
    noexcept(std::is_nothrow_invocable_v<Comp&, std::common_type_t<A, B> const&, std::common_type_t<A, B> const&>)
{
    using result = std::common_type_t<A, B>;
    return (std::min)(static_cast<result>(a), static_cast<result>(b), std::forward<Comp>(comp));
}

// Returns by value, unlike `std::max`, since the result is in the common
// representation and may not be any of the arguments. `std::max` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like A, quantity_like B>
    requires detail::same_quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
(max)(A const& a, B const& b) noexcept
{
    using result = std::common_type_t<A, B>;
    return (std::max)(static_cast<result>(a), static_cast<result>(b));
}

// Returns by value, unlike `std::max`, since the result is in the common
// representation and may not be any of the arguments. `std::max` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like A, quantity_like B, class Comp>
    requires
        detail::same_quantity_family<A, B> &&
        std::strict_weak_order<Comp&, std::common_type_t<A, B> const&, std::common_type_t<A, B> const&>
[[nodiscard]] constexpr std::common_type_t<A, B>
(max)(A const& a, B const& b, Comp&& comp)
    noexcept(std::is_nothrow_invocable_v<Comp&, std::common_type_t<A, B> const&, std::common_type_t<A, B> const&>)
{
    using result = std::common_type_t<A, B>;
    return (std::max)(static_cast<result>(a), static_cast<result>(b), std::forward<Comp>(comp));
}

// Returns by value, unlike `std::clamp`, since the result is in the common
// representation and may not be any of the arguments. `std::clamp` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like Q, quantity_like Lo, quantity_like Hi>
    requires detail::same_quantity_family<Q, Lo, Hi>
[[nodiscard]] constexpr std::common_type_t<Q, Lo, Hi>
clamp(Q const& v, Lo const& lo, Hi const& hi)
{
    using result = std::common_type_t<Q, Lo, Hi>;
    return std::clamp(static_cast<result>(v), static_cast<result>(lo), static_cast<result>(hi));
}

// Returns by value, unlike `std::clamp`, since the result is in the common
// representation and may not be any of the arguments. `std::clamp` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_like Q, quantity_like Lo, quantity_like Hi, class Comp>
    requires
        detail::same_quantity_family<Q, Lo, Hi> &&
        std::strict_weak_order<Comp&, std::common_type_t<Q, Lo, Hi> const&, std::common_type_t<Q, Lo, Hi> const&>
[[nodiscard]] constexpr std::common_type_t<Q, Lo, Hi>
clamp(Q const& v, Lo const& lo, Hi const& hi, Comp&& comp)
    noexcept(std::is_nothrow_invocable_v<Comp&, std::common_type_t<Q, Lo, Hi> const&, std::common_type_t<Q, Lo, Hi> const&>)
{
    using result = std::common_type_t<Q, Lo, Hi>;
    return std::clamp(static_cast<result>(v), static_cast<result>(lo), static_cast<result>(hi), std::forward<Comp>(comp));
}

// ---------------------------------------------------

template<quantity_like A, quantity_like B>
    requires detail::same_quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
midpoint(A a, B b) noexcept
{
    using result = std::common_type_t<A, B>;
    using common = quantity_traits<result>::value_type;
    return result{std::midpoint(static_cast<common>(a.value), static_cast<common>(b.value))};
}

template<quantity_like A, quantity_like B, numeric_arithmetic T>
    requires
        detail::same_quantity_family<A, B> &&
        std::floating_point<typename quantity_traits<std::common_type_t<A, B>>::value_type>
[[nodiscard]] constexpr std::common_type_t<A, B>
lerp(A a, B b, T t) noexcept
{
    using result = std::common_type_t<A, B>;
    using common = quantity_traits<result>::value_type;
    return result{std::lerp(static_cast<common>(a.value), static_cast<common>(b.value), static_cast<common>(t))};
}

} // iris::units

template<iris::units::quantity_like Q>
struct std::hash<Q>
{
    [[nodiscard]] static std::size_t operator()(Q const& q) noexcept
    {
        return std::hash<typename iris::units::quantity_traits<Q>::value_type>{}(q.value);
    }
};

template<iris::units::quantity_like Q, class CharT>
struct std::formatter<Q, CharT> : std::formatter<typename iris::units::quantity_traits<Q>::value_type, CharT>
{
    auto format(Q const& q, auto& ctx) const
    {
        return std::formatter<typename iris::units::quantity_traits<Q>::value_type, CharT>::format(q.value, ctx);
    }
};

#endif

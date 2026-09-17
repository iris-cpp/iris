#ifndef IRIS_ZZ_UNITS_QUANTITY_HPP
#define IRIS_ZZ_UNITS_QUANTITY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/units/traits.hpp>

#include <iris/type_traits.hpp>
#include <iris/math.hpp>

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

template<template<class...> class DerivedTT, numeric_arithmetic T, class... Rest>
    requires
        requires { sizeof(DerivedTT<T, Rest...>); } &&
        std::derived_from<DerivedTT<T, Rest...>, quantity<T>> &&
        (!std::same_as<DerivedTT<T, Rest...>, quantity<T>>)
struct unit_traits<DerivedTT<T, Rest...>>
{
    using value_type = T;

    template<class U>
    using base_rebind = quantity<U>;

    template<class U>
    using rebind = DerivedTT<U, Rest...>;
};

template<class Q>
concept quantity_class = unit_class_of<Q, quantity>;

template<class... Qs>
concept quantity_family = unit_family_of<quantity, Qs...>;

// -------------------------------------------------

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
        requires (!std::same_as<Self, Target>) && quantity_family<Self, Target>
    [[nodiscard]] constexpr
    explicit(!dominant_unit<Target, Self>)
    operator Target(this Self const& self) noexcept
    {
        return Target{static_cast<detail::value_type_t<Target>>(self.value)};
    }

    // ----------------------------------------------------

    template<class Self, class Other>
        requires quantity_family<Self, Other>
    [[nodiscard]] constexpr bool
    operator==(this Self const& self, Other const& other) noexcept
    {
        using common = common_value_type_t<Self, Other>;
        return static_cast<common>(self.value) == static_cast<common>(other.value);
    }

    template<class Self, class Other>
        requires quantity_family<Self, Other>
    [[nodiscard]] constexpr auto
    operator<=>(this Self const& self, Other const& other) noexcept
    {
        using common = common_value_type_t<Self, Other>;
        return static_cast<common>(self.value) <=> static_cast<common>(other.value);
    }

    // ----------------------------------------------------

    template<class Self>
    [[nodiscard]] constexpr Self
    operator+(this Self const& self) noexcept
    {
        return self;
    }

    template<class Self>
    [[nodiscard]] constexpr Self
    operator-(this Self const& self) noexcept
    {
        return Self{static_cast<detail::value_type_t<Self>>(-self.value)};
    }

    template<class Self>
        requires std::integral<detail::value_type_t<Self>>
    constexpr Self&
    operator++(this Self& self) noexcept
    {
        ++self.value;
        return self;
    }

    template<class Self>
        requires std::integral<detail::value_type_t<Self>>
    constexpr Self
    operator++(this Self& self, int) noexcept
    {
        return Self{self.value++};
    }

    template<class Self>
        requires std::integral<detail::value_type_t<Self>>
    constexpr Self&
    operator--(this Self& self) noexcept
    {
        --self.value;
        return self;
    }

    template<class Self>
        requires std::integral<detail::value_type_t<Self>>
    constexpr Self
    operator--(this Self& self, int) noexcept
    {
        return Self{self.value--};
    }

    // ----------------------------------------------------

    template<class Self, class Other>
        requires quantity_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator+(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<detail::value_type_t<result>>(self.value + other.value)};
    }

    template<class Self, class Other>
        requires dominant_unit<Self, Other>
    constexpr Self&
    operator+=(this Self& self, Other const& other) noexcept
    {
        self.value += other.value;
        return self;
    }

    template<class Self, class Other>
        requires quantity_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator-(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<detail::value_type_t<result>>(self.value - other.value)};
    }

    template<class Self, class Other>
        requires dominant_unit<Self, Other>
    constexpr Self&
    operator-=(this Self& self, Other const& other) noexcept
    {
        self.value -= other.value;
        return self;
    }

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
        requires dominant<detail::value_type_t<Self>, U>
    constexpr Self&
    operator*=(this Self& self, U scalar) noexcept
    {
        self.value *= scalar;
        return self;
    }

    template<class Self, numeric_arithmetic U>
        requires dominant<detail::value_type_t<Self>, U>
    constexpr Self&
    operator/=(this Self& self, U scalar) noexcept
    {
        self.value /= scalar;
        return self;
    }

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
    [[nodiscard]] constexpr auto
    operator*(this Self const& self, U scalar) noexcept
        -> detail::rebind_t<Self, std::common_type_t<detail::value_type_t<Self>, U>>
    {
        using common = std::common_type_t<detail::value_type_t<Self>, U>;
        return detail::rebind_t<Self, common>{static_cast<common>(self.value * scalar)};
    }

    // Scalar on the left (cannot use explicit object parameter)
    template<numeric_arithmetic U, quantity_class Q>
        requires std::derived_from<Q, quantity> // depend on self type to avoid ODR
    [[nodiscard]] friend constexpr auto
    operator*(U scalar, Q const& q) noexcept
        -> detail::rebind_t<Q, std::common_type_t<detail::value_type_t<Q>, U>>
    {
        using common = std::common_type_t<detail::value_type_t<Q>, U>;
        return detail::rebind_t<Q, common>{static_cast<common>(scalar * q.value)};
    }

    template<class Self, quantity_class Other>
    void operator*(this Self const&, Other const&) = delete;

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
    [[nodiscard]] constexpr auto
    operator/(this Self const& self, U scalar) noexcept
        -> detail::rebind_t<Self, std::common_type_t<detail::value_type_t<Self>, U>>
    {
        using common = std::common_type_t<detail::value_type_t<Self>, U>;
        return detail::rebind_t<Self, common>{static_cast<common>(self.value / scalar)};
    }

    template<class Self, class Other>
        requires quantity_family<Self, Other>
    [[nodiscard]] constexpr common_value_type_t<Self, Other>
    operator/(this Self const& self, Other const& other) noexcept
    {
        return static_cast<common_value_type_t<Self, Other>>(self.value / other.value);
    }

    template<numeric_arithmetic U, quantity_class Q>
        requires std::derived_from<Q, quantity> // depend on self type to avoid ODR
    friend void operator/(U, Q const&) = delete;

    // ----------------------------------------------------

    template<class Self, numeric_arithmetic U>
        requires
            std::integral<detail::value_type_t<Self>> &&
            std::integral<U> &&
            dominant<detail::value_type_t<Self>, U>
    constexpr Self&
    operator%=(this Self& self, U scalar) noexcept
    {
        self.value %= scalar;
        return self;
    }

    template<class Self, class Other>
        requires
            std::integral<detail::value_type_t<Self>> &&
            std::integral<detail::value_type_t<Other>> &&
            dominant_unit<Self, Other>
    constexpr Self&
    operator%=(this Self& self, Other const& other) noexcept
    {
        self.value %= other.value;
        return self;
    }

    template<class Self, numeric_arithmetic U>
        requires std::integral<detail::value_type_t<Self>> && std::integral<U>
    [[nodiscard]] constexpr auto
    operator%(this Self const& self, U scalar) noexcept
        -> detail::rebind_t<Self, std::common_type_t<detail::value_type_t<Self>, U>>
    {
        using common = std::common_type_t<detail::value_type_t<Self>, U>;
        return detail::rebind_t<Self, common>{static_cast<common>(self.value % scalar)};
    }

    template<class Self, class Other>
        requires
            std::integral<detail::value_type_t<Self>> &&
            std::integral<detail::value_type_t<Other>> &&
            quantity_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator%(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        return result{static_cast<detail::value_type_t<result>>(self.value % other.value)};
    }

    template<numeric_arithmetic U, quantity_class Q>
        requires std::derived_from<Q, quantity> // depend on self type to avoid ODR
    friend void operator%(U, Q const&) = delete;
};

} // iris::units

template<template<class...> class DerivedTT, iris::numeric_arithmetic T, class... Rest>
    requires iris::units::quantity_class<DerivedTT<T, Rest...>>
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

using iris::isnan;

template<quantity_class Q>
[[nodiscard]] constexpr bool isnan(Q const& q) noexcept
{
    return iris::isnan(q.value);
}

// --------------------------------------------------

template<quantity_class Q>
    requires std::floating_point<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q trunc(Q const& q) noexcept
{
    return Q{std::trunc(q.value)};
}

template<quantity_class Q>
    requires std::integral<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q trunc(Q const& q) noexcept
{
    return q;
}

template<quantity_class Q>
    requires std::floating_point<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q floor(Q const& q) noexcept
{
    return Q{std::floor(q.value)};
}

template<quantity_class Q>
    requires std::integral<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q floor(Q const& q) noexcept
{
    return q;
}

template<quantity_class Q>
    requires std::floating_point<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q ceil(Q const& q) noexcept
{
    return Q{std::ceil(q.value)};
}

template<quantity_class Q>
    requires std::integral<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q ceil(Q const& q) noexcept
{
    return q;
}

template<quantity_class Q>
    requires std::floating_point<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q round(Q const& q) noexcept
{
    return Q{std::round(q.value)};
}

template<quantity_class Q>
    requires std::integral<detail::value_type_t<Q>>
[[nodiscard]] constexpr Q round(Q const& q) noexcept
{
    return q;
}

template<quantity_class Q>
[[nodiscard]] constexpr Q abs(Q const& q) noexcept
{
    if constexpr (std::unsigned_integral<detail::value_type_t<Q>>) {
        return q;
    } else {
        return Q{static_cast<detail::value_type_t<Q>>(std::abs(q.value))};
    }
}

// --------------------------------------------------

// Returns by value, unlike `std::min`, since the result is in the common
// representation and may not be any of the arguments. `std::min` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_class A, quantity_class B>
    requires quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
(min)(A const& a, B const& b) noexcept
{
    using result = std::common_type_t<A, B>;
    return (std::min)(static_cast<result>(a), static_cast<result>(b));
}

// Returns by value, unlike `std::min`, since the result is in the common
// representation and may not be any of the arguments. `std::min` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_class A, quantity_class B, class Comp>
    requires
        quantity_family<A, B> &&
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
template<quantity_class A, quantity_class B>
    requires quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
(max)(A const& a, B const& b) noexcept
{
    using result = std::common_type_t<A, B>;
    return (std::max)(static_cast<result>(a), static_cast<result>(b));
}

// Returns by value, unlike `std::max`, since the result is in the common
// representation and may not be any of the arguments. `std::max` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_class A, quantity_class B, class Comp>
    requires
        quantity_family<A, B> &&
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
template<quantity_class Q, quantity_class Lo, quantity_class Hi>
    requires quantity_family<Q, Lo, Hi>
[[nodiscard]] constexpr common_unit_t<Q, Lo, Hi>
clamp(Q const& v, Lo const& lo, Hi const& hi) noexcept
{
    using result = common_unit_t<Q, Lo, Hi>;
    return std::clamp(static_cast<result>(v), static_cast<result>(lo), static_cast<result>(hi));
}

// Returns by value, unlike `std::clamp`, since the result is in the common
// representation and may not be any of the arguments. `std::clamp` remains
// available when a reference is wanted, but only for a single representation.
template<quantity_class Q, quantity_class Lo, quantity_class Hi, class Comp>
    requires
        quantity_family<Q, Lo, Hi> &&
        std::strict_weak_order<Comp&, common_unit_t<Q, Lo, Hi> const&, common_unit_t<Q, Lo, Hi> const&>
[[nodiscard]] constexpr common_unit_t<Q, Lo, Hi>
clamp(Q const& v, Lo const& lo, Hi const& hi, Comp&& comp)
    noexcept(std::is_nothrow_invocable_v<Comp&, common_unit_t<Q, Lo, Hi> const&, common_unit_t<Q, Lo, Hi> const&>)
{
    using result = common_unit_t<Q, Lo, Hi>;
    return std::clamp(static_cast<result>(v), static_cast<result>(lo), static_cast<result>(hi), std::forward<Comp>(comp));
}

// ---------------------------------------------------

template<quantity_class A, quantity_class B>
    requires quantity_family<A, B>
[[nodiscard]] constexpr std::common_type_t<A, B>
midpoint(A a, B b) noexcept
{
    using result = std::common_type_t<A, B>;
    using common = detail::value_type_t<result>;
    return result{std::midpoint(static_cast<common>(a.value), static_cast<common>(b.value))};
}

template<quantity_class A, quantity_class B, numeric_arithmetic T>
    requires
        quantity_family<A, B> &&
        std::floating_point<detail::value_type_t<std::common_type_t<A, B>>>
[[nodiscard]] constexpr std::common_type_t<A, B>
lerp(A a, B b, T t) noexcept
{
    using result = std::common_type_t<A, B>;
    using common = detail::value_type_t<result>;
    return result{std::lerp(static_cast<common>(a.value), static_cast<common>(b.value), static_cast<common>(t))};
}

} // iris::units

template<iris::units::quantity_class Q>
struct std::hash<Q>
{
    [[nodiscard]] static std::size_t operator()(Q const& q) noexcept
    {
        return std::hash<iris::units::detail::value_type_t<Q>>{}(q.value);
    }
};

template<iris::units::quantity_class Q, class CharT>
struct std::formatter<Q, CharT> : std::formatter<iris::units::detail::value_type_t<Q>, CharT>
{
    auto format(Q const& q, auto& ctx) const
    {
        return std::formatter<iris::units::detail::value_type_t<Q>, CharT>::format(q.value, ctx);
    }
};

#endif

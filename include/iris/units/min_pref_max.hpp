#ifndef IRIS_ZZ_UNITS_MIN_PREF_MAX_HPP
#define IRIS_ZZ_UNITS_MIN_PREF_MAX_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/units/concepts.hpp>
#include <iris/units/traits.hpp>
#include <iris/error/throwf_format.hpp>

#include <iris/hash.hpp>
#include <iris/math.hpp>

#include <algorithm>
#include <format>
#include <functional>
#include <concepts>
#include <type_traits>
#include <utility>
#include <compare> // IWYU pragma: keep
#include <stdexcept>

#include <cassert>
#include <cmath> // IWYU pragma: keep

namespace iris::units {

template<class T>
class min_pref_max;

template<template<class...> class DerivedTT, ordered_linear T, class... Rest>
    requires
        std::derived_from<DerivedTT<T, Rest...>, min_pref_max<T>> &&
        (!std::same_as<DerivedTT<T, Rest...>, min_pref_max<T>>)
struct unit_traits<DerivedTT<T, Rest...>>
{
    using value_type = T;

    template<class U>
    using base_rebind = min_pref_max<U>;

    template<class U>
    using rebind = DerivedTT<U, Rest...>;
};

template<class MPM>
concept min_pref_max_class = unit_class_of<MPM, min_pref_max>;

template<class... MPMs>
concept min_pref_max_family = unit_family_of<min_pref_max, MPMs...>;

// Declares the deduction guide for a min_pref_max family, so that `DerivedClass{min, pref, max}`
// can be deduced.
// Not required on compilers that implement <http://wg21.link/P2582>.
#define IRIS_MIN_PREF_MAX_DEDUCTION_GUIDE(class_name, ...) \
    template<class MinT, class PrefT, class MaxT> \
        requires ::iris::units::ordered_linear<::iris::symmetric_common_type_t<MinT, PrefT, MaxT>> \
    class_name(MinT, PrefT, MaxT) -> class_name<::iris::symmetric_common_type_t<MinT, PrefT, MaxT> __VA_OPT__(,) __VA_ARGS__>

// `min_pref_max<T>` holds three components `T min`, `T pref`, and `T max`, with the
// invariant `min <= pref <= max`. It serves as a base type for settings in typesetting
// applications: for instance, a "margin" setting is naturally expressed as the set of
// "min margin", "preferred margin", and "max margin".
//
// When setting a new value to a component, other components are automatically clamped
// to satisfy the invariant `min <= pref <= max`.
//
// Applying `final` to the derived class is strongly advised.
//
// For practical ergonomics, the derived class should provide the following:
//
// 1. `using iris::units::min_pref_max<T>::min_pref_max;`
//    Required. Without it, the class has no usable constructor and the operators
//    fail to instantiate.
//
// 2. `IRIS_MIN_PREF_MAX_DEDUCTION_GUIDE(class_name);`
//    Enables construction with the syntax `DerivedClass{min, pref, max}`.
//    Not required on compilers that implement <http://wg21.link/P2582>.
template<class T>
class min_pref_max
{
    static_assert(ordered_linear<T>);
    static_assert(
        nothrow_ordered_linear<T>,
        "In order to provide strong exception guarantee, `min_pref_max<T>` naturally requires "
        "the entire operation to be atomic (i.e., each component-wise operation must not throw)."
    );

    template<class>
    friend class min_pref_max;

public:
    using value_type = T;

    [[nodiscard]] constexpr T const& min() const noexcept { return min_; }
    [[nodiscard]] constexpr T const& pref() const noexcept { return pref_; }
    [[nodiscard]] constexpr T const& max() const noexcept { return max_; }

    [[nodiscard]] constexpr T shrink() const noexcept { return pref_ - min_; }
    [[nodiscard]] constexpr T stretch() const noexcept { return max_ - pref_; }

    [[nodiscard]] constexpr bool is_all_zero() const noexcept { return min_ == T{} && pref_ == T{} && max_ == T{}; }

    constexpr void set_min(T v) noexcept
    {
    #if __cpp_lib_constexpr_cmath >= 202202L
        assert(!isnan(v));
    #else
        assert(v == v);
    #endif
        min_ = v;
        pref_ = std::max(pref_, v);
        max_ = std::max(max_, v);
        this->check_invariants();
    }
    constexpr void set_pref(T v) noexcept
    {
    #if __cpp_lib_constexpr_cmath >= 202202L
        assert(!isnan(v));
    #else
        assert(v == v);
    #endif
        min_ = std::min(min_, v);
        pref_ = v;
        max_ = std::max(max_, v);
        this->check_invariants();
    }
    constexpr void set_max(T v) noexcept
    {
    #if __cpp_lib_constexpr_cmath >= 202202L
        assert(!isnan(v));
    #else
        assert(v == v);
    #endif
        min_ = std::min(min_, v);
        pref_ = std::min(pref_, v);
        max_ = v;
        this->check_invariants();
    }

    // ---------------------------------------------------

    template<class Self, class Other>
        requires min_pref_max_family<Self, Other>
    [[nodiscard]] constexpr bool
    operator==(this Self const& self, Other const& other) noexcept
    {
        using common = common_value_type_t<Self, Other>;
        return static_cast<common>(self.min_) == static_cast<common>(other.min_) &&
            static_cast<common>(self.pref_) == static_cast<common>(other.pref_) &&
            static_cast<common>(self.max_) == static_cast<common>(other.max_);
    }

    // ---------------------------------------------------

    constexpr min_pref_max() = default;

protected:
    // Using `min_pref_max<T>` without deriving is prohibited as a "min_pref_max" without
    // target domain is meaningless
    constexpr ~min_pref_max() noexcept = default;

public:
    // We need to resurrect defaulted special members since we declare destructor
    constexpr min_pref_max(min_pref_max const&) noexcept = default;
    constexpr min_pref_max(min_pref_max&&) noexcept = default;
    constexpr min_pref_max& operator=(min_pref_max const&) noexcept = default;
    constexpr min_pref_max& operator=(min_pref_max&&) noexcept = default;

    template<class MinT, class PrefT, class MaxT>
        requires
            std::constructible_from<T, MinT> &&
            std::constructible_from<T, PrefT> &&
            std::constructible_from<T, MaxT>
    constexpr explicit(!(
        std::convertible_to<MinT, T> &&
        std::convertible_to<PrefT, T> &&
        std::convertible_to<MaxT, T>
    )) min_pref_max(MinT min, PrefT pref, MaxT max)
        : min_(std::move(min))
        , pref_(std::move(pref))
        , max_(std::move(max))
    {
        if (!(min_ <= pref_ && pref_ <= max_)) {
            throwf<std::invalid_argument>("min <= pref <= max: got {}/{}/{}", min_, pref_, max_);
        }
    }

private:
    constexpr explicit min_pref_max(detail::invariants_always_satisfied_t, T min, T pref, T max) noexcept
        : min_(std::move(min))
        , pref_(std::move(pref))
        , max_(std::move(max))
    {
        check_invariants(); // assert
    }

public:
    template<class Self, class Target>
        requires
            (!std::same_as<Self, Target>) &&
            min_pref_max_family<Self, Target>
    [[nodiscard]] constexpr explicit(!dominant_unit<Target, Self>)
    operator Target(this Self const& self) noexcept
    {
        using target_value_type = detail::value_type_t<Target>;
        return Target(
            detail::invariants_always_satisfied,
            static_cast<target_value_type>(self.min_),
            static_cast<target_value_type>(self.pref_),
            static_cast<target_value_type>(self.max_)
        );
    }

    // ---------------------------------------------------

    template<class Self, class Other>
        requires min_pref_max_family<Self, Other>
    [[nodiscard]] constexpr std::common_type_t<Self, Other>
    operator+(this Self const& self, Other const& other) noexcept
    {
        using result = std::common_type_t<Self, Other>;
        using common = detail::value_type_t<result>;
        return result(
            detail::invariants_always_satisfied,
            static_cast<common>(self.min_ + other.min_),
            static_cast<common>(self.pref_ + other.pref_),
            static_cast<common>(self.max_ + other.max_)
        );
    }

    template<class Self, class Other>
        requires dominant_unit<Self, Other>
    constexpr Self&
    operator+=(this Self& self, Other const& other) noexcept
    {
        self.min_ += other.min_;
        self.pref_ += other.pref_;
        self.max_ += other.max_;
        self.check_invariants();
        return self;
    }

    // ----------------------------------------------------

    template<class Self>
    [[nodiscard]] constexpr Self
    operator+(this Self const& self, T const& delta) noexcept
    {
        return Self(detail::invariants_always_satisfied, self.min_ + delta, self.pref_ + delta, self.max_ + delta);
    }

    template<class Self>
    [[nodiscard]] constexpr Self
    operator-(this Self const& self, T const& delta) noexcept
    {
        return Self(detail::invariants_always_satisfied, self.min_ - delta, self.pref_ - delta, self.max_ - delta);
    }

    template<class Self>
    constexpr Self&
    operator+=(this Self& self, T const& delta) noexcept
    {
        self.min_ += delta;
        self.pref_ += delta;
        self.max_ += delta;
        self.check_invariants();
        return self;
    }

    template<class Self>
    constexpr Self&
    operator-=(this Self& self, T const& delta) noexcept
    {
        self.min_ -= delta;
        self.pref_ -= delta;
        self.max_ -= delta;
        self.check_invariants();
        return self;
    }

    template<min_pref_max_class M>
        requires std::derived_from<M, min_pref_max> // required for avoiding ODR violation
    [[nodiscard]] friend constexpr M operator+(T const& delta, M const& m) noexcept
    {
        return m + delta;
    }

private:
    constexpr void check_invariants() const noexcept
    {
        assert(min_ <= pref_);
        assert(pref_ <= max_);
    }

    T min_{}, pref_{}, max_{};
};

} // iris::units

template<iris::units::min_pref_max_class M>
struct std::hash<M>
{
    [[nodiscard]] static std::size_t operator()(M const& m) noexcept
    {
        return iris::hash_all(m.min(), m.pref(), m.max());
    }
};

template<iris::units::min_pref_max_class M, class CharT>
struct std::formatter<M, CharT> : std::formatter<iris::units::detail::value_type_t<M>, CharT>
{
    using base = std::formatter<iris::units::detail::value_type_t<M>, CharT>;

    template<class FormatContext>
    auto format(M const& m, FormatContext& ctx) const
    {
        auto out = base::format(m.min(), ctx);
        *out++ = CharT('/');
        ctx.advance_to(out);
        out = base::format(m.pref(), ctx);
        *out++ = CharT('/');
        ctx.advance_to(out);
        return base::format(m.max(), ctx);
    }
};

#endif

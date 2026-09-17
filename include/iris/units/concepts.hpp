#ifndef IRIS_ZZ_UNITS_CONCEPTS_HPP
#define IRIS_ZZ_UNITS_CONCEPTS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/type_traits.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace iris::units {

template<class T>
using scalar_t = std::remove_cvref_t<decltype(std::declval<T const&>() / std::declval<T const&>())>;

template<class T>
concept scalar = iris::numeric_arithmetic<T>;

// `T` models `ordered_linear` if `T` is a totally ordered one-dimensional
// vector space over `scalar_t<T>`.
//
// For example, a class derived from `iris::units::quantity<Rep>` models
// `ordered_linear` when `Rep` is a floating-point type. Integral `Rep`
// does not, because integer division is not the inverse of multiplication.
//
// Preconditions:
//   Values of `T` (and of `scalar_t<T>`) passed to algorithms constrained by
//   this concept satisfy the following conditions:
//   - They are not NaN.
//   - They are finite. In particular, infinity must not be used as a sentinel
//     for an unbounded or invalid value.
//   - Arithmetic among them does not overflow to infinity.
//
// Semantic requirements are as follows:
//   - Let `a`, `b`, and `c` denote instances of `T`.
//   - Let `s` denote an instance of `scalar_t<T>`.
//   - `T{}` is the additive identity: `a + T{} == a`.
//   - `a <= b` implies `a + c <= b + c`.
//   - `T{} <= a` and `0 <= s` imply `T{} <= a * s`.
//   - For `b != T{}`, `(a / b) * b` approximates `a` within floating-point rounding.
template<class T>
concept ordered_linear =
    std::regular<T> &&
    std::totally_ordered<T> &&
    requires {
        typename scalar_t<T>;
    } &&
    requires(T const& t, scalar_t<T> const& s) {
        { t + t } -> std::same_as<T>;
        { t - t } -> std::same_as<T>;
        { t * s } -> std::same_as<T>;
        { t / s } -> std::same_as<T>;
        { t / t } -> scalar;
    } &&
    std::floating_point<scalar_t<T>> /* scalars must form a field */;

template<class T>
concept nothrow_ordered_linear =
    ordered_linear<T> &&
    std::is_nothrow_default_constructible_v<T> &&
    std::is_nothrow_copy_constructible_v<T> &&
    std::is_nothrow_copy_assignable_v<T> &&
    requires(T const& t, scalar_t<T> const& s) {
        { t + t } noexcept;
        { t - t } noexcept;
        { t * s } noexcept;
        { t / s } noexcept;
        { t / t } noexcept;
        { t <= t } noexcept;
    };

} // iris::units

#endif

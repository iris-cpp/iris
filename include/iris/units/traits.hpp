#ifndef IRIS_ZZ_UNITS_TRAITS_HPP
#define IRIS_ZZ_UNITS_TRAITS_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

// Don't include <iris/units/concepts.hpp>; dependency ordering is important

#include <iris/type_traits.hpp>

#include <type_traits>
#include <concepts>

namespace iris::units {

template<class First, class... Rest>
concept compatible_value_type =
    requires {
        typename symmetric_common_type_t<First, Rest...>;
    };

template<class Derived>
struct unit_traits
{};

namespace detail {

template<class Derived>
using value_type_t = unit_traits<std::remove_cvref_t<Derived>>::value_type;

template<class Derived, class U>
using base_rebind_t = unit_traits<std::remove_cvref_t<Derived>>::template base_rebind<U>;

template<class Derived, class U>
using rebind_t = unit_traits<std::remove_cvref_t<Derived>>::template rebind<U>;

template<class Derived>
concept unit_class_impl =
    requires {
        typename value_type_t<Derived>;
        typename base_rebind_t<Derived, value_type_t<Derived>>;
        typename rebind_t<Derived, value_type_t<Derived>>;
    } &&
    std::same_as<
        Derived,
        rebind_t<Derived, value_type_t<Derived>>
    > &&
    std::derived_from<
        Derived,
        base_rebind_t<Derived, value_type_t<Derived>>
    > &&
    // Exclude the case when `Derived` is not actually a derived class, i.e. it is the base class
    !std::same_as<
        Derived,
        base_rebind_t<Derived, value_type_t<Derived>>
    >;

template<class Derived, template<class...> class UnitTT>
concept unit_class_of_impl =
    unit_class_impl<Derived> &&
    std::same_as<
        base_rebind_t<Derived, value_type_t<Derived>>,
        UnitTT<value_type_t<Derived>>
    >;

template<class DerivedA, class DerivedB>
concept same_unit_class_impl =
    unit_class_impl<DerivedA> &&
    unit_class_impl<DerivedB> &&
    std::same_as<
        rebind_t<DerivedA, value_type_t<DerivedB>>,
        DerivedB
    > &&
    std::same_as<
        rebind_t<DerivedB, value_type_t<DerivedA>>,
        DerivedA
    >;

} // detail

// `Derived = DerivedTT<T, ...>` is a unit class, i.e., a class
// derived from some arbitrary base unit class `BaseTT<T>`.
template<class Derived>
concept unit_class = detail::unit_class_impl<std::remove_cvref_t<Derived>>;

// `Derived = DerivedTT<T, ...>` is a unit class derived from `UnitTT<T>`.
template<class Derived, template<class...> class UnitTT>
concept unit_class_of = detail::unit_class_of_impl<std::remove_cvref_t<Derived>, UnitTT>;

// `DerivedA` and `DerivedB` are specializations of the same unit class template
// (`DerivedTT<T, ...>` and `DerivedTT<U, ...>`), and their value types `T` and `U`
// are compatible.
template<class DerivedA, class DerivedB>
concept unit_family_with =
    detail::same_unit_class_impl<std::remove_cvref_t<DerivedA>, std::remove_cvref_t<DerivedB>> &&
    compatible_value_type<detail::value_type_t<DerivedA>, detail::value_type_t<DerivedB>>;

// All of `First, Rest...` are specializations of the same unit class template, and
// their value types are pairwise compatible with that of `First`.
template<class First, class... Rest>
concept unit_family =
    unit_class<First> &&
    (unit_family_with<First, Rest> && ...);

// `unit_family<First, Rest...>` whose base unit class is `UnitTT`.
template<template<class...> class UnitTT, class First, class... Rest>
concept unit_family_of =
    unit_family<First, Rest...> &&
    unit_class_of<First, UnitTT>;

} // iris::units

// Specializations of the same unit class template share a common type whose value
// type is the common type of theirs
template<
    template<class...> class DerivedTT,
    class T, class U, class... Rest
>
    requires iris::units::unit_family_with<DerivedTT<T, Rest...>, DerivedTT<U, Rest...>>
struct std::common_type<DerivedTT<T, Rest...>, DerivedTT<U, Rest...>>
{
    using type = DerivedTT<std::common_type_t<T, U>, Rest...>;
};

// A base unit and a derived unit have no common type
template<template<class...> class DerivedTT, class T, class... Rest>
    requires iris::units::unit_class<DerivedTT<T, Rest...>>
struct std::common_type<
    DerivedTT<T, Rest...>,
    iris::units::detail::base_rebind_t<
        DerivedTT<T, Rest...>,
        iris::units::detail::value_type_t<DerivedTT<T, Rest...>>
    >
>
{
    // No `::type`
};

// A base unit and a derived unit have no common type
template<template<class...> class DerivedTT, class T, class... Rest>
    requires iris::units::unit_class<DerivedTT<T, Rest...>>
struct std::common_type<
    iris::units::detail::base_rebind_t<
        DerivedTT<T, Rest...>,
        iris::units::detail::value_type_t<DerivedTT<T, Rest...>>
    >,
    DerivedTT<T, Rest...>
>
{
    // No `::type`
};

// -------------------------------------------------------------------------

namespace iris::units {

// The value type shared by the units in `Units...`, i.e., `symmetric_common_type_t`
// of their value types.
template<class... Units>
    requires
        unit_family<Units...> &&
        compatible_value_type<detail::value_type_t<Units>...>
using common_value_type_t = symmetric_common_type_t<detail::value_type_t<Units>...>;

// The unit type of `First` rebound to the common value type of `First, Rest...`.
template<class First, class... Rest>
    requires unit_family<First, Rest...>
using common_unit_t = detail::rebind_t<First, common_value_type_t<First, Rest...>>;

// `U` is the dominant unit among `U, Us...`, i.e., all are of the same unit family and
// the value type of `U` dominates those of `Us...` (in the sense of `dominant`).
// Converting from any of `Us...` to `U` is what the usual arithmetic conversions do,
// so such a conversion is implicit while the reverse is explicit.
template<class U, class... Us>
concept dominant_unit =
    unit_family<U, Us...> &&
    dominant<detail::value_type_t<U>, detail::value_type_t<Us>...>;

} // iris::units

// -------------------------------------------------------------------------

namespace iris::units::detail {

// Tag for constructors whose invariants are guaranteed by the caller; the runtime
// check is replaced by `assert`.
struct invariants_always_satisfied_t { constexpr explicit invariants_always_satisfied_t() = default; };
inline constexpr invariants_always_satisfied_t invariants_always_satisfied{};

} // iris::units::detail

#endif

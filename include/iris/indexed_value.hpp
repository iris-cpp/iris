#ifndef IRIS_ZZ_INDEXED_VALUE_HPP
#define IRIS_ZZ_INDEXED_VALUE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/requirements.hpp>
#include <iris/compare.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

#include <cstddef>

namespace iris {

template<class IndexT, class T>
struct indexed_value
{
    using index_type = IndexT;
    using value_type = T;

    IndexT index;
    T value;

    template<class IndexU, class U>
        requires
            std::convertible_to<IndexT&, IndexU> &&
            std::convertible_to<T&, U>
    constexpr operator indexed_value<IndexU, U>() &
        noexcept(
            noexcept(static_cast<IndexU>(std::declval<IndexT&>())) &&
            noexcept(static_cast<U>(std::declval<T&>()))
        )
    {
        return {
            static_cast<IndexU>(index),
            static_cast<U>(value)
        };
    }

    template<class IndexU, class U>
        requires
            std::convertible_to<IndexT const&, IndexU> &&
            std::convertible_to<T const&, U>
    constexpr operator indexed_value<IndexU, U>() const&
        noexcept(
            noexcept(static_cast<IndexU>(std::declval<IndexT const&>())) &&
            noexcept(static_cast<U>(std::declval<T const&>()))
        )
    {
        return {
            static_cast<IndexU>(index),
            static_cast<U>(value)
        };
    }

    template<class IndexU, class U>
        requires
            std::convertible_to<IndexT&&, IndexU> &&
            std::convertible_to<T&&, U>
    constexpr operator indexed_value<IndexU, U>() &&
        noexcept(
            noexcept(static_cast<IndexU>(std::declval<IndexT&&>())) &&
            noexcept(static_cast<U>(std::declval<T&&>()))
        )
    {
        return {
            static_cast<IndexU>(static_cast<IndexT&&>(index)),
            static_cast<U>(static_cast<T&&>(value))
        };
    }

    template<class IndexU, class U>
        requires
            std::convertible_to<IndexT const&&, IndexU> &&
            std::convertible_to<T const&&, U>
    constexpr operator indexed_value<IndexU, U>() const&&
        noexcept(
            noexcept(static_cast<IndexU>(std::declval<IndexT const&&>())) &&
            noexcept(static_cast<U>(std::declval<T const&&>()))
        )
    {
        return {
            static_cast<IndexU>(static_cast<IndexT const&&>(index)),
            static_cast<U>(static_cast<T const&&>(value))
        };
    }
};

template<class IndexT, class T, class IndexU, class U>
    requires req::half_equality_comparable<IndexT, IndexU> && req::half_equality_comparable<T, U>
[[nodiscard]] constexpr bool operator==(indexed_value<IndexT, T> const& a, indexed_value<IndexU, U> const& b)
    noexcept(noexcept(a.index == b.index) && noexcept(a.value == b.value))
{
    return a.index == b.index && a.value == b.value;
}

template<class IndexT, class T, class IndexU, class U>
    requires requires (IndexT const& i, IndexU const& j, T const& t, U const& u) {
        cmp::synth_three_way{}(i, j);
        cmp::synth_three_way{}(t, u);
    }
[[nodiscard]] constexpr auto operator<=>(indexed_value<IndexT, T> const& a, indexed_value<IndexU, U> const& b)
    noexcept(
        noexcept(cmp::synth_three_way{}(a.index, b.index)) &&
        noexcept(cmp::synth_three_way{}(a.value, b.value))
    )
    -> std::common_comparison_category_t<
        cmp::synth_three_way_result<IndexT, IndexU>,
        cmp::synth_three_way_result<T, U>
    >
{
    if (auto const comp = cmp::synth_three_way{}(a.index, b.index); comp != 0) {
        return comp;
    }
    return cmp::synth_three_way{}(a.value, b.value);
}

template<std::size_t I, class IndexT, class T>
[[nodiscard]] constexpr decltype(auto) get(indexed_value<IndexT, T>& elem) noexcept
{
    static_assert(I < 2);
    if constexpr (I == 0)      { return (elem.index); }
    else if constexpr (I == 1) { return (elem.value); }
}

template<std::size_t I, class IndexT, class T>
[[nodiscard]] constexpr decltype(auto) get(indexed_value<IndexT, T> const& elem) noexcept
{
    static_assert(I < 2);
    if constexpr (I == 0)      { return (elem.index); }
    else if constexpr (I == 1) { return (elem.value); }
}

template<std::size_t I, class IndexT, class T>
[[nodiscard]] constexpr decltype(auto) get(indexed_value<IndexT, T>&& elem) noexcept
{
    static_assert(I < 2);
    if constexpr (I == 0)      { return static_cast<IndexT&&>(elem.index); }
    else if constexpr (I == 1) { return static_cast<T&&>(elem.value); }
}

template<std::size_t I, class IndexT, class T>
[[nodiscard]] constexpr decltype(auto) get(indexed_value<IndexT, T> const&& elem) noexcept
{
    static_assert(I < 2);
    if constexpr (I == 0)      { return static_cast<IndexT const&&>(elem.index); }
    else if constexpr (I == 1) { return static_cast<T const&&>(elem.value); }
}

} // iris

template<class IndexT, class T, class IndexU, class U>
    requires requires {
        typename std::common_type_t<IndexT, IndexU>;
        typename std::common_type_t<T, U>;
    }
struct std::common_type<
    iris::indexed_value<IndexT, T>,
    iris::indexed_value<IndexU, U>
>
{
    using type = iris::indexed_value<
        std::common_type_t<IndexT, IndexU>,
        std::common_type_t<T, U>
    >;
};

template<
    class IndexT, class T,
    class IndexU, class U,
    template<class> class TQual,
    template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<IndexT>, UQual<IndexU>>;
        typename std::common_reference_t<TQual<T>, UQual<U>>;
    }
struct std::basic_common_reference<
    iris::indexed_value<IndexT, T>,
    iris::indexed_value<IndexU, U>,
    TQual, UQual
>
{
    using type = iris::indexed_value<
        std::common_reference_t<TQual<IndexT>, UQual<IndexU>>,
        std::common_reference_t<TQual<T>, UQual<U>>
    >;
};

template<class IndexT, class T>
struct std::tuple_size<iris::indexed_value<IndexT, T>>
    : std::integral_constant<std::size_t, 2>
{};

template<class IndexT, class T>
struct std::tuple_element<0, iris::indexed_value<IndexT, T>>
{
    using type = IndexT;
};
template<class IndexT, class T>
struct std::tuple_element<1, iris::indexed_value<IndexT, T>>
{
    using type = T;
};

#endif

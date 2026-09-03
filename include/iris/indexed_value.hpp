#ifndef IRIS_ZZ_INDEXED_VALUE_HPP
#define IRIS_ZZ_INDEXED_VALUE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep
#include <iris/requirements.hpp>
#include <iris/compare.hpp>

#include <iris/bits/specialization_of.hpp>

#include <concepts>
#include <type_traits>
#include <utility>
#include <tuple>

#include <cstddef>

namespace iris {

template<class IndexT, class T>
struct indexed_value
{
    using index_type = IndexT;
    using value_type = T;

    IndexT index;
    T value;

    // ----------------------------------------------------

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

    // ----------------------------------------------------

    template<class U, class V>
        requires
            std::convertible_to<IndexT&, U> &&
            std::convertible_to<T&, V>
    constexpr operator std::tuple<U, V>() &
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT&>())) &&
            noexcept(static_cast<V>(std::declval<T&>()))
        )
    {
        return std::tuple<U, V>(
            static_cast<U>(index),
            static_cast<V>(value)
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT const&, U> &&
            std::convertible_to<T const&, V>
    constexpr operator std::tuple<U, V>() const&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT const&>())) &&
            noexcept(static_cast<V>(std::declval<T const&>()))
        )
    {
        return std::tuple<U, V>(
            static_cast<U>(index),
            static_cast<V>(value)
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT&&, U> &&
            std::convertible_to<T&&, V>
    constexpr operator std::tuple<U, V>() &&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT&&>())) &&
            noexcept(static_cast<V>(std::declval<T&&>()))
        )
    {
        return std::tuple<U, V>(
            static_cast<U>(static_cast<IndexT&&>(index)),
            static_cast<V>(static_cast<T&&>(value))
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT const&&, U> &&
            std::convertible_to<T const&&, V>
    constexpr operator std::tuple<U, V>() const&&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT const&&>())) &&
            noexcept(static_cast<V>(std::declval<T const&&>()))
        )
    {
        return std::tuple<U, V>(
            static_cast<U>(static_cast<IndexT const&&>(index)),
            static_cast<V>(static_cast<T const&&>(value))
        );
    }

    // ----------------------------------------------------

    template<class U, class V>
        requires
            std::convertible_to<IndexT&, U> &&
            std::convertible_to<T&, V>
    constexpr operator std::pair<U, V>() &
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT&>())) &&
            noexcept(static_cast<V>(std::declval<T&>()))
        )
    {
        return std::pair<U, V>(
            static_cast<U>(index),
            static_cast<V>(value)
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT const&, U> &&
            std::convertible_to<T const&, V>
    constexpr operator std::pair<U, V>() const&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT const&>())) &&
            noexcept(static_cast<V>(std::declval<T const&>()))
        )
    {
        return std::pair<U, V>(
            static_cast<U>(index),
            static_cast<V>(value)
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT&&, U> &&
            std::convertible_to<T&&, V>
    constexpr operator std::pair<U, V>() &&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT&&>())) &&
            noexcept(static_cast<V>(std::declval<T&&>()))
        )
    {
        return std::pair<U, V>(
            static_cast<U>(static_cast<IndexT&&>(index)),
            static_cast<V>(static_cast<T&&>(value))
        );
    }

    template<class U, class V>
        requires
            std::convertible_to<IndexT const&&, U> &&
            std::convertible_to<T const&&, V>
    constexpr operator std::pair<U, V>() const&&
        noexcept(
            noexcept(static_cast<U>(std::declval<IndexT const&&>())) &&
            noexcept(static_cast<V>(std::declval<T const&&>()))
        )
    {
        return std::pair<U, V>(
            static_cast<U>(static_cast<IndexT const&&>(index)),
            static_cast<V>(static_cast<T const&&>(value))
        );
    }
};

template<class IndexT, class T, class IndexU, class U>
    requires req::half_equality_comparable<IndexT, IndexU> && req::half_equality_comparable<T, U>
[[nodiscard]] constexpr bool operator==(indexed_value<IndexT, T> const& a, indexed_value<IndexU, U> const& b)
    noexcept(noexcept(a.index == b.index) && noexcept(a.value == b.value))
{
    return a.index == b.index && a.value == b.value;
}

template<class IndexT, class T, class U>
    requires
        (!is_ttp_specialization_of_v<U, indexed_value>) &&
        requires {
            requires std::tuple_size<std::remove_cvref_t<U>>::value == 2;
        } &&
        req::half_equality_comparable<IndexT, std::tuple_element_t<0, std::remove_cvref_t<U>>> &&
        req::half_equality_comparable<T, std::tuple_element_t<1, std::remove_cvref_t<U>>>
[[nodiscard]] constexpr bool operator==(indexed_value<IndexT, T> const& a, U const& b)
{
    return a.index == std::get<0>(b) && a.value == std::get<1>(b);
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


// indexed_value + indexed_value
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

// indexed_value + indexed_value
template<
    class IndexT, class T, class IndexU, class U,
    template<class> class TQual, template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<IndexT>, UQual<IndexU>>;
        typename std::common_reference_t<TQual<T>, UQual<U>>;
    }
struct std::basic_common_reference<
    iris::indexed_value<IndexT, T>, iris::indexed_value<IndexU, U>, TQual, UQual
>
{
    using type = iris::indexed_value<
        std::common_reference_t<TQual<IndexT>, UQual<IndexU>>,
        std::common_reference_t<TQual<T>, UQual<U>>
    >;
};

// -----------------------------------------------------------

// indexed_value + std::tuple
template<class IndexT, class T, class Tup0, class Tup1>
    requires requires {
        typename std::common_type_t<IndexT, Tup0>;
        typename std::common_type_t<T, Tup1>;
    }
struct std::common_type<
    iris::indexed_value<IndexT, T>,
    std::tuple<Tup0, Tup1>
>
{
    using type = std::tuple<
        std::common_type_t<IndexT, Tup0>,
        std::common_type_t<T, Tup1>
    >;
};

// std::tuple + indexed_value
template<class Tup0, class Tup1, class IndexU, class U>
    requires requires {
        typename std::common_type_t<Tup0, IndexU>;
        typename std::common_type_t<Tup1, U>;
    }
struct std::common_type<
    std::tuple<Tup0, Tup1>,
    iris::indexed_value<IndexU, U>
>
{
    using type = std::tuple<
        std::common_type_t<Tup0, IndexU>,
        std::common_type_t<Tup1, U>
    >;
};


// indexed_value + std::tuple
template<
    class IndexT, class T, class Tup0, class Tup1,
    template<class> class TQual, template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<IndexT>, UQual<Tup0>>;
        typename std::common_reference_t<TQual<T>, UQual<Tup1>>;
    }
struct std::basic_common_reference<
    iris::indexed_value<IndexT, T>, std::tuple<Tup0, Tup1>, TQual, UQual
>
{
    using type = std::tuple<
        std::common_reference_t<TQual<IndexT>, UQual<Tup0>>,
        std::common_reference_t<TQual<T>, UQual<Tup1>>
    >;
};

// std::tuple + indexed_value
template<
    class Tup0, class Tup1, class IndexU, class U,
    template<class> class TQual, template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<Tup0>, UQual<IndexU>>;
        typename std::common_reference_t<TQual<Tup1>, UQual<U>>;
    }
struct std::basic_common_reference<
    std::tuple<Tup0, Tup1>, iris::indexed_value<IndexU, U>, TQual, UQual
>
{
    using type = std::tuple<
        std::common_reference_t<TQual<Tup0>, UQual<IndexU>>,
        std::common_reference_t<TQual<Tup1>, UQual<U>>
    >;
};

// -----------------------------------------------------------

// indexed_value + std::pair
template<class IndexT, class T, class Tup0, class Tup1>
    requires requires {
        typename std::common_type_t<IndexT, Tup0>;
        typename std::common_type_t<T, Tup1>;
    }
struct std::common_type<
    iris::indexed_value<IndexT, T>,
    std::pair<Tup0, Tup1>
>
{
    using type = std::pair<
        std::common_type_t<IndexT, Tup0>,
        std::common_type_t<T, Tup1>
    >;
};

// std::pair + indexed_value
template<class Tup0, class Tup1, class IndexU, class U>
    requires requires {
        typename std::common_type_t<Tup0, IndexU>;
        typename std::common_type_t<Tup1, U>;
    }
struct std::common_type<
    std::pair<Tup0, Tup1>,
    iris::indexed_value<IndexU, U>
>
{
    using type = std::pair<
        std::common_type_t<Tup0, IndexU>,
        std::common_type_t<Tup1, U>
    >;
};


// indexed_value + std::pair
template<
    class IndexT, class T, class Tup0, class Tup1,
    template<class> class TQual, template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<IndexT>, UQual<Tup0>>;
        typename std::common_reference_t<TQual<T>, UQual<Tup1>>;
    }
struct std::basic_common_reference<
    iris::indexed_value<IndexT, T>, std::pair<Tup0, Tup1>, TQual, UQual
>
{
    using type = std::pair<
        std::common_reference_t<TQual<IndexT>, UQual<Tup0>>,
        std::common_reference_t<TQual<T>, UQual<Tup1>>
    >;
};

// std::pair + indexed_value
template<
    class Tup0, class Tup1, class IndexU, class U,
    template<class> class TQual, template<class> class UQual
>
    requires requires {
        typename std::common_reference_t<TQual<Tup0>, UQual<IndexU>>;
        typename std::common_reference_t<TQual<Tup1>, UQual<U>>;
    }
struct std::basic_common_reference<
    std::pair<Tup0, Tup1>, iris::indexed_value<IndexU, U>, TQual, UQual
>
{
    using type = std::pair<
        std::common_reference_t<TQual<Tup0>, UQual<IndexU>>,
        std::common_reference_t<TQual<Tup1>, UQual<U>>
    >;
};

#endif

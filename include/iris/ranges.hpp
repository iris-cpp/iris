#ifndef IRIS_ZZ_RANGES_HPP
#define IRIS_ZZ_RANGES_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/utility.hpp>

#include <ranges> // IWYU pragma: export
#include <concepts>
#include <type_traits>
#include <utility>

namespace iris::ranges {

namespace detail {

template<std::ranges::range R>
using kv_element_t = std::remove_cvref_t<std::ranges::range_reference_t<R>>;

} // detail

// `R` is a range with value type of 2-element tuple.
// This accepts also array of pair. If stricter check is needed, use `mapping_range<R>`.
// Note: To convert `key_value_range` to `mapping_range`, use `iris::ranges::as_map`.
template<class R>
concept key_value_range =
    std::ranges::input_range<R> &&
    requires {
        typename detail::kv_element_t<R>;
        requires std::tuple_size<detail::kv_element_t<R>>::value == 2;
        typename std::tuple_element<0, detail::kv_element_t<R>>::type;
        typename std::tuple_element<1, detail::kv_element_t<R>>::type;
    } &&
    gettable<0, std::ranges::range_reference_t<R>> &&
    gettable<1, std::ranges::range_reference_t<R>>;

template<class K, class V>
struct dummy_key_value_range
{
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;
    ~dummy_key_value_range() = delete;
};

template<key_value_range R>
using range_key_t    = std::remove_cvref_t<std::tuple_element_t<0, detail::kv_element_t<R>>>;

template<key_value_range R>
using range_mapped_t = std::remove_cvref_t<std::tuple_element_t<1, detail::kv_element_t<R>>>;


// `R` is a range with value type of 2-element tuple AND it has certain
// strictly map-specific traits such as `::key_type` and `::mapped_type`.
// To include more loose type like an array of pair, use `key_value_range<R>`.
template<class R>
concept mapping_range =
    key_value_range<R> &&
    requires {
        typename std::remove_cvref_t<R>::key_type;
        typename std::remove_cvref_t<R>::mapped_type;
    } &&
    std::same_as<range_key_t<R>,    typename std::remove_cvref_t<R>::key_type> &&
    std::same_as<range_mapped_t<R>, typename std::remove_cvref_t<R>::mapped_type>;

template<class K, class V>
struct dummy_mapping_range
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;
    ~dummy_mapping_range() = delete;
};


// Thin view that adds the map-specific trait to the underlying range.
// Can be used for making `key_value_range` model `mapping_range`.
template<std::ranges::input_range R>
    requires ranges::key_value_range<R>
struct as_map_view : std::ranges::view_interface<as_map_view<R>>
{
    R base_ = R{};

public:
    using key_type    = range_key_t<R>;
    using mapped_type = range_mapped_t<R>;

    as_map_view() requires std::default_initializable<R> = default;
    constexpr explicit as_map_view(R base) : base_(std::move(base)) {}

    [[nodiscard]] constexpr R base() const& requires std::copy_constructible<R> { return base_; }
    [[nodiscard]] constexpr R base() && { return std::move(base_); }

    [[nodiscard]] constexpr auto begin()       { return std::ranges::begin(base_); }
    [[nodiscard]] constexpr auto end()         { return std::ranges::end(base_); }
    [[nodiscard]] constexpr auto begin() const requires std::ranges::range<R const> { return std::ranges::begin(base_); }
    [[nodiscard]] constexpr auto end()   const requires std::ranges::range<R const> { return std::ranges::end(base_); }

    [[nodiscard]] constexpr auto size()       requires std::ranges::sized_range<R>       { return std::ranges::size(base_); }
    [[nodiscard]] constexpr auto size() const requires std::ranges::sized_range<R const> { return std::ranges::size(base_); }
};

template<class R>
as_map_view(R&&) -> as_map_view<std::views::all_t<R>>;


namespace detail {

struct as_map_fn : std::ranges::range_adaptor_closure<as_map_fn>
{
    template<std::ranges::viewable_range R>
        requires ranges::key_value_range<R>
    [[nodiscard]] static constexpr auto operator()(R&& r)
    {
        return as_map_view{std::views::all(std::forward<R>(r))};
    }
};

} // detail

// Can be used for making `key_value_range` model `mapping_range`.
[[maybe_unused]] inline constexpr detail::as_map_fn as_map{};

} // iris::ranges

template<class V>
inline constexpr bool std::ranges::enable_borrowed_range<iris::ranges::as_map_view<V>>
    = std::ranges::enable_borrowed_range<V>;


namespace iris::ranges {

template<class C>
concept mapping_container =
    mapping_range<C> &&
    std::default_initializable<std::remove_cvref_t<C>> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type v
    ) {
        c.emplace(std::move(k), std::move(v));
    };

template<class K, class V>
struct dummy_mapping_container
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;

    dummy_mapping_container() = default;
    dummy_mapping_container(dummy_mapping_container const&) = delete;
    dummy_mapping_container(dummy_mapping_container&&) = delete;
    dummy_mapping_container& operator=(dummy_mapping_container const&) = delete;
    dummy_mapping_container& operator=(dummy_mapping_container&&) = delete;

    void emplace(K&&, V&&);
};

template<class C>
concept unique_mapping_container =
    mapping_container<C> &&
    requires(
        std::remove_cvref_t<C>& c,
        typename std::remove_cvref_t<C>::key_type k,
        typename std::remove_cvref_t<C>::mapped_type v
    ) {
        { c.try_emplace(std::move(k), std::move(v)).second } -> std::convertible_to<bool>;
        { c.insert_or_assign(std::move(k), std::move(v)).second } -> std::convertible_to<bool>;
    };

template<class K, class V>
struct dummy_unique_mapping_container : dummy_mapping_container<K, V>
{
    using key_type = K;
    using mapped_type = V;
    std::pair<K, V> const* begin() const;
    std::pair<K, V> const* end() const;

    using dummy_mapping_container<K, V>::dummy_mapping_container;

    std::pair<std::pair<K, V> const*, bool> try_emplace(K&&, V&&);
    std::pair<std::pair<K, V> const*, bool> insert_or_assign(K&&, V&&);
};


template<class T>
concept default_back_emplaceable = requires(T c) {
    std::forward<T>(c).emplace_back();
};

template<class T>
concept back_emplaceable =
    requires(T c, std::ranges::range_value_t<std::remove_cvref_t<T>> v) {
        std::forward<T>(c).emplace_back(std::move(v));
    } &&
    (
        !std::default_initializable<std::ranges::range_value_t<std::remove_cvref_t<T>>> ||
        default_back_emplaceable<T>
    );

template<class T>
concept default_front_emplaceable = requires(T c) {
    std::forward<T>(c).emplace_front();
};

template<class T>
concept front_emplaceable =
    requires(T c, std::ranges::range_value_t<std::remove_cvref_t<T>> v) {
        std::forward<T>(c).emplace_front(std::move(v));
    } &&
    (
        !std::default_initializable<std::ranges::range_value_t<std::remove_cvref_t<T>>> ||
        default_front_emplaceable<T>
    );

namespace detail {

struct front_fn
{
    template<class R>
    static constexpr bool has_front = requires(R&& r) { std::forward<R>(r).front(); };

    template<std::ranges::range R>
        requires has_front<R>
    [[nodiscard]] static constexpr auto&& operator()(R&& r) noexcept
    {
        return std::forward<R>(r).front();
    }

    template<std::ranges::input_range R>
        requires (!has_front<R>)
    [[nodiscard]] static constexpr auto&& operator()(R&& r) noexcept
    {
        return *std::ranges::begin(std::forward<R>(r));
    }
};

struct back_fn
{
    template<class R>
    static constexpr bool has_back = requires(R&& r) { std::forward<R>(r).back(); };

    template<std::ranges::range R>
        requires has_back<R>
    [[nodiscard]] static constexpr auto&& operator()(R&& r) noexcept
    {
        return std::forward<R>(r).back();
    }

    template<std::ranges::bidirectional_range R>
        requires (!has_back<R>)
    [[nodiscard]] static constexpr auto&& operator()(R&& r) noexcept
    {
        return *std::ranges::prev(std::ranges::end(std::forward<R>(r)));
    }
};

} // detail

[[nodiscard]] inline constexpr detail::front_fn front{};
[[nodiscard]] inline constexpr detail::back_fn back{};

namespace detail {

struct emplace_front_ref_fn
{
    template<front_emplaceable ContainerT, class... Args>
    [[nodiscard]] static constexpr auto& operator()(ContainerT& cont, Args&&... args)
        noexcept(noexcept(cont.emplace_front(std::forward<Args>(args)...)))
    {
        if constexpr (std::is_void_v<decltype(cont.emplace_front(std::forward<Args>(args)...))>) {
            cont.emplace_front(std::forward<Args>(args)...);
            return front(cont);
        } else {
            return cont.emplace_front(std::forward<Args>(args)...);
        }
    }
};

struct emplace_back_ref_fn
{
    template<back_emplaceable ContainerT, class... Args>
    [[nodiscard]] static constexpr auto& operator()(ContainerT& cont, Args&&... args)
        noexcept(noexcept(cont.emplace_back(std::forward<Args>(args)...)))
    {
        if constexpr (std::is_void_v<decltype(cont.emplace_back(std::forward<Args>(args)...))>) {
            cont.emplace_back(std::forward<Args>(args)...);
            return back(cont);
        } else {
            return cont.emplace_back(std::forward<Args>(args)...);
        }
    }
};

} // detail

[[nodiscard]] inline constexpr detail::emplace_front_ref_fn emplace_front_ref{};
[[nodiscard]] inline constexpr detail::emplace_back_ref_fn emplace_back_ref{};


namespace detail {

struct weak_pop_front_fn
{
    template<class ContainerT>
    static constexpr bool has_pop_front = requires (ContainerT& cont) {
        cont.pop_front();
    };

    template<class ContainerT>
        requires has_pop_front<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.pop_front()))
    {
        cont.pop_front();
    }

    template<std::ranges::range ContainerT>
        requires (!has_pop_front<ContainerT>)
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.erase(std::ranges::begin(cont))))
    {
        cont.erase(std::ranges::begin(cont));
    }
};

struct weak_pop_back_fn
{
    template<class ContainerT>
    static constexpr bool has_pop_back = requires (ContainerT& cont) {
        cont.pop_back();
    };

    template<class ContainerT>
        requires has_pop_back<ContainerT>
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.pop_back()))
    {
        cont.pop_back();
    }

    template<std::ranges::bidirectional_range ContainerT>
        requires (!has_pop_back<ContainerT>)
    static constexpr void operator()(ContainerT& cont)
        noexcept(noexcept(cont.erase(std::ranges::prev(std::ranges::end(cont)))))
    {
        cont.erase(std::ranges::prev(std::ranges::end(cont)));
    }
};

} // detail

[[nodiscard]] inline constexpr detail::weak_pop_front_fn weak_pop_front{};
[[nodiscard]] inline constexpr detail::weak_pop_back_fn weak_pop_back{};


template<class T>
concept default_emplaceable = requires(T c) {
    std::forward<T>(c).emplace();
};

template<class T>
concept emplaceable =
    requires(T c, std::ranges::range_value_t<std::remove_cvref_t<T>> v) {
        std::forward<T>(c).emplace(std::move(v));
    } &&
    (
        !std::default_initializable<std::ranges::range_value_t<std::remove_cvref_t<T>>> ||
        default_emplaceable<T>
    );

template<class T>
concept growable_array_writable = back_emplaceable<T> || emplaceable<T>;

template<class T>
struct dummy_growable_array
{
    T const* begin() const;
    T const* end() const;
    void emplace_back(T&&);
    void emplace_back() requires std::default_initializable<T>;
};

template<class T>
concept fixed_array_writable =
    !growable_array_writable<T> &&
    std::ranges::forward_range<T> &&
    std::ranges::sized_range<T> &&
    std::is_lvalue_reference_v<std::ranges::range_reference_t<T>> &&
    !std::is_const_v<std::remove_reference_t<std::ranges::range_reference_t<T>>> &&
    std::is_move_assignable_v<std::remove_cvref_t<std::ranges::range_value_t<T>>>;

template<class T>
using dummy_fixed_array = T[1];

} // iris::ranges

#endif

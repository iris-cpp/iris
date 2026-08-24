#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/detail/field.hpp>

#include <iris/alloy/utility.hpp>

#include <iris/string.hpp>

#include <iris/unicode/string.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <utility>
#include <type_traits>

namespace iris::marshal::json {

namespace detail {

template<class T>
struct native_t_impl
{
    using type = T;
};

template<StringLike T>
struct native_t_impl<T>
{
    using type = std::string;
};

} // detail

struct format : generic_format
{
    template<class K>
    static constexpr bool map_key = StringLike<K>;

    template<class K>
    static constexpr bool loadable_key = StringLike<K>;
};

} // iris::marshal::json

namespace iris::marshal {

template<StringLike T>
    requires (!std::same_as<char_type_for<T>, char>)
struct adapted_proxy_traits<T, json::format>
{
    using native_type = std::string;

    [[nodiscard]] static native_type const& to_native_type(native_type const& t) noexcept
    {
        return t;
    }

    [[nodiscard]] static native_type to_native_type(native_type const&& t)
    {
        return t;
    }

    [[nodiscard]] static native_type to_native_type(StringLike auto const& u)
    {
        return unicode::transcode<char>(u);
    }

    [[nodiscard]] static T from_native_type(native_type const& t)
    {
        return unicode::transcode<typename decltype(std::basic_string_view{std::declval<T>()})::value_type>(t);
    }
};

} // iris::marshal

namespace iris::marshal::json {

template<class T> concept serializable_proxy    = marshal::serializable_proxy<T, format>;
template<class T> concept serializable_class    = marshal::serializable_class<T, format>;
template<class T> concept serializable_optional = marshal::serializable_optional<T, format>;
template<class T> concept serializable_scalar   = marshal::serializable_scalar<T, format>;
template<class T> concept serializable_map      = marshal::serializable_map<T, format>;
template<class T> concept serializable_array    = marshal::serializable_array<T, format>;
template<class T> concept serializable_tuple    = marshal::serializable_tuple<T, format>;
template<class T> concept serializable          = marshal::serializable<T, format>;

template<class T> concept deserializable_proxy    = marshal::deserializable_proxy<T, format>;
template<class T> concept deserializable_class    = marshal::deserializable_class<T, format>;
template<class T> concept deserializable_optional = marshal::deserializable_optional<T, format>;
template<class T> concept deserializable_scalar   = marshal::deserializable_scalar<T, format>;
template<class T> concept deserializable_map      = marshal::deserializable_map<T, format>;
template<class T> concept deserializable_array    = marshal::deserializable_array<T, format>;
template<class T> concept deserializable_tuple    = marshal::deserializable_tuple<T, format>;
template<class T> concept deserializable          = marshal::deserializable<T, format>;

namespace detail {

struct save_fn
{
    template<serializable_scalar T>
    static void operator()(nlohmann::json& out, T const& value)
    {
        if constexpr (std::is_enum_v<T>) {
            out = std::to_underlying(value);
        } else {
            out = value;
        }
    }

    template<serializable_array R>
    static void operator()(nlohmann::json& out, R const& arr)
    {
        auto json_arr = nlohmann::json::array();

        for (auto const& elem : arr) {
            nlohmann::json elem_json;
            save_fn{}(elem_json, elem);
            json_arr.emplace_back(std::move(elem_json));
        }

        out = std::move(json_arr);
    }

    template<serializable_map MapT>
    static void operator()(nlohmann::json& out, MapT const& map)
    {
        auto json_map = nlohmann::json::object();

        for (auto const& [key, value] : map) {
            if constexpr (adapted_proxy<ranges::range_key_t<MapT>, format>) {
                auto&& json_key = adapted_proxy_traits<ranges::range_key_t<MapT>, format>::to_native_type(key);
                save_fn{}(json_map[json_key], value);
            } else {
                save_fn{}(json_map[std::basic_string_view{key}], value);
            }
        }

        out = std::move(json_map);
    }

    template<serializable_tuple TupleT>
    static void operator()(nlohmann::json& out, TupleT const& tup)
    {
        auto json_arr = nlohmann::json::array();

        alloy::for_each(tup, [&](auto const& elem) {
            nlohmann::json elem_json;
            save_fn{}(elem_json, elem);
            json_arr.emplace_back(std::move(elem_json));
        });

        out = std::move(json_arr);
    }

    template<serializable_class ClassT>
    static void operator()(nlohmann::json& out, ClassT const& klass)
    {
        auto json_map = nlohmann::json::object();

        constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
        alloy::for_each(fields, [&]<class T, auto GetMem, auto SetMem>(marshal::detail::field_definition<T, GetMem, SetMem> const& def) {
            save_fn{}(json_map[def.name], (klass.*GetMem)());
        });

        out = std::move(json_map);
    }

    template<serializable_optional OptionalT>
    static void operator()(nlohmann::json& out, OptionalT const& opt)
    {
        if (opt) {
            save_fn{}(out, *opt);
        } else {
            out = nullptr;
        }
    }

    template<serializable_proxy ProxyT>
    static void operator()(nlohmann::json& out, ProxyT const& proxy)
    {
        save_fn{}(out, adapted_proxy_traits<ProxyT, format>::to_native_type(proxy));
    }

    template<serializable T>
    [[nodiscard]] static nlohmann::json operator()(T const& value)
    {
        nlohmann::json out;
        save_fn{}(out, value);
        return out;
    }
};

// ----------------------------------------------------

struct load_fn
{
    template<deserializable_scalar T>
    static void operator()(nlohmann::json const& j, T& value)
    {
        if constexpr (std::is_arithmetic_v<T>) {
            value = j.get<T>();

        } else if constexpr (std::is_enum_v<T>) {
            value = static_cast<T>(j.get<std::underlying_type_t<T>>());

        } else if constexpr (StringLike<T>) {
            using CharT = decltype(std::basic_string_view{value})::value_type;
            value = unicode::transcode_ref<CharT>(j.get_ref<nlohmann::json::string_t const&>());

        } else {
            value = j.get_ref<T const&>();
        }
    }

    template<deserializable_array R>
    static void operator()(nlohmann::json const& j, R& arr)
    {
        R tmp;
        for (auto const& elem_json : j) {
            load_fn{}(elem_json, tmp.emplace_back());
        }
        arr = std::move(tmp);
    }

    template<deserializable_map MapT>
    static void operator()(nlohmann::json const& j, MapT& map)
    {
        MapT tmp;
        for (auto const& [json_key, json_value] : j.items()) {
            ranges::range_key_t<MapT> key;
            load_fn{}(json_key, key);

            ranges::range_mapped_t<MapT> value;
            load_fn{}(json_value, value);

            if constexpr (ranges::unique_mapping_container<MapT>) {
                tmp.insert_or_assign(std::move(key), std::move(value));
            } else {
                tmp.emplace(std::move(key), std::move(value));
            }
        }
        map = std::move(tmp);
    }

    template<deserializable_tuple TupleT>
    static void operator()(nlohmann::json const& j, TupleT& tup)
    {
        TupleT tmp;
        alloy::for_each(tmp, [&]<std::size_t I>(std::in_place_index_t<I>, auto& elem) {
            load_fn{}(j.at(I), elem);
        });
        tup = std::move(tmp);
    }

    template<deserializable_class ClassT>
    static void operator()(nlohmann::json const& j, ClassT& klass)
    {
        ClassT tmp;
        constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
        alloy::for_each(fields, [&]<class T, auto GetMem, auto SetMem>(marshal::detail::field_definition<T, GetMem, SetMem> const& def) {
            T value;
            load_fn{}(j[def.name], value);
            (tmp.*SetMem)(std::move(value));
        });
        klass = std::move(tmp);
    }

    template<deserializable_optional OptionalT>
    static void operator()(nlohmann::json const& j, OptionalT& opt)
    {
        if (j.is_null()) {
            opt = {};
        } else {
            adapted_optional_value_t<OptionalT> value;
            load_fn{}(j, value);
            opt = std::move(value);
        }
    }

    template<deserializable_proxy ProxyT>
    static void operator()(nlohmann::json const& j, ProxyT& proxy)
    {
        using traits = adapted_proxy_traits<ProxyT, format>;
        proxy = traits::from_native_type(
            j.get_ref<typename traits::native_type const&>()
        );
    }
};

} // detail

[[maybe_unused]] inline constexpr detail::save_fn save{};


template<deserializable T>
    requires requires(nlohmann::json const& j) {
        detail::load_fn{}(j, std::declval<T&>());
    }
[[nodiscard]] T load(nlohmann::json const& j)
{
    T value; // default-initialize
    detail::load_fn{}(j, value);
    return value;
}

template<deserializable T>
void load(nlohmann::json const& j, T& value)
{
    static_assert(!std::is_const_v<T>, "cannot deserialize into const variable");
    detail::load_fn{}(j, value);
}

} // iris::marshal::json

#endif

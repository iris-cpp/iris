#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/detail/field.hpp>

#include <iris/alloy/utility.hpp>

#include <iris/string.hpp>

#include <nlohmann/json.hpp>

#include <utility>
#include <type_traits>

namespace iris::marshal::json {

struct format
{
    template<class K>
    static constexpr bool key = StringLike<std::remove_cvref_t<K>>;
};

template<class T> concept serializable_proxy    = marshal::serializable_proxy<T, format>;
template<class T> concept serializable_class    = marshal::serializable_class<T, format>;
template<class T> concept serializable_optional = marshal::serializable_optional<T, format>;
template<class T> concept serializable_scalar   = marshal::serializable_scalar<T, format>;
template<class T> concept serializable_map      = marshal::serializable_map<T, format>;
template<class T> concept serializable_array    = marshal::serializable_array<T, format>;
template<class T> concept serializable_tuple    = marshal::serializable_tuple<T, format>;
template<class T> concept serializable          = marshal::serializable<T, format>;

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
            save_fn{}(json_map[std::basic_string_view{key}], value);
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
        alloy::for_each(fields, [&]<auto Mem>(marshal::detail::field_definition<Mem> const& def) {
            save_fn{}(json_map[def.name], (klass.*Mem)());
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
        save_fn{}(out, adapted_proxy_traits<ProxyT>::to_proxy(proxy));
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

// TODO
struct load_fn
{

};

} // detail

[[maybe_unused]] inline constexpr detail::save_fn save{};
[[maybe_unused]] inline constexpr detail::load_fn load{};

} // iris::marshal::json

#endif

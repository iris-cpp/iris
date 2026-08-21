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

template<class T> concept serializable_class    = marshal::serializable_class<T, format>;
template<class T> concept serializable_optional = marshal::serializable_optional<T, format>;
template<class T> concept serializable_scalar   = marshal::serializable_scalar<T, format>;
template<class T> concept serializable_map      = marshal::serializable_map<T, format>;
template<class T> concept serializable_array    = marshal::serializable_array<T, format>;
template<class T> concept serializable_tuple    = marshal::serializable_tuple<T, format>;
template<class T> concept serializable          = marshal::serializable<T, format>;

template<serializable_scalar T>
void save(nlohmann::json& out, T const& value);

template<serializable_array R>
void save(nlohmann::json& out, R const& arr);

template<serializable_map MapT>
void save(nlohmann::json& out, MapT const& map);

template<serializable_tuple TupleT>
void save(nlohmann::json& out, TupleT const& tup);

template<serializable_class ClassT>
void save(nlohmann::json& out, ClassT const& klass);

template<serializable_optional OptionalT>
void save(nlohmann::json& out, OptionalT const& opt);


template<serializable_scalar T>
void save(nlohmann::json& out, T const& value)
{
    if constexpr (std::is_enum_v<T>) {
        out = std::to_underlying(value);
    } else {
        out = value;
    }
}

template<serializable_array R>
void save(nlohmann::json& out, R const& arr)
{
    auto json_arr = nlohmann::json::array();

    for (auto const& elem : arr) {
        nlohmann::json elem_json;
        json::save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    }

    out = std::move(json_arr);
}

template<serializable_map MapT>
void save(nlohmann::json& out, MapT const& map)
{
    auto json_map = nlohmann::json::object();

    for (auto const& [key, value] : map) {
        json::save(json_map[std::basic_string_view{key}], value);
    }

    out = std::move(json_map);
}

template<serializable_tuple TupleT>
void save(nlohmann::json& out, TupleT const& tup)
{
    auto json_arr = nlohmann::json::array();

    alloy::for_each(tup, [&](auto const& elem) {
        nlohmann::json elem_json;
        json::save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    });

    out = std::move(json_arr);
}

template<serializable_class ClassT>
void save(nlohmann::json& out, ClassT const& klass)
{
    auto json_map = nlohmann::json::object();

    constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
    alloy::for_each(fields, [&]<auto Mem>(detail::field_definition<Mem> const& def) {
        json::save(json_map[def.name], (klass.*Mem)());
    });

    out = std::move(json_map);
}

template<serializable_optional OptionalT>
void save(nlohmann::json& out, OptionalT const& opt)
{
    if (opt) {
        json::save(out, *opt);
    } else {
        out = nullptr;
    }
}

// ----------------------------------------------------

// TODO: load


} // iris::marshal::json

#endif

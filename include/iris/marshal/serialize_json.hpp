#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>

#include <iris/alloy/utility.hpp>

#include <nlohmann/json.hpp>

namespace iris::marshal {

template<serializable_scalar T>
void save(nlohmann::json& json, T const& value);

template<serializable_array R>
void save(nlohmann::json& json, R const& arr);

template<serializable_map MapT>
void save(nlohmann::json& json, MapT const& map);

template<serializable_tuple TupleT>
void save(nlohmann::json& json, TupleT const& tup);

template<serializable_class ClassT>
void save(nlohmann::json& json, ClassT const& klass);

template<serializable_optional OptionalT>
void save(nlohmann::json& json, OptionalT const& opt);


template<serializable_scalar T>
void save(nlohmann::json& json, T const& value)
{
    json = value;
}

template<serializable_array R>
void save(nlohmann::json& json, R const& arr)
{
    auto json_arr = nlohmann::json::array();

    for (auto const& elem : arr) {
        nlohmann::json elem_json;
        marshal::save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    }

    json = std::move(json_arr);
}

template<serializable_map MapT>
void save(nlohmann::json& json, MapT const& map)
{
    auto json_map = nlohmann::json::object();

    for (auto const& [key, value] : map) {
        marshal::save(json_map[key], value);
    }

    json = std::move(json_map);
}

template<serializable_tuple TupleT>
void save(nlohmann::json& json, TupleT const& tup)
{
    auto json_arr = nlohmann::json::array();

    alloy::for_each(tup, [&](auto const& elem) {
        nlohmann::json elem_json;
        marshal::save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    });

    json = std::move(json_arr);
}

template<serializable_class ClassT>
void save(nlohmann::json& json, ClassT const& klass)
{
    auto json_map = nlohmann::json::object();

    constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
    alloy::for_each(fields, [&]<auto Mem>(detail::field_definition<Mem> const& def) {
        marshal::save(json_map[def.name], (klass.*Mem)());
    });

    json = std::move(json_map);
}

template<serializable_optional OptionalT>
void save(nlohmann::json& json, OptionalT const& opt)
{
    if (opt) {
        marshal::save(json, *opt);
    } else {
        json = nullptr;
    }
}

// ----------------------------------------------------

// TODO: load


} // iris::marshal

#endif

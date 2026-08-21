#ifndef IRIS_ZZ_SFIELD_SERIALIZE_JSON_HPP
#define IRIS_ZZ_SFIELD_SERIALIZE_JSON_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/sfield/serialize_traits.hpp>

#include <iris/alloy/utility.hpp>

#include <nlohmann/json.hpp>

namespace iris::sfield {

template<serializable_primitive T>
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
        save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    }

    json = std::move(json_arr);
}

template<serializable_map MapT>
void save(nlohmann::json& json, MapT const& map)
{
    auto json_map = nlohmann::json::object();

    for (auto const& [key, value] : map) {
        save(json_map[key], value);
    }

    json = std::move(json_map);
}

template<serializable_tuple TupleT>
void save(nlohmann::json& json, TupleT const& tup)
{
    auto json_arr = nlohmann::json::array();

    alloy::for_each(tup, [&](auto const& elem) {
        nlohmann::json elem_json;
        save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    });

    json = std::move(json_arr);
}

template<serializable_class ClassT>
void save(nlohmann::json& json, ClassT const& klass)
{
    auto json_map = nlohmann::json::object();

    constexpr auto const& fields = detail::adapted_class<ClassT>::fields;
    alloy::for_each(fields, [&]<auto Mem>(detail::field_definition<Mem> const& def) {
        save(json_map[def.name], (klass.*Mem)());
    });

    json = std::move(json_map);
}


// ----------------------------------------------------


template<serializable_primitive T>
void load(nlohmann::json const& json, T& value)
{
    value = json.get_ref<T>();
}

template<serializable_array R>
void load(nlohmann::json const& json, R& arr)
{
    for (auto const& elem_json : json) {
        std::ranges::range_value_t<R> elem;
        load(elem_json, elem);
        arr.emplace_back(std::move(elem));
    }
}

template<serializable_map MapT>
void load(nlohmann::json const& json, MapT& map)
{
    if constexpr (requires (std::size_t count) {
        map.reserve(count);
    }) {
        map.reserve(json.size());
    }
    for (auto const& [json_key, json_value] : json.items()) {
        typename MapT::mapped_type value;
        load(json_value, value);
        map.emplace(json_key, std::move(value));
    }
}

template<serializable_tuple TupleT>
void load(nlohmann::json const& json, TupleT& tup)
{
    auto json_arr = nlohmann::json::array();

    alloy::for_each(tup, [&](auto const& elem) {
        nlohmann::json elem_json;
        save(elem_json, elem);
        json_arr.emplace_back(std::move(elem_json));
    });

    json = std::move(json_arr);
}

template<serializable_class ClassT>
void load(nlohmann::json const& json, ClassT& klass)
{
    auto json_map = nlohmann::json::object();

    constexpr auto const& fields = detail::adapted_class<ClassT>::fields;
    alloy::for_each(fields, [&]<auto Mem>(detail::field_definition<Mem> const& def) {
        save(json_map[def.name], (klass.*Mem)());
    });

    json = std::move(json_map);
}


} // iris::sfield

#endif

#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_JSON_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize.hpp>

#include <iris/unicode/string.hpp>

#include <iris/string.hpp>

#include <nlohmann/json.hpp>

#include <vector>
#include <string>
#include <string_view>
#include <utility>
#include <type_traits>

namespace iris::marshal::json {

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

class dom_writer
{
public:
    using format = json::format;
    using document_type = nlohmann::json;

    explicit dom_writer(document_type& node)
        : node_(&node)
    {}

    template<class T>
        requires format::template loadable_scalar<T>
    void scalar(T const& value)
    {
        if constexpr (std::is_enum_v<T>) {
            slot() = std::to_underlying(value);
        } else {
            slot() = value;
        }
    }

    void null()
    {
        slot() = nullptr;
    }

    void begin_array()
    {
        stack_.push_back(&(slot() = document_type::array()));
    }
    void end_array()
    {
        stack_.pop_back();
    }

    void begin_object()
    {
        stack_.push_back(&(slot() = document_type::object()));
    }
    void map_key(std::string_view k)
    {
        pending_key_.assign(k);
    }
    void end_object()
    {
        stack_.pop_back();
    }

private:
    [[nodiscard]] document_type& slot()
    {
        if (stack_.empty()) return *node_;
        auto& parent = *stack_.back();
        return parent.is_object() ? parent[pending_key_] : parent.emplace_back();
    }

    nlohmann::json* node_;
    std::vector<nlohmann::json*> stack_;
    std::string pending_key_;
};

[[maybe_unused]] inline constexpr basic_save_fn<dom_writer> save{};


class dom_reader
{
public:
    using format = json::format;
    using document_type = nlohmann::json;

    explicit dom_reader(document_type const& node)
        : node_{&node}
    {}

    [[nodiscard]] bool is_null() const
    {
        return node_->is_null();
    }

    template<class T>
        requires format::template loadable_scalar<T>
    [[nodiscard]] bool scalar(T& value) const
    {
        if constexpr (std::same_as<T, bool>) {
            if (!node_->is_boolean()) return false;
            value = node_->get<bool>();

        } else if constexpr (std::is_integral_v<T>) {
            if (!node_->is_number_integer() && !node_->is_number_unsigned()) return false;
            auto const wide = node_->get<std::int64_t>();
            if (!std::in_range<T>(wide)) return false;
            value = static_cast<T>(wide);

        } else if constexpr (std::is_floating_point_v<T>) {
            if (!node_->is_number()) return false;
            value = node_->get<T>();

        } else if constexpr (std::is_enum_v<T>) {
            std::underlying_type_t<T> underlying;
            if (!scalar(underlying)) return false;
            value = static_cast<T>(underlying);

        } else { // string-like
            if (!node_->is_string()) return false;
            value = std::string_view{node_->get_ref<document_type::string_t const&>()};
        }
        return true;
    }

    template<class VisitorT>
    [[nodiscard]] bool array(VisitorT&& vis) const
    {
        if (!node_->is_array()) return false;

        for (auto const& elem : *node_) {
            dom_reader elem_rd{elem};
            vis(elem_rd);
        }
        return true;
    }

    template<class VisitorT>
    [[nodiscard]] bool object(VisitorT&& vis) const
    {
        if (!node_->is_object()) return false;

        for (auto const& [map_key, member] : node_->items()) {
            dom_reader member_rd{member};
            vis(std::string_view{map_key}, member_rd);
        }
        return true;
    }

private:
    document_type const* node_;
};

template<deserializable T>
    requires requires(nlohmann::json const& j) {
        basic_load_fn<dom_reader>{}(j, std::declval<T&>());
    }
[[nodiscard]] T load(nlohmann::json const& j)
{
    T value; // default-initialize
    basic_load_fn<dom_reader>{}(j, value);
    return value;
}

template<deserializable T>
void load(nlohmann::json const& j, T& value)
{
    basic_load_fn<dom_reader>{}(j, value);
}

} // iris::marshal::json

#endif

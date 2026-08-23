#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/detail/field.hpp>

#include <iris/alloy/utility.hpp>

#include <string_view>

namespace iris::marshal {

template<class W>
concept writer =
    requires {
        typename W::format;
    } &&
    requires(W& w, std::string_view map_key) {
        w.null();
        w.begin_array();
        w.end_array();
        w.begin_object();
        w.map_key(map_key);
        w.end_object();
    };

template<writer W>
struct basic_save_fn
{
    using format = W::format;

    template<serializable<format> T>
        requires requires {
            typename W::result_type;
        } && std::constructible_from<W, typename W::result_type&>
    static constexpr void operator()(W::result_type& res, T const& value)
    {
        W w{res};
        basic_save_fn{}(w, value);
    }

    template<serializable_scalar<format> T>
        requires requires(W& w, T const& value) { w.scalar(value); }
    static constexpr void operator()(W& w, T const& value)
    {
        w.scalar(value);
    }

    template<serializable_optional<format> OptionalT>
    static constexpr void operator()(W& w, OptionalT const& opt)
    {
        if (opt) {
            basic_save_fn{}(w, *opt);
        } else {
            w.null();
        }
    }

    template<serializable_proxy<format> ProxyT>
    static constexpr void operator()(W& w, ProxyT const& proxy)
    {
        basic_save_fn{}(w, adapted_proxy_traits<ProxyT, format>::to_native_type(proxy));
    }

    template<serializable_array<format> R>
    static constexpr void operator()(W& w, R const& arr)
    {
        w.begin_array();
        for (auto const& elem : arr) {
            basic_save_fn{}(w, elem);
        }
        w.end_array();
    }

    template<serializable_map<format> MapT>
    static constexpr void operator()(W& w, MapT const& map)
    {
        w.begin_object();
        for (auto const& [k, v] : map) {
            if constexpr (adapted_proxy<ranges::range_key_t<MapT>, format>) {
                w.map_key(adapted_proxy_traits<ranges::range_key_t<MapT>, format>::to_native_type(k));
            } else {
                w.map_key(k);
            }
            basic_save_fn{}(w, v);
        }
        w.end_object();
    }

    template<serializable_tuple<format> TupleT>
    static constexpr void operator()(W& w, TupleT const& tup)
    {
        w.begin_array();
        alloy::for_each(tup, [&](auto const& elem) {
            basic_save_fn{}(w, elem);
        });
        w.end_array();
    }

    template<serializable_class<format> ClassT>
    static constexpr void operator()(W& w, ClassT const& klass)
    {
        w.begin_object();
        constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
        alloy::for_each(fields, [&]<class T, auto GetMem, auto SetMem>(detail::field_definition<T, GetMem, SetMem> const& def) {
            if constexpr (adapted_proxy<decltype(def.name), format>) {
                w.map_key(adapted_proxy_traits<decltype(def.name), format>::to_native_type(def.name));
            } else {
                w.map_key(def.name);
            }
            basic_save_fn{}(w, (klass.*GetMem)());
        });
        w.end_object();
    }
};

} // iris::marshal

#endif

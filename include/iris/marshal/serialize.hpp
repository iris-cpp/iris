#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/detail/field.hpp>

#include <iris/alloy/utility.hpp>

#include <string_view>

namespace iris::marshal {

template<class WriterT>
concept writer =
    requires {
        typename WriterT::format;
    } &&
    requires(WriterT& wr, std::string_view map_key) {
        wr.null();
        wr.begin_array();
        wr.end_array();
        wr.begin_object();
        wr.map_key(map_key);
        wr.end_object();
    };

template<writer WriterT>
struct basic_save_fn
{
    using format = WriterT::format;

    template<serializable<format> T>
        requires requires {
            typename WriterT::result_type;
        } && std::constructible_from<WriterT, typename WriterT::result_type&>
    static constexpr void operator()(WriterT::result_type& res, T const& value)
    {
        WriterT wr{res};
        basic_save_fn{}(wr, value);
    }

    template<serializable_scalar<format> T>
        requires requires(WriterT& wr, T const& value) { wr.scalar(value); }
    static constexpr void operator()(WriterT& wr, T const& value)
    {
        wr.scalar(value);
    }

    template<serializable_optional<format> OptionalT>
    static constexpr void operator()(WriterT& wr, OptionalT const& opt)
    {
        if (opt) {
            basic_save_fn{}(wr, *opt);
        } else {
            wr.null();
        }
    }

    template<serializable_proxy<format> ProxyT>
    static constexpr void operator()(WriterT& wr, ProxyT const& proxy)
    {
        basic_save_fn{}(wr, adapted_proxy_traits<ProxyT, format>::to_native_type(proxy));
    }

    template<serializable_array<format> R>
    static constexpr void operator()(WriterT& wr, R const& arr)
    {
        wr.begin_array();
        for (auto const& elem : arr) {
            basic_save_fn{}(wr, elem);
        }
        wr.end_array();
    }

    template<serializable_map<format> MapT>
    static constexpr void operator()(WriterT& wr, MapT const& map)
    {
        wr.begin_object();
        for (auto const& [k, v] : map) {
            if constexpr (adapted_proxy<ranges::range_key_t<MapT>, format>) {
                wr.map_key(adapted_proxy_traits<ranges::range_key_t<MapT>, format>::to_native_type(k));
            } else {
                wr.map_key(k);
            }
            basic_save_fn{}(wr, v);
        }
        wr.end_object();
    }

    template<serializable_tuple<format> TupleT>
    static constexpr void operator()(WriterT& wr, TupleT const& tup)
    {
        wr.begin_array();
        alloy::for_each(tup, [&](auto const& elem) {
            basic_save_fn{}(wr, elem);
        });
        wr.end_array();
    }

    template<serializable_class<format> ClassT>
    static constexpr void operator()(WriterT& wr, ClassT const& klass)
    {
        wr.begin_object();
        constexpr auto const& fields = adapted_class_traits<ClassT>::fields;
        alloy::for_each(fields, [&]<class T, auto GetMem, auto SetMem>(detail::field_definition<T, GetMem, SetMem> const& def) {
            if constexpr (adapted_proxy<decltype(def.name), format>) {
                wr.map_key(adapted_proxy_traits<decltype(def.name), format>::to_native_type(def.name));
            } else {
                wr.map_key(def.name);
            }
            basic_save_fn{}(wr, (klass.*GetMem)());
        });
        wr.end_object();
    }
};

} // iris::marshal

#endif

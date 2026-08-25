#ifndef IRIS_ZZ_MARSHAL_SERIALIZE_HPP
#define IRIS_ZZ_MARSHAL_SERIALIZE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/marshal/serialize_traits.hpp>
#include <iris/marshal/detail/field.hpp>

#include <iris/alloy/utility.hpp>

#include <string_view>
#include <bitset>

namespace iris::marshal {

struct load_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};


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
            typename WriterT::document_type;
        } && std::constructible_from<WriterT, typename WriterT::document_type&>
    static constexpr void operator()(WriterT::document_type& res, T const& value)
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
        alloy::for_each(fields, [&]<class FieldT, auto GetMem, auto SetMem>(detail::field_definition<FieldT, GetMem, SetMem> const& def) {
            if constexpr (adapted_proxy<decltype(def.name), format>) {
                wr.map_key(adapted_proxy_traits<decltype(def.name), format>::to_native_type(def.name));
            } else {
                wr.map_key(def.name);
            }
            basic_save_fn{}(wr, def.get(klass));
        });
        wr.end_object();
    }
};


namespace detail {

struct noop_element_visitor
{
    template<class R>
    void operator()(R&) const
    {}
};
struct noop_member_visitor
{
    template<class R>
    void operator()(std::string_view, R&) const
    {}
};

} // detail

template<class ReaderT>
concept reader =
    requires {
        typename ReaderT::format;
    } &&
    requires(ReaderT& rd) {
        { rd.is_null() } -> std::convertible_to<bool>;
        { rd.array(detail::noop_element_visitor{}) } -> std::convertible_to<bool>;
        { rd.object(detail::noop_member_visitor{}) } -> std::convertible_to<bool>;
    };

template<reader ReaderT>
struct basic_load_fn
{
    using format = ReaderT::format;

    template<class T>
        requires
            requires { typename ReaderT::document_type; } &&
            std::constructible_from<ReaderT, typename ReaderT::document_type const&> &&
            deserializable<T, format>
    static constexpr void operator()(ReaderT::document_type const& doc, T& value)
    {
        ReaderT rd{doc};
        basic_load_fn{}(rd, value);
    }

    template<deserializable_scalar<format> T>
        requires requires(ReaderT& rd, T& value) {
            { rd.scalar(value) } -> std::convertible_to<bool>;
        }
    static constexpr void operator()(ReaderT& rd, T& value)
    {
        if (!rd.scalar(value)) {
            throw load_error{"scalar: kind mismatch"};
        }
    }

    template<deserializable_optional<format> OptionalT>
    static constexpr void operator()(ReaderT& rd, OptionalT& opt)
    {
        using value_type = adapted_optional_traits<OptionalT>::value_type;

        if (rd.is_null()) {
            opt = OptionalT{};
            return;
        }

        value_type tmp{};
        basic_load_fn{}(rd, tmp);
        opt = std::move(tmp);
    }

    template<deserializable_proxy<format> ProxyT>
    static constexpr void operator()(ReaderT& rd, ProxyT& proxy)
    {
        using native_type = adapted_proxy_traits<ProxyT, format>::native_type;

        native_type tmp{};
        basic_load_fn{}(rd, tmp);
        proxy = adapted_proxy_traits<ProxyT, format>::from_native_type(std::move(tmp));
    }

    template<deserializable_array<format> R>
        requires ranges::growable_array_writable<R&>
    static constexpr void operator()(ReaderT& rd, R& arr)
    {
        R tmp{};

        bool const ok = rd.array([&](ReaderT& elem_rd) {
            std::ranges::range_value_t<R> elem{};
            basic_load_fn{}(elem_rd, elem);

            if constexpr (ranges::back_emplaceable<R>) {
                tmp.emplace_back(std::move(elem));
            } else {
                tmp.emplace(std::move(elem));
            }
        });

        if (!ok) {
            throw load_error{"array: kind mismatch"};
        }

        arr = std::move(tmp);
    }

    template<deserializable_array<format> R>
        requires ranges::fixed_array_writable<R&>
    static constexpr void operator()(ReaderT& rd, R& arr)
    {
        auto it = std::ranges::begin(arr);
        auto const last = std::ranges::end(arr);

        bool const ok = rd.array([&](ReaderT& elem_rd) {
            if (it == last) {
                throw load_error{"array: too many elements"};
            }
            basic_load_fn{}(elem_rd, *it);
            ++it;
        });

        if (!ok) {
            throw load_error{"array: kind mismatch"};
        }
        if (it != last) {
            throw load_error{"array: too few elements"};
        }
    }

    template<deserializable_map<format> MapT>
    static constexpr void operator()(ReaderT& rd, MapT& map)
    {
        using key_type    = MapT::key_type;
        using mapped_type = MapT::mapped_type;

        MapT tmp{};

        bool const ok = rd.object([&](std::string_view map_key, ReaderT& member_rd) {
            key_type k = [&] {
                if constexpr (adapted_proxy<key_type, format>) {
                    using native_type = adapted_proxy_traits<key_type, format>::native_type;
                    return adapted_proxy_traits<key_type, format>::from_native_type(native_type{map_key});
                } else {
                    return key_type{map_key};
                }
            }();

            mapped_type v{};
            basic_load_fn{}(member_rd, v);

            if constexpr (ranges::unique_mapping_container<MapT>) {
                if (!tmp.try_emplace(std::move(k), std::move(v)).second) {
                    throw load_error{"object: duplicate key"};
                }
            } else {
                tmp.emplace(std::move(k), std::move(v));
            }
        });

        if (!ok) {
            throw load_error{"object: kind mismatch"};
        }
        map = std::move(tmp);
    }

    template<deserializable_tuple<format> TupleT>
    static constexpr void operator()(ReaderT& rd, TupleT& tup)
    {
        constexpr std::size_t size = alloy::tuple_size_v<TupleT>;
        std::size_t i = 0;

        bool const ok = rd.array([&](ReaderT& elem_rd) {
            if (i >= size) {
                throw load_error{"tuple: too many elements"};
            }
            alloy::visit_at(i, [&](auto& elem) {
                basic_load_fn{}(elem_rd, elem);
            }, tup);
            ++i;
        });

        if (!ok) {
            throw load_error{"tuple: kind mismatch"};
        }
        if (i != size) {
            throw load_error{"tuple: too few elements"};
        }
    }

    template<deserializable_class<format> ClassT>
    static constexpr void operator()(ReaderT& rd, ClassT& klass)
    {
        constexpr auto const& fields = adapted_class_traits<ClassT>::fields;

        std::bitset<alloy::tuple_size_v<std::remove_cvref_t<decltype(fields)>>>
        seen;

        bool const ok = rd.object([&](std::string_view map_key, ReaderT& member_rd) {
            std::size_t index = 0;
            bool matched = false;

            alloy::for_each(fields, [&]<class FieldT, auto GetMem, auto SetMem>(detail::field_definition<FieldT, GetMem, SetMem> const& def) {
                if (!matched && basic_load_fn::field_name_matches(def.name, map_key)) {
                    FieldT tmp{};
                    basic_load_fn{}(member_rd, tmp);
                    def.set(klass, std::move(tmp));

                    seen.set(index);
                    matched = true;
                }
                ++index;
            });
            // unmatched `map_key` is ignored ...
        });

        if (!ok) {
            throw load_error{"object: kind mismatch"};
        }

        std::size_t index = 0;
        alloy::for_each(fields, [&](auto&& /* def */) {
            if (!seen.test(index)) {
                throw load_error{"object: missing required field"};
            }
            ++index;
        });
    }

private:
    template<class NameT>
    [[nodiscard]] static constexpr bool field_name_matches(NameT const& name, std::string_view map_key)
    {
        if constexpr (adapted_proxy<NameT, format>) {
            return adapted_proxy_traits<NameT, format>::to_native_type(name) == map_key;
        } else {
            return name == map_key;
        }
    }
};

} // iris::marshal

#endif

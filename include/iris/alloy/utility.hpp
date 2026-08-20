#ifndef IRIS_ZZ_ALLOY_UTILITY_HPP
#define IRIS_ZZ_ALLOY_UTILITY_HPP

/*=============================================================================
    Copyright (c) 2025 Yaito Kakeyama
    Copyright (c) 2026 The Iris Project Contributors

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <iris/alloy/traits.hpp>
#include <iris/alloy/tuple.hpp>

#include <functional>
#include <type_traits>
#include <utility>

#include <cstddef>

namespace iris::alloy {

namespace detail {

template<class... Ts>
struct type_list;

template<class ResultTypeList, class IndexSeqList, class... Tuples>
struct tuple_cat_result_impl;

template<class... ResultTypes>
struct tuple_cat_result_impl<type_list<ResultTypes...>, type_list<>>
{
    using type = tuple<ResultTypes...>;
};

template<class... ResultTypes, std::size_t... Is, class... IndexSeqs, class Tuple, class... Tuples>
struct tuple_cat_result_impl<type_list<ResultTypes...>, type_list<std::index_sequence<Is...>, IndexSeqs...>, Tuple, Tuples...>
    : tuple_cat_result_impl<type_list<ResultTypes..., tuple_element_t<Is, std::remove_cvref_t<Tuple>>...>, type_list<IndexSeqs...>, Tuples...> {};

template<class... Tuples>
struct tuple_cat_result : tuple_cat_result_impl<type_list<>, type_list<std::make_index_sequence<tuple_size_v<std::remove_cvref_t<Tuples>>>...>, Tuples...> {};

template<class ResultTuple, class IndexSeqList, class... Tuples>
struct tuple_cat_impl_base;

template<class ResultTuple>
struct tuple_cat_impl_base<ResultTuple, type_list<>>
{
    template<class... Args>
    static constexpr bool nothrow = std::is_nothrow_constructible_v<ResultTuple, Args...>;

    template<class... Args>
    static constexpr ResultTuple apply(Args&&... args) noexcept(nothrow<Args...>)
    {
        return ResultTuple(std::forward<Args>(args)...);
    }
};

template<class ResultTuple, std::size_t... Is, class... IndexSeqs, class Tuple, class... Tuples>
struct tuple_cat_impl_base<ResultTuple, type_list<std::index_sequence<Is...>, IndexSeqs...>, Tuple, Tuples...>
{
    template<class... Args>
    static constexpr bool nothrow =
        tuple_cat_impl_base<ResultTuple, type_list<IndexSeqs...>, Tuples...>::template nothrow<Args...> && (is_nothrow_gettable_v<Is, Tuple> && ...);

    template<class... Args>
    static constexpr ResultTuple apply(Tuple&& tuple, Tuples&&... tuples, Args&&... args) noexcept(nothrow<Args...>)
    {
        return tuple_cat_impl_base<ResultTuple, type_list<IndexSeqs...>, Tuples...>::apply(std::forward<Tuples>(tuples)..., std::forward<Args>(args)...,
                                                                                           alloy::get<Is>(std::forward<Tuple>(tuple))...);
    }
};

template<class... Tuples>
struct tuple_cat_impl
{
    using Base = tuple_cat_impl_base<typename tuple_cat_result<Tuples...>::type,
                                     detail::type_list<std::make_index_sequence<tuple_size_v<std::remove_cvref_t<Tuples>>>...>, Tuples...>;

    static constexpr bool nothrow = Base::template nothrow<>;

    static constexpr typename tuple_cat_result<Tuples...>::type apply(Tuples&&... tuples) noexcept(nothrow)
    {
        return Base::apply(std::forward<Tuples>(tuples)...);
    }
};

template<std::size_t N, class TIndexSeq, class UIndexSeq>
struct index_sequence_split_impl;

template<std::size_t N, std::size_t... Is, std::size_t J, std::size_t... Js>
    requires (sizeof...(Is) < N)
struct index_sequence_split_impl<N, std::index_sequence<Is...>, std::index_sequence<J, Js...>>
    : index_sequence_split_impl<N, std::index_sequence<Is..., J>, std::index_sequence<Js...>> {};

template<std::size_t N, std::size_t... Is, std::size_t... Js>
    requires (sizeof...(Is) == N)
struct index_sequence_split_impl<N, std::index_sequence<Is...>, std::index_sequence<Js...>>
{
    using head = std::index_sequence<Is...>;
    using tail = std::index_sequence<Js...>;
};

template<std::size_t N, class IndexSeq>
struct index_sequence_split : index_sequence_split_impl<N, std::index_sequence<>, IndexSeq> {};

template<std::size_t N, class IndexSeq>
struct index_sequence_take
{
    using type = typename index_sequence_split<N, IndexSeq>::head;
};

template<std::size_t N, class IndexSeq>
using index_sequence_take_t = typename index_sequence_take<N, IndexSeq>::type;

template<std::size_t N, class IndexSeq>
struct index_sequence_drop
{
    using type = typename index_sequence_split<N, IndexSeq>::tail;
};

template<std::size_t N, class IndexSeq>
using index_sequence_drop_t = typename index_sequence_drop<N, IndexSeq>::type;

template<std::size_t From, std::size_t To, class IndexSeq>
struct index_sequence_subrange
{
    using type = index_sequence_take_t<To - From, index_sequence_drop_t<From, IndexSeq>>;
};

template<std::size_t From, std::size_t To, class IndexSeq>
using index_sequence_subrange_t = typename index_sequence_subrange<From, To, IndexSeq>::type;

template<class IndexSeq>
struct index_sequence_sum;

template<std::size_t... Is>
struct index_sequence_sum<std::index_sequence<Is...>> : std::integral_constant<std::size_t, (0 + ... + Is)> {};

template<class IndexSeq>
inline constexpr std::size_t index_sequence_sum_v = index_sequence_sum<IndexSeq>::value;

template<class IdxIndexSeq, class ValIndexSeq>
struct index_sequence_cumulative_sum_impl;

template<std::size_t... Idxs, class ValIndexSeq>
struct index_sequence_cumulative_sum_impl<std::index_sequence<Idxs...>, ValIndexSeq>
{
    using type = std::index_sequence<0, index_sequence_sum_v<index_sequence_take_t<Idxs + 1, ValIndexSeq>>...>;
};

template<class IndexSeq>
struct index_sequence_cumulative_sum;

template<std::size_t... Is>
struct index_sequence_cumulative_sum<std::index_sequence<Is...>>
    : index_sequence_cumulative_sum_impl<std::make_index_sequence<sizeof...(Is)>, std::index_sequence<Is...>> {};

template<class IndexSeq>
using index_sequence_cumulative_sum_t = typename index_sequence_cumulative_sum<IndexSeq>::type;

template<class IndexSeq, class FromIndexSeq, class ToIndexSeq>
struct index_sequence_segment_impl;

template<class IndexSeq, std::size_t... Froms, std::size_t... Tos>
struct index_sequence_segment_impl<IndexSeq, std::index_sequence<Froms...>, std::index_sequence<Tos...>>
{
    using type = type_list<index_sequence_subrange_t<Froms, Tos, IndexSeq>...>;
};

template<class IndexSeq, std::size_t... Sizes>
struct index_sequence_segment;

template<std::size_t... Is, std::size_t... Sizes>
struct index_sequence_segment<std::index_sequence<Is...>, Sizes...>
{
    using CumSumIndexSeq = index_sequence_cumulative_sum_t<std::index_sequence<Sizes...>>;

    using type = typename index_sequence_segment_impl<std::index_sequence<Is...>, index_sequence_take_t<sizeof...(Sizes), CumSumIndexSeq>,
                                                      index_sequence_drop_t<1, CumSumIndexSeq>>::type;
};

template<class IndexSeq, std::size_t... Sizes>
using index_sequence_segment_t = typename index_sequence_segment<IndexSeq, Sizes...>::type;

template<class Tuple, class IndexSeq>
struct tuple_from_tuple_and_index_sequence;

template<class Tuple, std::size_t... Is>
struct tuple_from_tuple_and_index_sequence<Tuple, std::index_sequence<Is...>>
{
    using type = tuple<tuple_element_t<Is, std::remove_cvref_t<Tuple>>...>;
};

template<class Tuple, class IndexSeq>
using tuple_from_tuple_and_index_sequence_t = typename tuple_from_tuple_and_index_sequence<Tuple, IndexSeq>::type;

template<class Tuple, class SegmentedIndexSeqList>
struct tuple_split_result_impl;

template<class Tuple, class... SegmentedIndexSeqs>
struct tuple_split_result_impl<Tuple, type_list<SegmentedIndexSeqs...>>
{
    using type = tuple<tuple_from_tuple_and_index_sequence_t<Tuple, SegmentedIndexSeqs>...>;
};

template<class Tuple, std::size_t... Sizes>
struct tuple_split_result
{
    using type = typename tuple_split_result_impl<Tuple, index_sequence_segment_t<std::make_index_sequence<tuple_size_v<std::remove_cvref_t<Tuple>>>, Sizes...>>::type;
};

template<class ResultInnerTuple, class Tuple, class IndexSeq>
struct tuple_split_make_inner;

template<class ResultInnerTuple, class Tuple, std::size_t... Is>
struct tuple_split_make_inner<ResultInnerTuple, Tuple, std::index_sequence<Is...>>
{
    static constexpr bool nothrow = std::conjunction_v<
        is_nothrow_gettable<Is, Tuple>...,
        std::is_nothrow_constructible<ResultInnerTuple, tuple_get_t<Is, Tuple>...>
    >;

    static constexpr ResultInnerTuple apply(Tuple&& t) noexcept(nothrow)
    {
        return ResultInnerTuple(alloy::get<Is>(std::forward<Tuple>(t))...);
    }
};

template<class ResultTuple, class Tuple, class SegmentedIndexSeqList>
struct tuple_split_make_outer;

template<class... ResultInnerTuples, class Tuple, class... SegmentedIndexSeqs>
struct tuple_split_make_outer<tuple<ResultInnerTuples...>, Tuple, type_list<SegmentedIndexSeqs...>>
{
    static constexpr bool nothrow = (tuple_split_make_inner<ResultInnerTuples, Tuple, SegmentedIndexSeqs>::nothrow && ...);

    static constexpr tuple<ResultInnerTuples...> apply(Tuple&& t) noexcept(nothrow)
    {
        return tuple<ResultInnerTuples...>(tuple_split_make_inner<ResultInnerTuples, Tuple, SegmentedIndexSeqs>::apply(std::forward<Tuple>(t))...);
    }
};

template<class Tuple, std::size_t... Sizes>
struct tuple_split_impl : tuple_split_make_outer<typename tuple_split_result<Tuple, Sizes...>::type, Tuple,
                                                 index_sequence_segment_t<std::make_index_sequence<tuple_size_v<std::remove_cvref_t<Tuple>>>, Sizes...>> {};

template<class From, class To, class IndexSeq = std::make_index_sequence<tuple_size_v<std::remove_cvref_t<From>>>>
struct tuple_assign_impl;

template<class From, class To, std::size_t... Is>
struct tuple_assign_impl<From, To, std::index_sequence<Is...>>
{
    static constexpr bool nothrow = std::conjunction_v<std::conjunction<is_nothrow_gettable<Is, From>, is_nothrow_gettable<Is, To>>...,
                                                       std::is_nothrow_assignable<tuple_get_t<Is, To>, tuple_get_t<Is, From>>...>;

    static constexpr void apply(From&& from, To&& to) noexcept(nothrow)
    {
        ((void)(alloy::get<Is>(std::forward<To>(to)) = alloy::get<Is>(std::forward<From>(from))), ...);
    }
};

template<class Tuple, class IndexSeq>
struct tuple_ref_result_impl;

template<class Tuple, std::size_t... Is>
struct tuple_ref_result_impl<Tuple, std::index_sequence<Is...>>
{
    using type = tuple<tuple_element_t<Is, Tuple>&...>;
};

template<class Tuple>
struct tuple_ref_result : tuple_ref_result_impl<Tuple, std::make_index_sequence<tuple_size_v<Tuple>>> {};

template<class IndexSeq>
struct for_each_impl;

template<std::size_t... Is>
struct for_each_impl<std::index_sequence<Is...>>
{
    template<class Tuple, class F>
    static constexpr void apply(Tuple&& t, F&& f){
        ((void)std::invoke(std::forward<F>(f), alloy::get<Is>(std::forward<Tuple>(t))), ...);
    }
};

} // detail

template<class... Tuples>
using tuple_cat_t = typename detail::tuple_cat_result<Tuples...>::type;

template<class Tuple, std::size_t... Sizes>
using tuple_split_t = typename detail::tuple_split_result<Tuple, Sizes...>::type;

template<class Tuple>
using tuple_ref_t = typename detail::tuple_ref_result<Tuple>::type;

template<TupleLike... Tuples>
[[nodiscard]] constexpr tuple_cat_t<Tuples...> tuple_cat(Tuples&&... tuples) noexcept(detail::tuple_cat_impl<Tuples...>::nothrow)
{
    return detail::tuple_cat_impl<Tuples...>::apply(std::forward<Tuples>(tuples)...);
}

template<std::size_t... Sizes, TupleLike Tuple>
[[nodiscard]] constexpr tuple_split_t<Tuple, Sizes...> tuple_split(Tuple&& t) noexcept(detail::tuple_split_impl<Tuple, Sizes...>::nothrow)
{
    static_assert((0 + ... + Sizes) == tuple_size_v<std::remove_cvref_t<Tuple>>);
    return detail::tuple_split_impl<Tuple, Sizes...>::apply(std::forward<Tuple>(t));
}

template<TupleLike From, TupleLike To>
constexpr void tuple_assign(From&& from, To&& to) noexcept(detail::tuple_assign_impl<From, To>::nothrow)
{
    static_assert(tuple_size_v<std::remove_cvref_t<From>> == tuple_size_v<std::remove_cvref_t<To>>);
    detail::tuple_assign_impl<From, To>::apply(std::forward<From>(from), std::forward<To>(to));
}

template<TupleLike Tuple>
[[nodiscard]] constexpr tuple_ref_t<Tuple> tuple_ref(Tuple& t) noexcept(std::is_nothrow_constructible_v<tuple_ref_t<Tuple>, Tuple&>)
{
    return tuple_ref_t<Tuple>(t);
}

template<TupleLike Tuple, class F>
constexpr void for_each(Tuple&& t, F&& f)
{
    return detail::for_each_impl<std::make_index_sequence<tuple_size_v<std::remove_cvref_t<Tuple>>>>::apply(std::forward<Tuple>(t), std::forward<F>(f));
}

} // iris::alloy

#endif

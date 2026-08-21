#ifndef IRIS_ZZ_UTILITY_HPP
#define IRIS_ZZ_UTILITY_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <utility> // IWYU pragma: export

namespace iris {

namespace detail::adl_get {

using std::get;

template<std::size_t I, class T>
concept gettable_impl = requires(T&& t) {
    get<I>(std::forward<T>(t));
};

} // detail::adl_get

template<std::size_t I, class T>
concept gettable = detail::adl_get::gettable_impl<I, T>;

} // iris

#endif

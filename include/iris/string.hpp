#ifndef IRIS_ZZ_STRING_HPP
#define IRIS_ZZ_STRING_HPP

// SPDX-License-Identifier: MIT

#include <string>
#include <string_view>

namespace iris {

template<class T>
concept StringLike = requires(T t) {
    std::basic_string_view{t};
};

template<class T>
concept NotStringLike = !StringLike<T>;

} // iris

#endif

#ifndef IRIS_IO_FWD_HPP
#define IRIS_IO_FWD_HPP

#include <concepts>
#include <iosfwd>
#include <type_traits>

namespace iris::req {

namespace detail {

namespace ADL_ostreamable_poison_pill {

template<class T>
void operator<<(std::ostream& os, T const&) = delete;

template<class T>
struct ADL_ostreamable_impl : std::false_type {};

template<class T>
    requires requires(std::ostream& os, T const& val) {
        { os << val } -> std::same_as<std::ostream&>;
    }
struct ADL_ostreamable_impl<T> : std::true_type {};

} // ADL_ostreamable_poison_pill

} // detail

template<class T>
struct ADL_ostreamable : detail::ADL_ostreamable_poison_pill::ADL_ostreamable_impl<T> {};

template<class T>
constexpr bool ADL_ostreamable_v = ADL_ostreamable<T>::value;

}  // iris::req

#endif

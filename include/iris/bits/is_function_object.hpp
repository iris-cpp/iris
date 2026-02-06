#ifndef IRIS_BITS_IS_FUNCTION_OBJECT
#define IRIS_BITS_IS_FUNCTION_OBJECT

#include <type_traits>

namespace iris {

// https://eel.is/c++draft/function.objects.general
template<class F, class... Args>
struct is_function_object : std::false_type {};

template<class F, class... Args>
constexpr bool is_function_object_v = is_function_object<F, Args...>::value;

template<class F, class... Args>
    requires
        (std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>> && requires(F f) { (*f)(std::declval<Args>()...); }) ||
        (std::is_class_v<F> && requires(F f) { f(std::declval<Args>()...); })
struct is_function_object<F, Args...> : std::true_type {};

}  // iris

#endif

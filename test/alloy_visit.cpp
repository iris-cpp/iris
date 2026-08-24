// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/utility.hpp>

#include <tuple>
#include <utility>

namespace alloy = iris::alloy;

namespace detail {

template<class Seq>
struct make_iota_tuple_impl;

template<class T, T... Is>
struct make_iota_tuple_impl<std::integer_sequence<T, Is...>>
{
    [[nodiscard]] static constexpr auto apply() noexcept
    {
        return std::tuple{Is...};
    }
};

} // detail

template<auto N>
[[nodiscard]] consteval auto make_iota_tuple() noexcept
{
    return detail::make_iota_tuple_impl<std::make_integer_sequence<decltype(N), N>>::apply();
}

TEST_CASE("alloy: visit")
{
    {
        constexpr auto f = [](auto&& elem) noexcept { return elem; };

        #define IRIS_TEST_VISIT(N) \
            STATIC_CHECK(alloy::visit_at(0, f, make_iota_tuple<N>()) == 0); \
            STATIC_CHECK(noexcept(alloy::visit_at(0, f, make_iota_tuple<N>()))); \
            STATIC_CHECK(alloy::visit_at(N - 1, f, make_iota_tuple<N>()) == N - 1); \
            STATIC_CHECK(noexcept(alloy::visit_at(N - 1, f, make_iota_tuple<N>())))

        // Strategy = 0
        IRIS_TEST_VISIT(4);

        // Strategy = 1
        IRIS_TEST_VISIT(16);

        // Strategy = 2
        IRIS_TEST_VISIT(64);

        // Strategy = -1 (table-based dispatch)
        IRIS_TEST_VISIT(65);

        #undef IRIS_TEST_VISIT
    }

    // Check "all noexcept" is working
    {
        constexpr auto conditionally_throwing_f = []<std::size_t I>(std::integral_constant<std::size_t, I>, auto&& elem)
            noexcept(I == 1) // arbitrary
        {
            return elem;
        };

        #define IRIS_TEST_VISIT(N) \
            STATIC_CHECK(alloy::visit_at(0, conditionally_throwing_f, make_iota_tuple<N>()) == 0); \
            STATIC_CHECK(!noexcept(alloy::visit_at(0, conditionally_throwing_f, make_iota_tuple<N>()))); \
            STATIC_CHECK(alloy::visit_at(N - 1, conditionally_throwing_f, make_iota_tuple<N>()) == N - 1); \
            STATIC_CHECK(!noexcept(alloy::visit_at(N - 1, conditionally_throwing_f, make_iota_tuple<N>())))

        // Strategy = 0
        IRIS_TEST_VISIT(4);

        // Strategy = 1
        IRIS_TEST_VISIT(16);

        // Strategy = 2
        IRIS_TEST_VISIT(64);

        // Strategy = -1 (table-based dispatch)
        IRIS_TEST_VISIT(65);

        #undef IRIS_TEST_VISIT
    }
}

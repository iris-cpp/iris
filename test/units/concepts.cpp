// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/units/concepts.hpp>
#include <iris/units/quantity.hpp>

#include <concepts>

template<class T>
struct Length : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};

TEST_CASE("concepts", "[units]")
{
    STATIC_CHECK(iris::units::ordered_linear<float>);
    STATIC_CHECK(iris::units::nothrow_ordered_linear<float>);

    STATIC_CHECK(iris::units::ordered_linear<double>);
    STATIC_CHECK(iris::units::nothrow_ordered_linear<double>);

    STATIC_CHECK(!iris::units::ordered_linear<int>);

    STATIC_CHECK(iris::units::ordered_linear<Length<float>>);
    STATIC_CHECK(iris::units::nothrow_ordered_linear<Length<float>>);

    STATIC_CHECK(iris::units::ordered_linear<Length<double>>);
    STATIC_CHECK(iris::units::nothrow_ordered_linear<Length<double>>);

    STATIC_CHECK(!iris::units::ordered_linear<Length<int>>);
}

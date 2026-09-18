// SPDX-License-Identifier: MIT

#include "iris_test.hpp"

#include <iris/units/min_pref_max.hpp>
#include <iris/units/quantity.hpp>

#include <format>
#include <functional>
#include <concepts>
#include <type_traits>
#include <stdexcept>
#include <limits>

using iris::units::min_pref_max;

template<class L, class R>
concept compound_addable = requires(L& lhs, R const& rhs) { lhs += rhs; };

template<class L, class R>
concept compound_subtractable = requires(L& lhs, R const& rhs) { lhs -= rhs; };

template<class L, class R>
concept subtractable = requires(L const& lhs, R const& rhs) { lhs - rhs; };

template<std::floating_point T>
struct RelativeLength final : iris::units::quantity<T>
{
    using iris::units::quantity<T>::quantity;
};
IRIS_QUANTITY_DEDUCTION_GUIDE(RelativeLength);

template<iris::units::ordered_linear T>
struct MinPrefMax final : min_pref_max<T>
{
    using min_pref_max<T>::min_pref_max;
};
IRIS_MIN_PREF_MAX_DEDUCTION_GUIDE(MinPrefMax);

using MPM_d = MinPrefMax<double>;
using MPM_f = MinPrefMax<float>;
using MPM_rel_d = MinPrefMax<RelativeLength<double>>;
using MPM_rel_f = MinPrefMax<RelativeLength<float>>;

TEST_CASE("type traits", "[units][min_pref_max]")
{
    STATIC_CHECK(std::is_trivially_copyable_v<MPM_d>);
    STATIC_CHECK(std::is_nothrow_copy_constructible_v<MPM_d>);
    STATIC_CHECK(std::is_nothrow_move_assignable_v<MPM_d>);
    STATIC_CHECK(std::is_nothrow_default_constructible_v<MPM_d>);
    STATIC_CHECK(std::is_standard_layout_v<MPM_d>);
    STATIC_CHECK(!std::is_aggregate_v<MPM_d>);
    STATIC_CHECK(std::is_trivially_copyable_v<MPM_rel_d>);

    STATIC_CHECK(iris::units::min_pref_max_class<MPM_d>);
    STATIC_CHECK(iris::units::min_pref_max_class<MPM_rel_d>);
    STATIC_CHECK(!iris::units::min_pref_max_class<min_pref_max<double>>);
    STATIC_CHECK(!iris::units::min_pref_max_class<RelativeLength<double>>);
    STATIC_CHECK(iris::units::min_pref_max_family<MPM_d, MPM_f>);
    STATIC_CHECK(iris::units::min_pref_max_family<MPM_rel_d, MPM_rel_f>);
    STATIC_CHECK(!iris::units::min_pref_max_family<MPM_d, MPM_rel_d>);

    STATIC_CHECK(std::same_as<std::common_type_t<MPM_f, MPM_d>, MPM_d>);
    STATIC_CHECK(std::same_as<std::common_type_t<MPM_rel_f, MPM_rel_d>, MPM_rel_d>);
    STATIC_CHECK(std::same_as<decltype(MinPrefMax{0, 0.5f, 1.0}), MPM_d>);
}

TEST_CASE("construction", "[units][min_pref_max]")
{
    constexpr MPM_d zero{};
    STATIC_CHECK(zero.is_all_zero());
    STATIC_CHECK(zero.min() == 0.0);
    STATIC_CHECK(zero.pref() == 0.0);
    STATIC_CHECK(zero.max() == 0.0);

    constexpr MPM_d m{0.0, 0.5, 1.0};
    STATIC_CHECK(m.min() == 0.0);
    STATIC_CHECK(m.pref() == 0.5);
    STATIC_CHECK(m.max() == 1.0);
    STATIC_CHECK(m.shrink() == 0.5);
    STATIC_CHECK(m.stretch() == 0.5);
    STATIC_CHECK(!m.is_all_zero());

    constexpr MPM_d hetero{0, 0.5f, 1};
    STATIC_CHECK(hetero == m);
    constexpr MPM_d degenerate{1.0, 1.0, 1.0};
    STATIC_CHECK(degenerate.shrink() == 0.0);
    STATIC_CHECK(degenerate.stretch() == 0.0);

    constexpr MPM_rel_d rel{RelativeLength{0.0}, RelativeLength{0.5}, RelativeLength{1.0}};
    STATIC_CHECK(rel.pref() == RelativeLength{0.5});
    STATIC_CHECK(rel.stretch() == RelativeLength{0.5});

    REQUIRE_THROWS_AS((MPM_d{0.5, 0.0, 1.0}), std::invalid_argument);
    REQUIRE_THROWS_AS((MPM_d{0.0, 1.0, 0.5}), std::invalid_argument);
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    REQUIRE_THROWS_AS((MPM_d{nan, 0.5, 1.0}), std::invalid_argument);
    REQUIRE_THROWS_AS((MPM_d{0.0, nan, 1.0}), std::invalid_argument);
    REQUIRE_THROWS_AS((MPM_d{0.0, 0.5, nan}), std::invalid_argument);
}

TEST_CASE("conversion", "[units][min_pref_max]")
{
    STATIC_CHECK(std::is_convertible_v<MPM_f, MPM_d>);
    STATIC_CHECK(!std::is_convertible_v<MPM_d, MPM_f>);
    STATIC_CHECK(std::is_constructible_v<MPM_f, MPM_d>);
    STATIC_CHECK(std::is_convertible_v<MPM_rel_f, MPM_rel_d>);
    STATIC_CHECK(!std::is_convertible_v<MPM_rel_d, MPM_rel_f>);
    STATIC_CHECK(!std::is_constructible_v<MPM_rel_d, MPM_d>);

    constexpr MPM_f f{0.0f, 0.5f, 1.0f};
    constexpr MPM_d d = f;
    STATIC_CHECK(d.pref() == 0.5);
    constexpr MPM_f back{d};
    STATIC_CHECK(back == f);
}

TEST_CASE("comparison", "[units][min_pref_max]")
{
    constexpr MPM_d a{0.0, 0.5, 1.0};
    constexpr MPM_d b{0.0, 0.5, 1.0};
    constexpr MPM_d c{0.0, 0.25, 1.0};
    constexpr MPM_f af{0.0f, 0.5f, 1.0f};

    STATIC_CHECK(a == b);
    STATIC_CHECK(a != c);
    STATIC_CHECK(a == af);
    STATIC_CHECK(af == a);
    CHECK(std::hash<MPM_d>{}(a) == std::hash<MPM_d>{}(b));
}

TEST_CASE("addition", "[units][min_pref_max]")
{
    constexpr MPM_d a{0.0, 0.5, 1.0};
    constexpr MPM_d b{0.25, 0.5, 2.0};
    constexpr MPM_f bf{0.25f, 0.5f, 2.0f};

    STATIC_CHECK(a + b == MPM_d{0.25, 1.0, 3.0});
    STATIC_CHECK(std::same_as<decltype(a + bf), MPM_d>);
    STATIC_CHECK(std::same_as<decltype(bf + a), MPM_d>);
    STATIC_CHECK(a + bf == MPM_d{0.25, 1.0, 3.0});

    constexpr auto compound = [] {
        MPM_d m{0.0, 0.5, 1.0};
        m += MPM_f{0.25f, 0.5f, 2.0f};
        return m;
    }();
    STATIC_CHECK(compound == MPM_d{0.25, 1.0, 3.0});

    STATIC_CHECK(compound_addable<MPM_d, MPM_f>);
    STATIC_CHECK(!compound_addable<MPM_f, MPM_d>);

    STATIC_CHECK(!subtractable<MPM_d, MPM_d>);
    STATIC_CHECK(!compound_subtractable<MPM_d, MPM_d>);
}

TEST_CASE("shift", "[units][min_pref_max]")
{
    constexpr MPM_d m{0.0, 0.5, 1.0};

    STATIC_CHECK(m + 1.0 == MPM_d{1.0, 1.5, 2.0});
    STATIC_CHECK(1.0 + m == MPM_d{1.0, 1.5, 2.0});
    STATIC_CHECK(m - 1.0 == MPM_d{-1.0, -0.5, 0.0});
    STATIC_CHECK(std::same_as<decltype(m + 1.0), MPM_d>);
    STATIC_CHECK(!subtractable<double, MPM_d>); // `1.0 - m`

    constexpr auto shifted = [] {
        MPM_d x{0.0, 0.5, 1.0};
        x += 1.0;
        x -= 0.5;
        return x;
    }();
    STATIC_CHECK(shifted == MPM_d{0.5, 1.0, 1.5});
    STATIC_CHECK(shifted.shrink() == 0.5);
    STATIC_CHECK(shifted.stretch() == 0.5);

    constexpr MPM_rel_d rel{RelativeLength{0.0}, RelativeLength{0.5}, RelativeLength{1.0}};
    STATIC_CHECK((rel + RelativeLength{1.0}).min() == RelativeLength{1.0});
}

TEST_CASE("setters", "[units][min_pref_max]")
{
    constexpr auto set = [](auto setter) {
        MPM_d m{0.0, 0.5, 1.0};
        setter(m);
        return m;
    };

    STATIC_CHECK(set([](MPM_d& m) { m.set_min(-1.0); }) == MPM_d{-1.0, 0.5, 1.0});
    STATIC_CHECK(set([](MPM_d& m) { m.set_min(0.7); }) == MPM_d{0.7, 0.7, 1.0});
    STATIC_CHECK(set([](MPM_d& m) { m.set_min(2.0); }) == MPM_d{2.0, 2.0, 2.0});

    STATIC_CHECK(set([](MPM_d& m) { m.set_max(3.0); }) == MPM_d{0.0, 0.5, 3.0});
    STATIC_CHECK(set([](MPM_d& m) { m.set_max(0.25); }) == MPM_d{0.0, 0.25, 0.25});
    STATIC_CHECK(set([](MPM_d& m) { m.set_max(-1.0); }) == MPM_d{-1.0, -1.0, -1.0});

    STATIC_CHECK(set([](MPM_d& m) { m.set_pref(0.75); }) == MPM_d{0.0, 0.75, 1.0});
    STATIC_CHECK(set([](MPM_d& m) { m.set_pref(-1.0); }) == MPM_d{-1.0, -1.0, 1.0});
    STATIC_CHECK(set([](MPM_d& m) { m.set_pref(2.0); }) == MPM_d{0.0, 2.0, 2.0});
}

TEST_CASE("format", "[units][min_pref_max]")
{
    MPM_d const m{0.0, 0.5, 1.0};
    CHECK(std::format("{}", m) == "0/0.5/1");
    CHECK(std::format("{:.2f}", m) == "0.00/0.50/1.00");

    MPM_rel_d const rel{RelativeLength{0.0}, RelativeLength{0.5}, RelativeLength{1.0}};
    CHECK(std::format("{:.1f}", rel) == "0.0/0.5/1.0");
}

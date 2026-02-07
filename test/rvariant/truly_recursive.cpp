// SPDX-License-Identifier: MIT

#include "iris_rvariant_test.hpp"

#include <iris/rvariant.hpp>

#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace unit_test {

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4244) // implicit numeric conversion
#endif

TEST_CASE("truly recursive", "[wrapper][recursive]")
{
    // Although this pattern is perfectly valid in type level,
    // it inherently allocates infinite amount of memory.
    // We just need to make sure it has the correct type traits.

    // std::variant
    {
        struct SubExpr;
        using Expr = std::variant<iris::recursive_wrapper<SubExpr>>;
        struct SubExpr { Expr expr; };

        STATIC_REQUIRE( std::is_constructible_v<Expr, Expr>);
        STATIC_REQUIRE( std::is_constructible_v<Expr, SubExpr>);
        STATIC_REQUIRE(!std::is_constructible_v<Expr, int>); // this should result in infinite recursion if our implementation is wrong

        STATIC_REQUIRE( std::is_constructible_v<SubExpr, SubExpr>);
        STATIC_REQUIRE( std::is_constructible_v<SubExpr, Expr>);
        STATIC_REQUIRE(!std::is_constructible_v<SubExpr, int>);

        //Expr expr; // infinite malloc
    }

    // rvariant
    {
        struct SubExpr;
        using Expr = iris::rvariant<iris::recursive_wrapper<SubExpr>>;
        struct SubExpr { Expr expr; };

        STATIC_REQUIRE( std::is_constructible_v<Expr, Expr>);
        STATIC_REQUIRE( std::is_constructible_v<Expr, SubExpr>);
        STATIC_REQUIRE(!std::is_constructible_v<Expr, int>); // this should result in infinite recursion if our implementation is wrong

        STATIC_REQUIRE( std::is_constructible_v<SubExpr, SubExpr>);
        STATIC_REQUIRE( std::is_constructible_v<SubExpr, Expr>);
        STATIC_REQUIRE(!std::is_constructible_v<SubExpr, int>);

        //Expr expr; // infinite malloc
    }

    // In contrast to above, this pattern has a safe *fallback* of int
    {
        // Sanity check
        {
            using V = std::variant<int>;
            STATIC_REQUIRE( std::is_constructible_v<V, V>);
            STATIC_REQUIRE( std::is_constructible_v<V, int>);
            STATIC_REQUIRE(!std::is_constructible_v<V, double>);
        }
        {
            using V = std::variant<iris::recursive_wrapper<int>>;
            STATIC_REQUIRE( std::is_constructible_v<V, V>);
            STATIC_REQUIRE( std::is_constructible_v<V, int>);
            STATIC_REQUIRE( std::is_constructible_v<V, double>);  // !!true!!
        }
        {
            using V = iris::rvariant<iris::recursive_wrapper<int>>;
            STATIC_REQUIRE( std::is_constructible_v<V, V>);
            STATIC_REQUIRE( std::is_constructible_v<V, int>);
            STATIC_REQUIRE( std::is_constructible_v<V, double>);  // !!true!!
        }

        // Sanity check
        {
            struct SubExpr;
            using Expr = std::variant<int, iris::recursive_wrapper<SubExpr>>;
            struct SubExpr { Expr expr; };

            STATIC_REQUIRE( std::is_constructible_v<Expr, int>);
            STATIC_REQUIRE(!std::is_constructible_v<Expr, double>); // false because equally viable

            STATIC_REQUIRE( std::is_constructible_v<SubExpr, int>);
            STATIC_REQUIRE(!std::is_constructible_v<SubExpr, double>); // false because equally viable
        }

        struct SubExpr;
        using Expr = iris::rvariant<int, iris::recursive_wrapper<SubExpr>>;
        struct SubExpr { Expr expr; };

        STATIC_REQUIRE( std::is_constructible_v<Expr, int>);
        STATIC_REQUIRE(!std::is_constructible_v<Expr, double>); // false because equally viable

        STATIC_REQUIRE( std::is_constructible_v<SubExpr, int>);
        STATIC_REQUIRE(!std::is_constructible_v<SubExpr, double>); // false because equally viable

        REQUIRE_NOTHROW(Expr{});
        REQUIRE_NOTHROW(SubExpr{});
        REQUIRE_NOTHROW(Expr{42});
        REQUIRE_NOTHROW(SubExpr{42});

        STATIC_REQUIRE(std::is_constructible_v<Expr, std::in_place_index_t<0>, int>);
        CHECK_NOTHROW(Expr{std::in_place_index<0>, 42});

        STATIC_REQUIRE(std::is_constructible_v<Expr, std::in_place_index_t<1>, SubExpr>);
        CHECK_NOTHROW(Expr{std::in_place_index<1>, SubExpr{42}});

        STATIC_REQUIRE(std::is_constructible_v<Expr, int>);
        CHECK_NOTHROW(Expr{42});

        constexpr auto vis = iris::overloaded{
            [](int const&)     { return 0; },
            [](SubExpr const&) { return 1; },
        };
        STATIC_CHECK(Expr{42}.visit(vis) == 0);
        STATIC_CHECK(Expr{SubExpr{Expr{42}}}.visit(vis) == 1);
    }
    {
        struct BinaryExpr;
        using Expr = iris::rvariant<int, double, iris::recursive_wrapper<BinaryExpr>>;
        enum class Op;
        struct BinaryExpr { Expr lhs, rhs; Op op{}; };

        Expr expr{BinaryExpr{Expr{42}, Expr{3.14}}};
        expr.visit(iris::overloaded{
            [](int const&) { /* ... */ },
            [](double const&) { /* ... */ },
            [](BinaryExpr const&) { /* ... */ },
        });
    }
}

namespace {

struct NodeArray;

using Node = iris::rvariant<int, iris::recursive_wrapper<NodeArray>>;

struct NodeArray : std::vector<Node>
{
    using std::vector<Node>::vector;
};

bool operator==(NodeArray const& a, NodeArray const& b)
{
    return static_cast<std::vector<Node> const&>(a) == static_cast<std::vector<Node> const&>(b);
}

std::strong_ordering operator<=>(NodeArray const& a, NodeArray const& b)
{
    return static_cast<std::vector<Node> const&>(a) <=> static_cast<std::vector<Node> const&>(b);
}

} // anonymous

TEST_CASE("recursive vector", "[wrapper][recursive]")
{
    STATIC_CHECK(std::three_way_comparable<NodeArray>);

    // ReSharper disable CppIdenticalOperandsInBinaryExpression
    // NOLINTBEGIN(misc-redundant-expression)
    {
        NodeArray node_arr;
        (void)(node_arr == node_arr);
        (void)(node_arr <=> node_arr);
        (void)(node_arr < node_arr);
    }
    {
        iris::recursive_wrapper<NodeArray> node_arr_rw;
        (void)(node_arr_rw == node_arr_rw);
        (void)(node_arr_rw <=> node_arr_rw);
        (void)(node_arr_rw < node_arr_rw);
    }
    // NOLINTEND(misc-redundant-expression)
    // ReSharper restore CppIdenticalOperandsInBinaryExpression
}

#ifdef _MSC_VER
# pragma warning(pop)
#endif

} // unit_test

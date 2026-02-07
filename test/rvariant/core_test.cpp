// SPDX-License-Identifier: MIT

#include <iris/type_traits.hpp>
#include <iris/requirements.hpp>
#include <iris/compare.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <utility>
#include <type_traits>

namespace unit_test {

TEST_CASE("pack_indexing")
{
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, int>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, int, float>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<1, int, float>, float>);

    // make sure non-object and non-referenceable type is working
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, void>, void>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, void, int>, void>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<1, void, int>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, int, void>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<1, int, void>, void>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<0, void, void>, void>);
    STATIC_REQUIRE(std::is_same_v<iris::pack_indexing_t<1, void, void>, void>);

    STATIC_REQUIRE(std::is_same_v<iris::at_c_t<0, iris::type_list<int>>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::at_c_t<0, iris::type_list<int, float>>, int>);
    STATIC_REQUIRE(std::is_same_v<iris::at_c_t<1, iris::type_list<int, float>>, float>);
}

TEST_CASE("exactly_once")
{
    STATIC_REQUIRE(iris::exactly_once_v<int, iris::type_list<int, float>>);
    STATIC_REQUIRE_FALSE(iris::exactly_once_v<int, iris::type_list<int, int>>);
}

TEST_CASE("is_in")
{
    STATIC_REQUIRE(iris::is_in_v<int, int, float>);
    STATIC_REQUIRE_FALSE(iris::is_in_v<int, float>);
}

TEST_CASE("find_index")
{
    STATIC_REQUIRE(iris::find_index_v<int,    iris::type_list<int, float, double>> == 0);
    STATIC_REQUIRE(iris::find_index_v<float,  iris::type_list<int, float, double>> == 1);
    STATIC_REQUIRE(iris::find_index_v<double, iris::type_list<int, float, double>> == 2);
    STATIC_REQUIRE(iris::find_index_v<int,    iris::type_list<float, double>> == iris::find_npos);

    STATIC_REQUIRE(iris::find_index_v<int, iris::type_list<int, int, double>> == 0);
}

TEST_CASE("Cpp17EqualityComparable")
{
    STATIC_REQUIRE(iris::req::Cpp17EqualityComparable<int>);
    {
        struct S
        {
            bool operator==(S const&) const { return true; }
        };
        STATIC_REQUIRE(iris::req::Cpp17EqualityComparable<S>);
    }
    {
        struct S
        {
            bool operator==(S const&) const = delete;
        };
        STATIC_REQUIRE(!iris::req::Cpp17EqualityComparable<S>);
    }
}

TEST_CASE("Cpp17LessThanComparable")
{
    STATIC_REQUIRE(iris::req::Cpp17LessThanComparable<int>);
    {
        struct S
        {
            bool operator<(S const&) const { return false; }
        };
        STATIC_REQUIRE(iris::req::Cpp17LessThanComparable<S>);
    }
    {
        struct S
        {
            bool operator<(S const&) const = delete;
        };
        STATIC_REQUIRE(!iris::req::Cpp17LessThanComparable<S>);
    }
}

TEST_CASE("Cpp17DefaultConstructible")
{
    STATIC_REQUIRE(iris::req::Cpp17DefaultConstructible<int>);
    {
        using T = int const;
        STATIC_REQUIRE(std::is_default_constructible_v<T>);
        // ReSharper disable once CppStaticAssertFailure
        STATIC_REQUIRE(!std::default_initializable<T>);
        // ReSharper disable once CppStaticAssertFailure
        STATIC_REQUIRE(!iris::req::Cpp17DefaultConstructible<T>);
    }
    {
        using T = int const[10];
        STATIC_REQUIRE(std::is_default_constructible_v<T>);
        STATIC_REQUIRE(!iris::req::Cpp17DefaultConstructible<T>);
    }
    {
        struct S
        {
            S() = delete;
        };
        STATIC_REQUIRE(!std::is_default_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17DefaultConstructible<S>);
    }
}

TEST_CASE("Cpp17MoveConstructible")
{
    STATIC_REQUIRE(iris::req::Cpp17MoveConstructible<int>);
    {
        using T = int const;
        STATIC_REQUIRE(std::is_move_constructible_v<T>);
        STATIC_REQUIRE(iris::req::Cpp17MoveConstructible<T>);
    }
    {
        using T = int const[10];
        STATIC_REQUIRE(!std::is_move_constructible_v<T>);
        STATIC_REQUIRE(!std::move_constructible<T>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveConstructible<T>);
    }
    {
        struct S
        {
            S(S&&) = delete;
        };
        STATIC_REQUIRE(!std::is_move_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveConstructible<S>);
    }
    {
        struct S
        {
            explicit S(S&&) noexcept {}
        };
        STATIC_REQUIRE(std::is_move_constructible_v<S>);
        STATIC_REQUIRE(!std::move_constructible<S>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveConstructible<S>);
    }
}

TEST_CASE("Cpp17CopyConstructible")
{
    STATIC_REQUIRE(iris::req::Cpp17CopyConstructible<int>);
    {
        using T = int const;
        STATIC_REQUIRE(std::is_copy_constructible_v<T>);
        STATIC_REQUIRE(iris::req::Cpp17CopyConstructible<T>);
    }
    {
        using T = int const[10];
        STATIC_REQUIRE(!std::is_copy_constructible_v<T>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<T>);
    }
    {
        struct S
        {
            int const t[10];  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        };
        STATIC_REQUIRE(std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            S(S&&) = default;
            S(S const&) = delete;
        };
        STATIC_REQUIRE(!std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            S(S&&) = delete;
            S(S const&) = default;
        };
        STATIC_REQUIRE(std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            explicit S(S const&) {}  // NOLINT(modernize-use-equals-default)
        };
        STATIC_REQUIRE(std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            S(S&) = delete;
            S(S const&) {}  // NOLINT(modernize-use-equals-default)
            S(S const&&) noexcept {}
        };
        STATIC_REQUIRE(std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            S(S&) {}  // NOLINT(modernize-use-equals-default)
            S(S const&) = delete;
            S(S const&&) noexcept {}
        };
        STATIC_REQUIRE(!std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
    {
        struct S
        {
            S(S&) {}  // NOLINT(modernize-use-equals-default)
            S(S const&) {}  // NOLINT(modernize-use-equals-default)
            S(S const&&) = delete;
        };
        STATIC_REQUIRE(std::is_copy_constructible_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyConstructible<S>);
    }
}

TEST_CASE("Cpp17MoveAssignable")
{
    STATIC_REQUIRE(iris::req::Cpp17MoveAssignable<int>);
    {
        using T = int const;
        STATIC_REQUIRE(!std::is_move_assignable_v<T>);
        STATIC_REQUIRE(!std::assignable_from<T&, T&&>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveAssignable<T>);
    }
    {
        using T = int const [10];
        STATIC_REQUIRE(!std::is_move_assignable_v<T>);
        STATIC_REQUIRE(!std::assignable_from<T&, T&&>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveAssignable<T>);
    }
    {
        struct S
        {
            S& operator=(S&&) = delete;
        };
        STATIC_REQUIRE(!std::is_move_assignable_v<S>);
        STATIC_REQUIRE(!std::assignable_from<S&, S&&>);
        STATIC_REQUIRE(!iris::req::Cpp17MoveAssignable<S>);
    }
}

TEST_CASE("Cpp17CopyAssignable")
{
    STATIC_REQUIRE(iris::req::Cpp17CopyAssignable<int>);
    {
        using T = int const;
        STATIC_REQUIRE(!std::is_copy_assignable_v<T>);
        STATIC_REQUIRE(!std::assignable_from<T&, T const&>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<T>);
    }
    {
        using T = int const [10];
        STATIC_REQUIRE(!std::is_copy_assignable_v<T>);
        STATIC_REQUIRE(!std::assignable_from<T&, T const&>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<T>);
    }
    {
        struct S
        {
            S& operator=(S const&) = delete;
        };
        STATIC_REQUIRE(!std::is_copy_assignable_v<S>);
        STATIC_REQUIRE(!std::assignable_from<S&, S const&>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<S>);
    }
    {
        struct S
        {
            S& operator=(S&) = delete;
            S& operator=(S const&) { return *this; }  // NOLINT(modernize-use-equals-default)
            S& operator=(S const&&) noexcept { return *this; }  // NOLINT(misc-unconventional-assign-operator)
        };
        STATIC_REQUIRE(std::is_copy_assignable_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<S>);
    }
    {
        struct S
        {
            S& operator=(S&) { return *this; }  // NOLINT(modernize-use-equals-default, misc-unconventional-assign-operator)
            S& operator=(S const&) = delete;
            S& operator=(S const&&) noexcept { return *this; }  // NOLINT(misc-unconventional-assign-operator)
        };
        STATIC_REQUIRE(!std::is_copy_assignable_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<S>);
    }
    {
        struct S
        {
            S& operator=(S&) { return *this; }  // NOLINT(misc-unconventional-assign-operator, modernize-use-equals-default)
            S& operator=(S const&) { return *this; }  // NOLINT(modernize-use-equals-default)
            S& operator=(S const&&) = delete;
        };
        STATIC_REQUIRE(std::is_copy_assignable_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<S>);
    }
    {
        struct S
        {
            S& operator=(S const&) { return *this; }  // NOLINT(modernize-use-equals-default)
            S& operator=(S&&) = delete;
        };
        STATIC_REQUIRE(std::is_copy_assignable_v<S>);
        STATIC_REQUIRE(!iris::req::Cpp17CopyAssignable<S>);
    }
}

TEST_CASE("Cpp17Destructible")
{
    STATIC_REQUIRE(iris::req::Cpp17Destructible<int>);
    {
        struct S
        {
            ~S() {}  // NOLINT(modernize-use-equals-default)
        };
        STATIC_REQUIRE(std::is_nothrow_destructible_v<S>);
        STATIC_REQUIRE(iris::req::Cpp17Destructible<S>);
    }
    {
        struct S
        {
            ~S() noexcept(true) {}  // NOLINT(modernize-use-equals-default)
        };
        STATIC_REQUIRE(std::is_nothrow_destructible_v<S>);
        STATIC_REQUIRE(iris::req::Cpp17Destructible<S>);
    }
    {
        struct S
        {
            ~S() noexcept(false) {}  // NOLINT(modernize-use-equals-default)
        };
        STATIC_REQUIRE(std::is_destructible_v<S>);
        STATIC_REQUIRE(!std::is_nothrow_destructible_v<S>);
        STATIC_REQUIRE(iris::req::Cpp17Destructible<S>); // noexcept(false) should still satisfy this, as long as the exception is not propagated

        // Cpp17Destructible is NOT the same as std::destructible
        STATIC_REQUIRE(!std::destructible<S>); // noexcept(false) is not satisfied
    }
    {
        // https://eel.is/c++draft/utility.arg.requirements#tab:cpp17.destructible-row-3-column-1-note-3
        // > Array types and non-object types are not Cpp17Destructible.

        STATIC_REQUIRE(!iris::req::Cpp17Destructible<int[]>);
        STATIC_REQUIRE(!iris::req::Cpp17Destructible<int[1]>);
        STATIC_REQUIRE(!iris::req::Cpp17Destructible<void>);
        STATIC_REQUIRE(!iris::req::Cpp17Destructible<int&>);

        // Cpp17Destructible is NOT the same as std::destructible
        STATIC_REQUIRE(!std::destructible<int[]>);
        STATIC_REQUIRE( std::destructible<int[1]>);
        STATIC_REQUIRE(!std::destructible<void>);
        STATIC_REQUIRE( std::destructible<int&>);
    }
}

namespace {

struct NonMovable
{
    NonMovable() {}  // NOLINT(modernize-use-equals-default)
    NonMovable(NonMovable const&) = delete;
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable const&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
    ~NonMovable() {}  // NOLINT(modernize-use-equals-default)
};

struct S
{
    [[maybe_unused]] NonMovable nm;
};

struct Ss
{
    [[maybe_unused]] NonMovable nm;
    friend void swap(Ss&, Ss&) noexcept {}
};

} // anonymous

TEST_CASE("Cpp17Swappable")
{
    STATIC_REQUIRE(iris::req::Cpp17Swappable<int>);
    {
        STATIC_REQUIRE(!std::is_swappable_v<S&>);
        STATIC_REQUIRE(!iris::req::Cpp17Swappable<S>);
    }
    {
        STATIC_REQUIRE(std::is_swappable_v<Ss&>);
        STATIC_REQUIRE(iris::req::Cpp17Swappable<Ss>);

        Ss a, b;
        using std::swap;
        swap(a, b);
    }
}

TEST_CASE("synth_three_way")
{
    struct NoThreeWay
    {
        int value = 0;

        constexpr bool operator<(NoThreeWay const& other) const noexcept
        {
            return value < other.value;
        }
    };

    static_assert(!std::three_way_comparable<NoThreeWay>);
    static_assert(!std::three_way_comparable_with<NoThreeWay, NoThreeWay>);

    STATIC_CHECK(std::same_as<iris::cmp::synth_three_way_result<NoThreeWay>, std::weak_ordering>);
    STATIC_CHECK(noexcept(iris::cmp::synth_three_way{}(NoThreeWay{0}, NoThreeWay{1})));
    STATIC_CHECK(iris::cmp::synth_three_way{}(NoThreeWay{0}, NoThreeWay{1}) == std::weak_ordering::less);
}

} // unit_test

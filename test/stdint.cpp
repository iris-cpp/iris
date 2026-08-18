#include "iris_test.hpp"

#include <iris/stdint.hpp>

#include <concepts>

#include <cstdint>
#include <cstddef> // std::byte

enum non_scoped_int16_t : std::int16_t {};
enum struct scoped_int16_t : std::int16_t {};

TEST_CASE("stdint")
{
    static_assert(sizeof(char) == 1);

    {
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int8_t>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int32_t>, std::int32_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int64_t>, std::int64_t>);

        // If cv variants works here, everything else works; skip other test cases.
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int8_t const>, std::int8_t const>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int16_t const>, std::int16_t const>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int32_t const>, std::int32_t const>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int64_t const>, std::int64_t const>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int8_t const volatile>, std::int8_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int16_t const volatile>, std::int16_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int32_t const volatile>, std::int32_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::int64_t const volatile>, std::int64_t const volatile>);

        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::uint8_t>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::uint16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::uint32_t>, std::int32_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::uint64_t>, std::int64_t>);

        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<char>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<signed char>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<unsigned char>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<char8_t>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<char16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<char32_t>, std::int32_t>);

        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<std::byte>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<non_scoped_int16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_signed_of_size_t<scoped_int16_t>, std::int16_t>);
    }
    {
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int8_t>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int32_t>, std::uint32_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int64_t>, std::uint64_t>);

        // If cv variants works here, everything else works; skip other test cases.
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int8_t const>, std::uint8_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int16_t const>, std::uint16_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int32_t const>, std::uint32_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int64_t const>, std::uint64_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int8_t const volatile>, std::uint8_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int16_t const volatile>, std::uint16_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int32_t const volatile>, std::uint32_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::int64_t const volatile>, std::uint64_t const volatile>);

        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::uint8_t>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::uint16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::uint32_t>, std::uint32_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::uint64_t>, std::uint64_t>);

        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<char>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<signed char>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<unsigned char>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<char8_t>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<char16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<char32_t>, std::uint32_t>);

        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::byte>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<std::byte const>, std::uint8_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<non_scoped_int16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<non_scoped_int16_t const>, std::uint16_t const>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<scoped_int16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_unsigned_of_size_t<scoped_int16_t const>, std::uint16_t const>);
    }
    {
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int8_t>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int32_t>, std::int32_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int64_t>, std::int64_t>);

        // If cv variants works here, everything else works; skip other test cases.
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int8_t const>, std::int8_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int16_t const>, std::int16_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int32_t const>, std::int32_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int64_t const>, std::int64_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int8_t const volatile>, std::int8_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int16_t const volatile>, std::int16_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int32_t const volatile>, std::int32_t const volatile>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::int64_t const volatile>, std::int64_t const volatile>);

        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::uint8_t>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::uint16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::uint32_t>, std::uint32_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::uint64_t>, std::uint64_t>);

        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<signed char>, std::int8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<unsigned char>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<char8_t>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<char16_t>, std::uint16_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<char32_t>, std::uint32_t>);

        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::byte>, std::uint8_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<std::byte const>, std::uint8_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<non_scoped_int16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<non_scoped_int16_t const>, std::int16_t const>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<scoped_int16_t>, std::int16_t>);
        STATIC_CHECK(std::same_as<iris::make_integer_of_size_t<scoped_int16_t const>, std::int16_t const>);
    }
}

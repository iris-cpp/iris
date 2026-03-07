#include "iris_test.hpp"

#include <iris/unicode/string.hpp>

#include <vector>
#include <algorithm>
#include <string>
#include <array>
#include <ranges>

#include <cstdint>

namespace iris_unicode_test {

namespace unicode = iris::unicode;

template<class T, class... Chars>
constexpr std::array<T, sizeof...(Chars)> to_array_cast(Chars... cs)
{
    return std::array<T, sizeof...(Chars)>{
        static_cast<T>(cs)...
    };
}

TEST_CASE("append")
{
    constexpr auto do_test = []<class T>() {
        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x0448U, u);
            return u;
        }() == to_array_cast<T>(0xd1, 0x88, 0, 0, 0));

        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x65e5U, u);
            return u;
        }() == to_array_cast<T>(0xe6, 0x97, 0xa5, 0, 0));

        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x3044U, u);
            return u;
        }() == to_array_cast<T>(0xe3, 0x81, 0x84, 0, 0));

        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x10346U, u);
            return u;
        }() == to_array_cast<T>(0xf0, 0x90, 0x8d, 0x86, 0));
    };

    do_test.operator()<char8_t>();
    do_test.operator()<char>();
    do_test.operator()<unsigned char>();
    do_test.operator()<std::int8_t>();
    do_test.operator()<std::uint8_t>();
}

TEST_CASE("append16")
{
    constexpr auto do_test = []<class T>() {
        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x0448U, u);
            return u;
        }() == to_array_cast<T>(0x0448, 0, 0, 0, 0));

        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x65e5U, u);
            return u;
        }() == to_array_cast<T>(0x65e5, 0, 0, 0, 0));

        STATIC_CHECK([] {
            std::array<T, 5> u{};
            unicode::append(0x10346U, u);
            return u;
        }() == to_array_cast<T>(0xd800, 0xdf46, 0, 0, 0));
    };

    do_test.operator()<char16_t>();
    do_test.operator()<std::int16_t>();
    do_test.operator()<std::uint16_t>();
}

TEST_CASE("next")
{
    char const* twochars = "\xe6\x97\xa5\xd1\x88";
    char const* w = twochars;
    unsigned int cp = unicode::next(w, twochars + 6);

    CHECK(cp == 0x65e5);
    CHECK(w == twochars + 3);

    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars;

    cp = unicode::next(w, threechars + 9);
    CHECK(cp == 0x10346);
    CHECK(w == threechars + 4);

    cp = unicode::next(w, threechars + 9);
    CHECK(cp == 0x65e5);
    CHECK(w == threechars + 7);

    cp = unicode::next(w, threechars + 9);
    CHECK(cp == 0x0448);
    CHECK(w == threechars + 9);
}

TEST_CASE("next16")
{
    char16_t const u[3] = {0x65e5, 0xd800, 0xdf46};
    char16_t const* w = u;
    char32_t cp = unicode::next16(w, w + 3);
    CHECK(cp == 0x65e5);
    CHECK(w == u + 1);

    cp = unicode::next16(w, w + 2);
    CHECK(cp == 0x10346);
    CHECK(w == u + 3);
}

TEST_CASE("peek_next")
{
    char const* const cw = "\xe6\x97\xa5\xd1\x88";
    unsigned int cp = unicode::peek_next(cw, cw + 6);
    CHECK(cp == 0x65e5);
}

TEST_CASE("prev")
{
    char const* twochars = "\xe6\x97\xa5\xd1\x88";
    char const* w = twochars + 3;
    unsigned int cp = unicode::prev(w, twochars);
    CHECK(cp == 0x65e5);
    CHECK(w == twochars);

    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars + 9;
    cp = unicode::prev(w, threechars);
    CHECK(cp == 0x0448);
    CHECK(w == threechars + 7);
    cp = unicode::prev(w, threechars);
    CHECK(cp == 0x65e5);
    CHECK(w == threechars + 4);
    cp = unicode::prev(w, threechars);
    CHECK(cp == 0x10346);
    CHECK(w == threechars);
}

TEST_CASE("advance")
{
    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    char const* w = threechars;
    unicode::advance(w, 2, threechars + 9);
    CHECK(w == threechars + 7);
    unicode::advance(w, -2, threechars);
    CHECK(w == threechars);
    unicode::advance(w, 3, threechars + 9);
    CHECK(w == threechars + 9);
    unicode::advance(w, -2, threechars);
    CHECK(w == threechars + 4);
    unicode::advance(w, -1, threechars);
    CHECK(w == threechars);
}

TEST_CASE("distance")
{
    constexpr char const* twochars = "\xe6\x97\xa5\xd1\x88";
    std::size_t const dist = static_cast<size_t>(unicode::distance(twochars, twochars + 5));
    CHECK(dist == 2);
}

TEST_CASE("replace_invalid (vector)")
{
    char invalid_sequence[] = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    std::vector<char> replace_invalid_result;

    unicode::replace_invalid(invalid_sequence, invalid_sequence + sizeof(invalid_sequence), std::back_inserter(replace_invalid_result), '?');
    CHECK(unicode::is_valid(replace_invalid_result.begin(), replace_invalid_result.end()));

    char const fixed_invalid_sequence[] = "a????z";
    CHECK(sizeof(fixed_invalid_sequence) == replace_invalid_result.size());
    CHECK(std::equal(replace_invalid_result.begin(), replace_invalid_result.begin() + sizeof(fixed_invalid_sequence), fixed_invalid_sequence));
}

TEST_CASE("replace_invalid (string)")
{
    std::string_view invalid_sequence = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    std::string const replace_invalid_result = unicode::replace_invalid(invalid_sequence, '?');
    CHECK(unicode::is_valid(replace_invalid_result));

    std::string const fixed_invalid_sequence = "a????z";
    CHECK(fixed_invalid_sequence == replace_invalid_result);
}

TEST_CASE("replace_invalid (u8string)")
{
    std::u8string invalid_sequence = reinterpret_cast<char8_t const*>("a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z");
    std::u8string const replace_invalid_result = unicode::replace_invalid(invalid_sequence, u8'?');

    CHECK(unicode::is_valid(replace_invalid_result));
    std::u8string const fixed_invalid_sequence = reinterpret_cast<char8_t const*>("a????z");
    CHECK(fixed_invalid_sequence == replace_invalid_result);
}

#if 0

TEST_CASE("find_invalid")
{
    char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    char const* invalid = find_invalid(utf_invalid, utf_invalid + 6);
    CHECK(invalid == utf_invalid + 5);
    invalid = utf_invalid + find_invalid(utf_invalid);
    CHECK(invalid == utf_invalid + 5);
}

TEST_CASE("is_valid")
{
    char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid, utf_invalid + 6);
    CHECK(!bvalid);
    bvalid = is_valid(utf_invalid);
    CHECK(!bvalid);
    char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates, utf8_with_surrogates + 9);
    CHECK(bvalid);
    bvalid = is_valid(utf8_with_surrogates);
    CHECK(bvalid);
}

TEST_CASE("starts_with_bom")
{
    unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
    bool bbom = starts_with_bom(byte_order_mark, byte_order_mark + sizeof(byte_order_mark));
    CHECK(bbom);
    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars, threechars + sizeof(threechars));
    CHECK(!no_bbom);
}

TEST_CASE("increment")
{
    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    iris::utflib::iterator<char const*> it(threechars, threechars, threechars + 9);
    iris::utflib::iterator<char const*> it2 = it;
    CHECK(it2 == it);
    CHECK(*it == 0x10346);
    CHECK(*(++it) == 0x65e5);
    CHECK((*it++) == 0x65e5);
    CHECK(*it == 0x0448);
    EXPECT_NE (it, it2);
    iris::utflib::iterator<char const*> endit (threechars + 9, threechars, threechars + 9);
    CHECK(++it == endit);
}

TEST_CASE("decrement")
{
    char const* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    iris::utflib::iterator<char const*> it(threechars+9, threechars, threechars + 9);
    CHECK(*(--it) == 0x0448);
    CHECK((*it--) == 0x0448);
    CHECK(*it == 0x65e5);
    CHECK(--it == iris::utflib::iterator<char const*>(threechars, threechars, threechars + 9));
    CHECK(*it == 0x10346);
}

TEST_CASE("utf32to8")
{
    char32_t utf32string[] = {0x448, 0x65E5, 0x10346, 0};
    std::string utf8result;
    iris::utflib::utf32to8(utf32string, utf32string + 3, back_inserter(utf8result));
    CHECK(utf8result.size() == 9);
}

TEST_CASE("utf8to32")
{
    char const* twochars = "\xe6\x97\xa5\xd1\x88";
    std::vector<unsigned int> utf32result;
    iris::utflib::utf8to32(twochars, twochars + 5, back_inserter(utf32result));
    CHECK(utf32result.size() == 2);
}

TEST_CASE("utf16to8")
{
    char16_t utf16string[] = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    std::string utf8result;
    iris::utflib::utf16to8(utf16string, utf16string + 5, back_inserter(utf8result));
    CHECK(utf8result.size() == 10);
}

TEST_CASE("utf8to16")
{
    char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    std::vector<char16_t> utf16result;
    iris::utflib::utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9, back_inserter(utf16result));
    CHECK(utf16result.size() == 4);
    CHECK(utf16result[2] == 0xd834);
    CHECK(utf16result[3] == 0xdd1e);
}

TEST_CASE("utf16to8")
{
    std::u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    std::string u = utf16to8(utf16string);
    CHECK(u.size() == 10);
}

TEST_CASE("utf8to16")
{
    std::string utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    std::u16string utf16result = utf8to16(utf8_with_surrogates);
    CHECK(utf16result.size() == 4);
    CHECK(utf16result[2] == 0xd834);
    CHECK(utf16result[3] == 0xdd1e);
    // Just to make sure it compiles with std::string literals
    CHECK(utf8to16(u8"simple") == u"simple");
    CHECK(utf8to16("simple") == u"simple");
}

TEST_CASE("utf32to8")
{
    std::u32string utf32string = {0x448, 0x65E5, 0x10346};
    std::string utf8result = utf32to8(utf32string);
    CHECK(utf8result.size() == 9);
}

TEST_CASE("utf8to32")
{
    char const* twochars = "\xe6\x97\xa5\xd1\x88";
    std::u32string utf32result = utf8to32(twochars);
    CHECK(utf32result.size() == 2);
}

TEST_CASE("find_invalid")
{
    std::string utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    auto invalid = find_invalid(utf_invalid);
    CHECK(invalid == 5);
}

TEST_CASE("is_valid")
{
    std::string utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid);
    CHECK(!bvalid);
    std::string utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates);
    CHECK(bvalid);
}

TEST_CASE("starts_with_bom")
{
    std::string byte_order_mark = {char(0xef), char(0xbb), char(0xbf)};
    bool bbom = starts_with_bom(byte_order_mark);
    CHECK(bbom);
    std::string threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars);
    CHECK(!no_bbom);
}


TEST_CASE("utf16to8")
{
    std::u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    u16string_view utf16stringview(utf16string);
    std::string u = utf16to8(utf16stringview);
    CHECK(u.size() == 10);
}

TEST_CASE("utf8to16")
{
    std::string_view utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    std::u16string utf16result = utf8to16(utf8_with_surrogates);
    CHECK(utf16result.size() == 4);
    CHECK(utf16result[2] == 0xd834);
    CHECK(utf16result[3] == 0xdd1e);
}

TEST_CASE("utf32to8")
{
    std::u32string utf32string = {0x448, 0x65E5, 0x10346};
    u32string_view utf32stringview(utf32string);
    std::string utf8result = utf32to8(utf32stringview);
    CHECK(utf8result.size() == 9);
}

TEST_CASE("utf8to32")
{
    std::string_view twochars = "\xe6\x97\xa5\xd1\x88";
    std::u32string utf32result = utf8to32(twochars);
    CHECK(utf32result.size() == 2);
}

TEST_CASE("find_invalid")
{
    std::string_view utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    auto invalid = find_invalid(utf_invalid);
    CHECK(invalid == 5);
}

TEST_CASE("is_valid")
{
    std::string_view utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid);
    CHECK(!bvalid);
    std::string_view utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates);
    CHECK(bvalid);
}

TEST_CASE("starts_with_bom")
{
    std::string byte_order_mark = {char(0xef), char(0xbb), char(0xbf)};
    std::string_view byte_order_mark_view(byte_order_mark);
    bool bbom = starts_with_bom(byte_order_mark_view);
    CHECK(bbom);
    std::string_view threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars);
    CHECK(!no_bbom);
}

TEST(CPP17APITests, string_class_and_literals)
{
    char const* twochars = "ab";
    CHECK(is_valid(twochars));
    std::string const two_chars_string(twochars);
    CHECK(is_valid(two_chars_string));
}


TEST_CASE("utf16tou8")
{
    std::u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    u16string_view utf16stringview{utf16string};
    std::u8string u = utf16tou8(utf16string);
    CHECK(u.size() == 10);
    u = utf16tou8(utf16stringview);
    CHECK(u.size() == 10);
}

TEST_CASE("utf8to16")
{
    std::u8string utf8_with_surrogates{ reinterpret_cast<char8_t const*>("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e") };
    std::u16string utf16result = utf8to16(utf8_with_surrogates);
    CHECK(utf16result.size() == 4);
    CHECK(utf16result[2] == 0xd834);
    CHECK(utf16result[3] == 0xdd1e);
}

TEST_CASE("utf32tou8")
{
    std::u32string utf32string = {0x448, 0x65E5, 0x10346};
    u32string_view utf32stringview{utf32string};
    std::u8string utf8result = utf32tou8(utf32stringview);
    CHECK(utf8result.size() == 9);
}

TEST_CASE("utf8to32")
{
    std::u8string twochars = reinterpret_cast<char8_t const*>("\xe6\x97\xa5\xd1\x88");
    std::u32string utf32result = utf8to32(twochars);
    CHECK(utf32result.size() == 2);
}

TEST_CASE("find_invalid")
{
    std::u8string utf_invalid = reinterpret_cast<char8_t const*>("\xe6\x97\xa5\xd1\x88\xfa");
    auto invalid = find_invalid(utf_invalid);
    CHECK(invalid == 5);
}

TEST_CASE("is_valid")
{
    std::u8string utf_invalid = reinterpret_cast<char8_t const*>("\xe6\x97\xa5\xd1\x88\xfa");
    bool bvalid = is_valid(utf_invalid);
    CHECK(!bvalid);
    std::u8string utf8_with_surrogates = reinterpret_cast<char8_t const*>("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e");
    bvalid = is_valid(utf8_with_surrogates);
    CHECK(bvalid);
}

TEST_CASE("starts_with_bom")
{
    std::u8string byte_order_mark = reinterpret_cast<char8_t const*>("\xef\xbb\xbf");
    bool bbom = starts_with_bom(byte_order_mark);
    CHECK(bbom);
    std::u8string threechars = reinterpret_cast<char8_t const*>("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
    bool no_bbom = starts_with_bom(threechars);
    CHECK(!no_bbom);
}

#endif

} // iris_unicode_test

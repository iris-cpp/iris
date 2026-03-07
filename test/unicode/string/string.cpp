#include "iris_test.hpp"

#include <iris/unicode/string.hpp>

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

#if 0

TEST_CASE("next")
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    const char* w = twochars;
    unsigned int cp = next(w, twochars + 6);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, twochars + 3);

    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars;

    cp = next(w, threechars + 9);
    EXPECT_EQ (cp, 0x10346);
    EXPECT_EQ (w, threechars + 4);

    cp = next(w, threechars + 9);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, threechars + 7);

    cp = next(w, threechars + 9);
    EXPECT_EQ (cp, 0x0448);
    EXPECT_EQ (w, threechars + 9);
}

TEST_CASE("next16")
{
    const char16_t u[3] = {0x65e5, 0xd800, 0xdf46};
    const char16_t* w = u;
    char32_t cp = next16(w, w + 3);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, u + 1);

    cp = next16(w, w + 2);
    EXPECT_EQ (cp, 0x10346);
    EXPECT_EQ (w, u + 3);
}

TEST_CASE("peek_next")
{
    const char* const cw = "\xe6\x97\xa5\xd1\x88";
    unsigned int cp = peek_next(cw, cw + 6);
    EXPECT_EQ (cp, 0x65e5);
}

TEST_CASE("prior")
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    const char* w = twochars + 3;
    unsigned int cp = prior (w, twochars);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, twochars);

    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    w = threechars + 9;
    cp = prior(w, threechars);
    EXPECT_EQ (cp, 0x0448);
    EXPECT_EQ (w, threechars + 7);
    cp = prior(w, threechars);
    EXPECT_EQ (cp, 0x65e5);
    EXPECT_EQ (w, threechars + 4);
    cp = prior(w, threechars);
    EXPECT_EQ (cp, 0x10346);
    EXPECT_EQ (w, threechars);
}

TEST_CASE("advance")
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    const char* w = threechars;
    advance(w, 2, threechars + 9);
    EXPECT_EQ(w, threechars + 7);
    advance(w, -2, threechars);
    EXPECT_EQ(w, threechars);
    advance(w, 3, threechars + 9);
    EXPECT_EQ(w, threechars + 9);
    advance(w, -2, threechars);
    EXPECT_EQ(w, threechars + 4);
    advance(w, -1, threechars);
    EXPECT_EQ(w, threechars);
}

TEST_CASE("distance")
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    size_t dist = static_cast<size_t>(iris::utflib::distance(twochars, twochars + 5));
    EXPECT_EQ (dist, 2);
}

TEST_CASE("utf32to8")
{
    char32_t utf32string[] = {0x448, 0x65E5, 0x10346, 0};
    string utf8result;
    iris::utflib::utf32to8(utf32string, utf32string + 3, back_inserter(utf8result));
    EXPECT_EQ (utf8result.size(), 9);
}

TEST_CASE("utf8to32")
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    vector<unsigned int> utf32result;
    iris::utflib::utf8to32(twochars, twochars + 5, back_inserter(utf32result));
    EXPECT_EQ (utf32result.size(), 2);
}

TEST_CASE("utf16to8")
{
    char16_t utf16string[] = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    string utf8result;
    iris::utflib::utf16to8(utf16string, utf16string + 5, back_inserter(utf8result));
    EXPECT_EQ (utf8result.size(), 10);
}

TEST_CASE("utf8to16")
{
    char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    vector <char16_t> utf16result;
    iris::utflib::utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9, back_inserter(utf16result));
    EXPECT_EQ (utf16result.size(), 4);
    EXPECT_EQ (utf16result[2], 0xd834);
    EXPECT_EQ (utf16result[3], 0xdd1e);
}

TEST_CASE("replace_invalid")
{
    char invalid_sequence[] = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    vector<char> replace_invalid_result;
    replace_invalid (invalid_sequence, invalid_sequence + sizeof(invalid_sequence), std::back_inserter(replace_invalid_result), '?');
    bool bvalid = is_valid(replace_invalid_result.begin(), replace_invalid_result.end());
    EXPECT_TRUE (bvalid);
    const char fixed_invalid_sequence[] = "a????z";
    EXPECT_EQ (sizeof(fixed_invalid_sequence), replace_invalid_result.size());
    EXPECT_TRUE (std::equal(replace_invalid_result.begin(), replace_invalid_result.begin() + sizeof(fixed_invalid_sequence), fixed_invalid_sequence));
}

TEST_CASE("find_invalid")
{
    char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    const char* invalid = find_invalid(utf_invalid, utf_invalid + 6);
    EXPECT_EQ (invalid, utf_invalid + 5);
    invalid = utf_invalid + find_invalid(utf_invalid);
    EXPECT_EQ (invalid, utf_invalid + 5);
}

TEST_CASE("is_valid")
{
    char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid, utf_invalid + 6);
    EXPECT_FALSE (bvalid);
    bvalid = is_valid(utf_invalid);
    EXPECT_FALSE (bvalid);
    char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates, utf8_with_surrogates + 9);
    EXPECT_TRUE (bvalid);
    bvalid = is_valid(utf8_with_surrogates);
    EXPECT_TRUE (bvalid);
}

TEST_CASE("starts_with_bom")
{
    unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
    bool bbom = starts_with_bom(byte_order_mark, byte_order_mark + sizeof(byte_order_mark));
    EXPECT_TRUE (bbom);
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars, threechars + sizeof(threechars));
    EXPECT_FALSE (no_bbom);
}

TEST_CASE("increment")
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    iris::utflib::iterator<const char*> it(threechars, threechars, threechars + 9);
    iris::utflib::iterator<const char*> it2 = it;
    EXPECT_EQ (it2, it);
    EXPECT_EQ (*it, 0x10346);
    EXPECT_EQ (*(++it), 0x65e5);
    EXPECT_EQ ((*it++), 0x65e5);
    EXPECT_EQ (*it, 0x0448);
    EXPECT_NE (it, it2);
    iris::utflib::iterator<const char*> endit (threechars + 9, threechars, threechars + 9);
    EXPECT_EQ (++it, endit);
}

TEST_CASE("decrement")
{
    const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    iris::utflib::iterator<const char*> it(threechars+9, threechars, threechars + 9);
    EXPECT_EQ (*(--it), 0x0448);
    EXPECT_EQ ((*it--), 0x0448);
    EXPECT_EQ (*it, 0x65e5);
    EXPECT_EQ (--it, iris::utflib::iterator<const char*>(threechars, threechars, threechars + 9));
    EXPECT_EQ (*it, 0x10346);
}

TEST_CASE("utf16to8")
{
    u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    string u = utf16to8(utf16string);
    EXPECT_EQ (u.size(), 10);
}

TEST_CASE("utf8to16")
{
    string utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    u16string utf16result = utf8to16(utf8_with_surrogates);
    EXPECT_EQ (utf16result.size(), 4);
    EXPECT_EQ (utf16result[2], 0xd834);
    EXPECT_EQ (utf16result[3], 0xdd1e);
    // Just to make sure it compiles with string literals
    EXPECT_EQ(utf8to16(u8"simple"), u"simple");
    EXPECT_EQ(utf8to16("simple"), u"simple");
}

TEST_CASE("utf32to8")
{
    u32string utf32string = {0x448, 0x65E5, 0x10346};
    string utf8result = utf32to8(utf32string);
    EXPECT_EQ (utf8result.size(), 9);
}

TEST_CASE("utf8to32")
{
    const char* twochars = "\xe6\x97\xa5\xd1\x88";
    u32string utf32result = utf8to32(twochars);
    EXPECT_EQ (utf32result.size(), 2);
}

TEST_CASE("find_invalid")
{
    string utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    auto invalid = find_invalid(utf_invalid);
    EXPECT_EQ (invalid, 5);
}

TEST_CASE("is_valid")
{
    string utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid);
    EXPECT_FALSE (bvalid);
    string utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates);
    EXPECT_TRUE (bvalid);
}

TEST_CASE("replace_invalid")
{
    string invalid_sequence = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    string replace_invalid_result = replace_invalid(invalid_sequence, '?');
    bool bvalid = is_valid(replace_invalid_result);
    EXPECT_TRUE (bvalid);
    const string fixed_invalid_sequence = "a????z";
    EXPECT_EQ(fixed_invalid_sequence, replace_invalid_result);
}

TEST_CASE("starts_with_bom")
{
    string byte_order_mark = {char(0xef), char(0xbb), char(0xbf)};
    bool bbom = starts_with_bom(byte_order_mark);
    EXPECT_TRUE (bbom);
    string threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars);
    EXPECT_FALSE (no_bbom);
}


TEST_CASE("utf16to8")
{
    u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    u16string_view utf16stringview(utf16string);
    string u = utf16to8(utf16stringview);
    EXPECT_EQ (u.size(), 10);
}

TEST_CASE("utf8to16")
{
    string_view utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    u16string utf16result = utf8to16(utf8_with_surrogates);
    EXPECT_EQ (utf16result.size(), 4);
    EXPECT_EQ (utf16result[2], 0xd834);
    EXPECT_EQ (utf16result[3], 0xdd1e);
}

TEST_CASE("utf32to8")
{
    u32string utf32string = {0x448, 0x65E5, 0x10346};
    u32string_view utf32stringview(utf32string);
    string utf8result = utf32to8(utf32stringview);
    EXPECT_EQ (utf8result.size(), 9);
}

TEST_CASE("utf8to32")
{
    string_view twochars = "\xe6\x97\xa5\xd1\x88";
    u32string utf32result = utf8to32(twochars);
    EXPECT_EQ (utf32result.size(), 2);
}

TEST_CASE("find_invalid")
{
    string_view utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    auto invalid = find_invalid(utf_invalid);
    EXPECT_EQ (invalid, 5);
}

TEST_CASE("is_valid")
{
    string_view utf_invalid = "\xe6\x97\xa5\xd1\x88\xfa";
    bool bvalid = is_valid(utf_invalid);
    EXPECT_FALSE (bvalid);
    string_view utf8_with_surrogates = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
    bvalid = is_valid(utf8_with_surrogates);
    EXPECT_TRUE (bvalid);
}

TEST_CASE("replace_invalid")
{
    string_view invalid_sequence = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
    string replace_invalid_result = replace_invalid(invalid_sequence, '?');
    bool bvalid = is_valid(replace_invalid_result);
    EXPECT_TRUE (bvalid);
    const string fixed_invalid_sequence = "a????z";
    EXPECT_EQ(fixed_invalid_sequence, replace_invalid_result);
}

TEST_CASE("starts_with_bom")
{
    string byte_order_mark = {char(0xef), char(0xbb), char(0xbf)};
    string_view byte_order_mark_view(byte_order_mark);
    bool bbom = starts_with_bom(byte_order_mark_view);
    EXPECT_TRUE (bbom);
    string_view threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
    bool no_bbom = starts_with_bom(threechars);
    EXPECT_FALSE (no_bbom);
}

TEST(CPP17APITests, string_class_and_literals)
{
    const char* twochars = "ab";
    EXPECT_TRUE (is_valid(twochars));
    const string two_chars_string(twochars);
    EXPECT_TRUE (is_valid(two_chars_string));
}


TEST_CASE("utf16tou8")
{
    u16string utf16string = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
    u16string_view utf16stringview{utf16string};
    u8string u = utf16tou8(utf16string);
    EXPECT_EQ (u.size(), 10);
    u = utf16tou8(utf16stringview);
    EXPECT_EQ (u.size(), 10);
}

TEST(CPP20APITests, tes20t_utf8to16)
{
    u8string utf8_with_surrogates{ reinterpret_cast<const char8_t*>("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e") };
    u16string utf16result = utf8to16(utf8_with_surrogates);
    EXPECT_EQ (utf16result.size(), 4);
    EXPECT_EQ (utf16result[2], 0xd834);
    EXPECT_EQ (utf16result[3], 0xdd1e);
}

TEST_CASE("utf32tou8")
{
    u32string utf32string = {0x448, 0x65E5, 0x10346};
    u32string_view utf32stringview{utf32string};
    u8string utf8result = utf32tou8(utf32stringview);
    EXPECT_EQ (utf8result.size(), 9);
}

TEST_CASE("utf8to32")
{
    u8string twochars = reinterpret_cast<const char8_t*>("\xe6\x97\xa5\xd1\x88");
    u32string utf32result = utf8to32(twochars);
    EXPECT_EQ (utf32result.size(), 2);
}

TEST_CASE("find_invalid")
{
    u8string utf_invalid = reinterpret_cast<const char8_t*>("\xe6\x97\xa5\xd1\x88\xfa");
    auto invalid = find_invalid(utf_invalid);
    EXPECT_EQ (invalid, 5);
}

TEST_CASE("is_valid")
{
    u8string utf_invalid = reinterpret_cast<const char8_t*>("\xe6\x97\xa5\xd1\x88\xfa");
    bool bvalid = is_valid(utf_invalid);
    EXPECT_FALSE (bvalid);
    u8string utf8_with_surrogates = reinterpret_cast<const char8_t*>("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e");
    bvalid = is_valid(utf8_with_surrogates);
    EXPECT_TRUE (bvalid);
}

TEST_CASE("replace_invalid")
{
    u8string invalid_sequence = reinterpret_cast<const char8_t*>("a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z");
    u8string replace_invalid_result = replace_invalid(invalid_sequence, u8'?');
    bool bvalid = is_valid(replace_invalid_result);
    EXPECT_TRUE (bvalid);
    const u8string fixed_invalid_sequence = reinterpret_cast<const char8_t*>("a????z");
    EXPECT_EQ(fixed_invalid_sequence, replace_invalid_result);
}

TEST_CASE("starts_with_bom")
{
    u8string byte_order_mark = reinterpret_cast<const char8_t*>("\xef\xbb\xbf");
    bool bbom = starts_with_bom(byte_order_mark);
    EXPECT_TRUE (bbom);
    u8string threechars = reinterpret_cast<const char8_t*>("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
    bool no_bbom = starts_with_bom(threechars);
    EXPECT_FALSE (no_bbom);
}

#endif

} // iris_unicode_test

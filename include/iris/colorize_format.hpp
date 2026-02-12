#ifndef IRIS_COLORIZE_FORMAT_HPP
#define IRIS_COLORIZE_FORMAT_HPP

#include <iris/string.hpp>
#include <iris/enum_bitops.hpp>
#include <iris/enum_bitops_algorithm.hpp>
#include <iris/fixed_string.hpp>

#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <iostream>
#include <optional>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <cstdint>

#if defined(_POSIX_SOURCE)
#include <stdio.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#endif

namespace iris {

class colorize_error : public std::invalid_argument
{
public:
    using invalid_argument::invalid_argument;
};

namespace ansi_colorize {

template<class CharT>
struct basic_colorize_parse_context
{
    using char_type = CharT;
    using const_iterator = typename std::basic_string_view<CharT>::const_iterator;
    using iterator = const_iterator;

    constexpr explicit basic_colorize_parse_context(std::basic_string_view<CharT> fmt) noexcept
        : begin_(fmt.begin()), end_(fmt.end())
    {}

    constexpr iterator begin() const noexcept { return begin_; }
    constexpr iterator end() const noexcept { return end_; }
    constexpr void advance_to(iterator it) noexcept { begin_ = it; }

private:
    iterator begin_;
    iterator end_;
};

template<class Out, class CharT>
struct basic_colorize_context
{
    using char_type = CharT;
    using iterator = Out;

    explicit constexpr basic_colorize_context(Out out)
        : out_(std::move(out))
    {}

    constexpr iterator out() { return std::move(out_); }
    constexpr void advance_to(iterator it) { out_ = std::move(it); }

private:
    iterator out_;
};

using colorize_parse_context = basic_colorize_parse_context<char>;

namespace detail {

enum class rgb_color : uint32_t
{
    // grayscale
    black = 0x000000,      // rgb(0,0,0)
    dimgray = 0x696969,    // rgb(105,105,105)
    gray = 0x808080,       // rgb(128,128,128)
    darkgray = 0xA9A9A9,   // rgb(169,169,169)
    silver = 0xC0C0C0,     // rgb(192,192,192)
    lightgray = 0xD3D3D3,  // rgb(211,211,211)
    gainsboro = 0xDCDCDC,  // rgb(220,220,220)
    whitesmoke = 0xF5F5F5, // rgb(245,245,245)
    white = 0xFFFFFF,      // rgb(255,255,255)

    // pure color
    red = 0xFF0000,         // rgb(255,0,0)
    orangered = 0xFF4500,   // rgb(255,69,0)
    darkorange = 0xFF8C00,  // rgb(255,140,0)
    orange = 0xFFA500,      // rgb(255,165,0)
    gold = 0xFFD700,        // rgb(255,215,0)
    yellow = 0xFFFF00,      // rgb(255,255,0)
    chartreuse = 0x7FFF00,  // rgb(127,255,0)
    lime = 0x00FF00,        // rgb(0,255,0)
    springgreen = 0x00FF7F, // rgb(0,255,127)
    aqua = 0x00FFFF,        // rgb(0,255,255)
    cyan = 0x00FFFF,        // rgb(0,255,255)
    deepskyblue = 0x00BFFF, // rgb(0,191,255)
    blue = 0x0000FF,        // rgb(0,0,255)
    fuchsia = 0xFF00FF,     // rgb(255,0,255)
    magenta = 0xFF00FF,     // rgb(255,0,255)

    // others
    snow = 0xFFFAFA,                 // rgb(255,250,250)
    rosybrown = 0xBC8F8F,            // rgb(188,143,143)
    lightcoral = 0xF08080,           // rgb(240,128,128)
    indianred = 0xCD5C5C,            // rgb(205,92,92)
    brown = 0xA52A2A,                // rgb(165,42,42)
    firebrick = 0xB22222,            // rgb(178,34,34)
    maroon = 0x800000,               // rgb(128,0,0)
    darkred = 0x8B0000,              // rgb(139,0,0)
    mistyrose = 0xFFE4E1,            // rgb(255,228,225)
    salmon = 0xFA8072,               // rgb(250,128,114)
    tomato = 0xFF6347,               // rgb(255,99,71)
    darksalmon = 0xE9967A,           // rgb(233,150,122)
    coral = 0xFF7F50,                // rgb(255,127,80)
    lightsalmon = 0xFFA07A,          // rgb(255,160,122)
    sienna = 0xA0522D,               // rgb(160,82,45)
    seashell = 0xFFF5EE,             // rgb(255,245,238)
    chocolate = 0xD2691E,            // rgb(210,105,30)
    saddlebrown = 0x8B4513,          // rgb(139,69,19)
    sandybrown = 0xF4A460,           // rgb(244,164,96)
    peachpuff = 0xFFDAB9,            // rgb(255,218,185)
    peru = 0xCD853F,                 // rgb(205,133,63)
    linen = 0xFAF0E6,                // rgb(250,240,230)
    bisque = 0xFFE4C4,               // rgb(255,228,196)
    burlywood = 0xDEB887,            // rgb(222,184,135)
    antiquewhite = 0xFAEBD7,         // rgb(250,235,215)
    tan = 0xD2B48C,                  // rgb(210,180,140)
    navajowhite = 0xFFDEAD,          // rgb(255,222,173)
    blanchedalmond = 0xFFEBCD,       // rgb(255,235,205)
    papayawhip = 0xFFEFD5,           // rgb(255,239,213)
    moccasin = 0xFFE4B5,             // rgb(255,228,181)
    wheat = 0xF5DEB3,                // rgb(245,222,179)
    oldlace = 0xFDF5E6,              // rgb(253,245,230)
    floralwhite = 0xFFFAF0,          // rgb(255,250,240)
    darkgoldenrod = 0xB8860B,        // rgb(184,134,11)
    goldenrod = 0xDAA520,            // rgb(218,165,32)
    cornsilk = 0xFFF8DC,             // rgb(255,248,220)
    lemonchiffon = 0xFFFACD,         // rgb(255,250,205)
    khaki = 0xF0E68C,                // rgb(240,230,140)
    palegoldenrod = 0xEEE8AA,        // rgb(238,232,170)
    darkkhaki = 0xBDB76B,            // rgb(189,183,107)
    ivory = 0xFFFFF0,                // rgb(255,255,240)
    beige = 0xF5F5DC,                // rgb(245,245,220)
    lightyellow = 0xFFFFE0,          // rgb(255,255,224)
    lightgoldenrodyellow = 0xFAFAD2, // rgb(250,250,210)
    olive = 0x808000,                // rgb(128,128,0)
    olivedrab = 0x6B8E23,            // rgb(107,142,35)
    yellowgreen = 0x9ACD32,          // rgb(154,205,50)
    darkolivegreen = 0x556B2F,       // rgb(85,107,47)
    greenyellow = 0xADFF2F,          // rgb(173,255,47)
    lawngreen = 0x7CFC00,            // rgb(124,252,0)
    honeydew = 0xF0FFF0,             // rgb(240,255,240)
    darkseagreen = 0x8FBC8F,         // rgb(143,188,143)
    palegreen = 0x98FB98,            // rgb(152,251,152)
    lightgreen = 0x90EE90,           // rgb(144,238,144)
    forestgreen = 0x228B22,          // rgb(34,139,34)
    limegreen = 0x32CD32,            // rgb(50,205,50)
    darkgreen = 0x006400,            // rgb(0,100,0)
    green = 0x008000,                // rgb(0,128,0)
    seagreen = 0x2E8B57,             // rgb(46,139,87)
    mediumseagreen = 0x3CB371,       // rgb(60,179,113)
    mintcream = 0xF5FFFA,            // rgb(245,255,250)
    mediumspringgreen = 0x00FA9A,    // rgb(0,250,154)
    mediumaquamarine = 0x66CDAA,     // rgb(102,205,170)
    aquamarine = 0x7FFFD4,           // rgb(127,255,212)
    turquoise = 0x40E0D0,            // rgb(64,224,208)
    lightseagreen = 0x20B2AA,        // rgb(32,178,170)
    mediumturquoise = 0x48D1CC,      // rgb(72,209,204)
    azure = 0xF0FFFF,                // rgb(240,255,255)
    lightcyan = 0xE0FFFF,            // rgb(224,255,255)
    paleturquoise = 0xAFEEEE,        // rgb(175,238,238)
    darkslategray = 0x2F4F4F,        // rgb(47,79,79)
    teal = 0x008080,                 // rgb(0,128,128)
    darkcyan = 0x008B8B,             // rgb(0,139,139)
    darkturquoise = 0x00CED1,        // rgb(0,206,209)
    cadetblue = 0x5F9EA0,            // rgb(95,158,160)
    powderblue = 0xB0E0E6,           // rgb(176,224,230)
    lightblue = 0xADD8E6,            // rgb(173,216,230)
    skyblue = 0x87CEEB,              // rgb(135,206,235)
    lightskyblue = 0x87CEFA,         // rgb(135,206,250)
    steelblue = 0x4682B4,            // rgb(70,130,180)
    aliceblue = 0xF0F8FF,            // rgb(240,248,255)
    dodgerblue = 0x1E90FF,           // rgb(30,144,255)
    lightslategray = 0x778899,       // rgb(119,136,153)
    slategray = 0x708090,            // rgb(112,128,144)
    lightsteelblue = 0xB0C4DE,       // rgb(176,196,222)
    cornflowerblue = 0x6495ED,       // rgb(100,149,237)
    royalblue = 0x4169E1,            // rgb(65,105,225)
    ghostwhite = 0xF8F8FF,           // rgb(248,248,255)
    lavender = 0xE6E6FA,             // rgb(230,230,250)
    midnightblue = 0x191970,         // rgb(25,25,112)
    navy = 0x000080,                 // rgb(0,0,128)
    darkblue = 0x00008B,             // rgb(0,0,139)
    mediumblue = 0x0000CD,           // rgb(0,0,205)
    slateblue = 0x6A5ACD,            // rgb(106,90,205)
    darkslateblue = 0x483D8B,        // rgb(72,61,139)
    mediumslateblue = 0x7B68EE,      // rgb(123,104,238)
    mediumpurple = 0x9370DB,         // rgb(147,112,219)
    rebeccapurple = 0x663399,        // rgb(102,51,153)
    blueviolet = 0x8A2BE2,           // rgb(138,43,226)
    indigo = 0x4B0082,               // rgb(75,0,130)
    darkorchid = 0x9932CC,           // rgb(153,50,204)
    darkviolet = 0x9400D3,           // rgb(148,0,211)
    mediumorchid = 0xBA55D3,         // rgb(186,85,211)
    thistle = 0xD8BFD8,              // rgb(216,191,216)
    plum = 0xDDA0DD,                 // rgb(221,160,221)
    violet = 0xEE82EE,               // rgb(238,130,238)
    purple = 0x800080,               // rgb(128,0,128)
    darkmagenta = 0x8B008B,          // rgb(139,0,139)
    orchid = 0xDA70D6,               // rgb(218,112,214)
    mediumvioletred = 0xC71585,      // rgb(199,21,133)
    deeppink = 0xFF1493,             // rgb(255,20,147)
    hotpink = 0xFF69B4,              // rgb(255,105,180)
    lavenderblush = 0xFFF0F5,        // rgb(255,240,245)
    palevioletred = 0xDB7093,        // rgb(219,112,147)
    crimson = 0xDC143C,              // rgb(220,20,60)
    pink = 0xFFC0CB,                 // rgb(255,192,203)
    lightpink = 0xFFB6C1,            // rgb(255,182,193)
};

constexpr auto get_rgb(rgb_color rgb) noexcept
{
    std::uint32_t val = std::to_underlying(rgb);
    return std::make_tuple(std::uint8_t((val >> 16) & 0xFF), std::uint8_t((val >> 8) & 0xFF), std::uint8_t(val & 0xFF));
}

class color
{
public:
    constexpr color() noexcept = default;
    constexpr color(rgb_color rgb) noexcept : value(rgb) {}

    constexpr bool is_rgb_color() const noexcept { return value.has_value(); }

    constexpr rgb_color get_rgb_color() const noexcept { return *value; }

    constexpr bool empty() const noexcept { return !value.has_value(); }

private:
    std::optional<rgb_color> value;
};

struct color_pair
{
    std::string_view name;
    color value;
};

// Must be sorted
static constexpr auto color_lookup_table = std::array
{
    color_pair{"aliceblue", rgb_color::aliceblue},
    color_pair{"antiquewhite", rgb_color::antiquewhite},
    color_pair{"aqua", rgb_color::aqua},
    color_pair{"aquamarine", rgb_color::aquamarine},
    color_pair{"azure", rgb_color::azure},
    color_pair{"beige", rgb_color::beige},
    color_pair{"bisque", rgb_color::bisque},
    color_pair{"black", rgb_color::black},
    color_pair{"blanchedalmond", rgb_color::blanchedalmond},
    color_pair{"blue", rgb_color::blue},
    color_pair{"blueviolet", rgb_color::blueviolet},
    color_pair{"brown", rgb_color::brown},
    color_pair{"burlywood", rgb_color::burlywood},
    color_pair{"cadetblue", rgb_color::cadetblue},
    color_pair{"chartreuse", rgb_color::chartreuse},
    color_pair{"chocolate", rgb_color::chocolate},
    color_pair{"coral", rgb_color::coral},
    color_pair{"cornflowerblue", rgb_color::cornflowerblue},
    color_pair{"cornsilk", rgb_color::cornsilk},
    color_pair{"crimson", rgb_color::crimson},
    color_pair{"cyan", rgb_color::cyan},
    color_pair{"darkblue", rgb_color::darkblue},
    color_pair{"darkcyan", rgb_color::darkcyan},
    color_pair{"darkgoldenrod", rgb_color::darkgoldenrod},
    color_pair{"darkgray", rgb_color::darkgray},
    color_pair{"darkgreen", rgb_color::darkgreen},
    color_pair{"darkkhaki", rgb_color::darkkhaki},
    color_pair{"darkmagenta", rgb_color::darkmagenta},
    color_pair{"darkolivegreen", rgb_color::darkolivegreen},
    color_pair{"darkorange", rgb_color::darkorange},
    color_pair{"darkorchid", rgb_color::darkorchid},
    color_pair{"darkred", rgb_color::darkred},
    color_pair{"darksalmon", rgb_color::darksalmon},
    color_pair{"darkseagreen", rgb_color::darkseagreen},
    color_pair{"darkslateblue", rgb_color::darkslateblue},
    color_pair{"darkslategray", rgb_color::darkslategray},
    color_pair{"darkturquoise", rgb_color::darkturquoise},
    color_pair{"darkviolet", rgb_color::darkviolet},
    color_pair{"deeppink", rgb_color::deeppink},
    color_pair{"deepskyblue", rgb_color::deepskyblue},
    color_pair{"dimgray", rgb_color::dimgray},
    color_pair{"dodgerblue", rgb_color::dodgerblue},
    color_pair{"firebrick", rgb_color::firebrick},
    color_pair{"floralwhite", rgb_color::floralwhite},
    color_pair{"forestgreen", rgb_color::forestgreen},
    color_pair{"fuchsia", rgb_color::fuchsia},
    color_pair{"gainsboro", rgb_color::gainsboro},
    color_pair{"ghostwhite", rgb_color::ghostwhite},
    color_pair{"gold", rgb_color::gold},
    color_pair{"goldenrod", rgb_color::goldenrod},
    color_pair{"gray", rgb_color::gray},
    color_pair{"green", rgb_color::green},
    color_pair{"greenyellow", rgb_color::greenyellow},
    color_pair{"honeydew", rgb_color::honeydew},
    color_pair{"hotpink", rgb_color::hotpink},
    color_pair{"indianred", rgb_color::indianred},
    color_pair{"indigo", rgb_color::indigo},
    color_pair{"ivory", rgb_color::ivory},
    color_pair{"khaki", rgb_color::khaki},
    color_pair{"lavender", rgb_color::lavender},
    color_pair{"lavenderblush", rgb_color::lavenderblush},
    color_pair{"lawngreen", rgb_color::lawngreen},
    color_pair{"lemonchiffon", rgb_color::lemonchiffon},
    color_pair{"lightblue", rgb_color::lightblue},
    color_pair{"lightcoral", rgb_color::lightcoral},
    color_pair{"lightcyan", rgb_color::lightcyan},
    color_pair{"lightgoldenrodyellow", rgb_color::lightgoldenrodyellow},
    color_pair{"lightgray", rgb_color::lightgray},
    color_pair{"lightgreen", rgb_color::lightgreen},
    color_pair{"lightpink", rgb_color::lightpink},
    color_pair{"lightsalmon", rgb_color::lightsalmon},
    color_pair{"lightseagreen", rgb_color::lightseagreen},
    color_pair{"lightskyblue", rgb_color::lightskyblue},
    color_pair{"lightslategray", rgb_color::lightslategray},
    color_pair{"lightsteelblue", rgb_color::lightsteelblue},
    color_pair{"lightyellow", rgb_color::lightyellow},
    color_pair{"lime", rgb_color::lime},
    color_pair{"limegreen", rgb_color::limegreen},
    color_pair{"linen", rgb_color::linen},
    color_pair{"magenta", rgb_color::magenta},
    color_pair{"maroon", rgb_color::maroon},
    color_pair{"mediumaquamarine", rgb_color::mediumaquamarine},
    color_pair{"mediumblue", rgb_color::mediumblue},
    color_pair{"mediumorchid", rgb_color::mediumorchid},
    color_pair{"mediumpurple", rgb_color::mediumpurple},
    color_pair{"mediumseagreen", rgb_color::mediumseagreen},
    color_pair{"mediumslateblue", rgb_color::mediumslateblue},
    color_pair{"mediumspringgreen", rgb_color::mediumspringgreen},
    color_pair{"mediumturquoise", rgb_color::mediumturquoise},
    color_pair{"mediumvioletred", rgb_color::mediumvioletred},
    color_pair{"midnightblue", rgb_color::midnightblue},
    color_pair{"mintcream", rgb_color::mintcream},
    color_pair{"mistyrose", rgb_color::mistyrose},
    color_pair{"moccasin", rgb_color::moccasin},
    color_pair{"navajowhite", rgb_color::navajowhite},
    color_pair{"navy", rgb_color::navy},
    color_pair{"oldlace", rgb_color::oldlace},
    color_pair{"olive", rgb_color::olive},
    color_pair{"olivedrab", rgb_color::olivedrab},
    color_pair{"orange", rgb_color::orange},
    color_pair{"orangered", rgb_color::orangered},
    color_pair{"orchid", rgb_color::orchid},
    color_pair{"palegoldenrod", rgb_color::palegoldenrod},
    color_pair{"palegreen", rgb_color::palegreen},
    color_pair{"paleturquoise", rgb_color::paleturquoise},
    color_pair{"palevioletred", rgb_color::palevioletred},
    color_pair{"papayawhip", rgb_color::papayawhip},
    color_pair{"peachpuff", rgb_color::peachpuff},
    color_pair{"peru", rgb_color::peru},
    color_pair{"pink", rgb_color::pink},
    color_pair{"plum", rgb_color::plum},
    color_pair{"powderblue", rgb_color::powderblue},
    color_pair{"purple", rgb_color::purple},
    color_pair{"rebeccapurple", rgb_color::rebeccapurple},
    color_pair{"red", rgb_color::red},
    color_pair{"rosybrown", rgb_color::rosybrown},
    color_pair{"royalblue", rgb_color::royalblue},
    color_pair{"saddlebrown", rgb_color::saddlebrown},
    color_pair{"salmon", rgb_color::salmon},
    color_pair{"sandybrown", rgb_color::sandybrown},
    color_pair{"seagreen", rgb_color::seagreen},
    color_pair{"seashell", rgb_color::seashell},
    color_pair{"sienna", rgb_color::sienna},
    color_pair{"silver", rgb_color::silver},
    color_pair{"skyblue", rgb_color::skyblue},
    color_pair{"slateblue", rgb_color::slateblue},
    color_pair{"slategray", rgb_color::slategray},
    color_pair{"snow", rgb_color::snow},
    color_pair{"springgreen", rgb_color::springgreen},
    color_pair{"steelblue", rgb_color::steelblue},
    color_pair{"tan", rgb_color::tan},
    color_pair{"teal", rgb_color::teal},
    color_pair{"thistle", rgb_color::thistle},
    color_pair{"tomato", rgb_color::tomato},
    color_pair{"turquoise", rgb_color::turquoise},
    color_pair{"violet", rgb_color::violet},
    color_pair{"wheat", rgb_color::wheat},
    color_pair{"white", rgb_color::white},
    color_pair{"whitesmoke", rgb_color::whitesmoke},
    color_pair{"yellow", rgb_color::yellow},
    color_pair{"yellowgreen", rgb_color::yellowgreen},
};

[[nodiscard]] static constexpr color name_to_color(std::string_view name) noexcept
{
    auto it = std::ranges::lower_bound(color_lookup_table, name, {}, &color_pair::name);
    if (it == color_lookup_table.end() || it->name != name) return color{};
    return it->value;
}

enum class emphasis : std::uint8_t
{
    bold = 1 << 0,
    faint = 1 << 1,
    italic = 1 << 2,
    underline = 1 << 3,
    blink = 1 << 4,
    reverse = 1 << 5,
    conceal = 1 << 6,
    strike = 1 << 7,
};

[[nodiscard]] static constexpr emphasis name_to_emphasis(std::string_view name) noexcept
{
    if (name == "bold") return emphasis::bold;
    if (name == "faint") return emphasis::faint;
    if (name == "italic") return emphasis::italic;
    if (name == "underline") return emphasis::underline;
    if (name == "blink") return emphasis::blink;
    if (name == "reverse") return emphasis::reverse;
    if (name == "conceal") return emphasis::conceal;
    if (name == "strike") return emphasis::strike;
    return emphasis{};
}

static constexpr std::uint8_t emphasis_to_value(emphasis em)
{
    switch (em) {
    case emphasis::bold:      return 1;
    case emphasis::faint:     return 2;
    case emphasis::italic:    return 3;
    case emphasis::underline: return 4;
    case emphasis::blink:     return 5;
    case emphasis::reverse:   return 7;
    case emphasis::conceal:   return 8;
    case emphasis::strike:    return 9;
    default: throw std::invalid_argument("invalid emphasis");
    }
}

} // detail

} // ansi_colorize

template<>
struct bitops_enabled<ansi_colorize::detail::emphasis> : std::true_type {
    static constexpr int max_bit = 7;
};

namespace ansi_colorize {

template<class CharT = char>
struct colorizer
{
    [[nodiscard]] constexpr auto parse(basic_colorize_parse_context<CharT>& pc)
    {
        auto const [it, fg_color, bg_color, emphasis, reset, fg_reset, bg_reset] = colorizer::do_parse(pc);
        fg_color_ = fg_color;
        bg_color_ = bg_color;
        emphasis_ = emphasis;
        reset_ = reset;
        fg_reset_ = fg_reset;
        bg_reset_ = bg_reset;
        return it;
    }

    template<class Out>
    constexpr auto colorize(basic_colorize_context<Out, CharT>& cc) const
    {
        using namespace std::string_view_literals;

        // shorthand for reset
        if (reset_) return std::ranges::copy("\033[0m"sv, cc.out()).out;

        // write prefix
        auto it = std::ranges::copy("\033["sv, cc.out()).out;

        bool first = true;

        auto const append_u8_to = [&first](auto it, std::uint8_t value) {
            if (!first) {
                *it = ';';
                ++it;
            }
            first = false;
            CharT buf[4];
            auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), value);
            if (ec != std::errc{}) throw colorize_error("internal buffer error");
            return std::ranges::copy(buf, ptr, std::move(it)).out;
        };

        // write emphases
        for (auto em : iris::each_bit(emphasis_)) {
            it = append_u8_to(std::move(it), detail::emphasis_to_value(em));
        }

        if (fg_reset_) {
            it = append_u8_to(std::move(it), 39);
        }
        if (bg_reset_) {
            it = append_u8_to(std::move(it), 49);
        }

        // write foreground color
        if (!fg_color_.empty()) {
            detail::rgb_color rgb = fg_color_.get_rgb_color();
            auto [r, g, b] = detail::get_rgb(rgb);
            it = append_u8_to(std::move(it), 38);
            it = append_u8_to(std::move(it), 2);
            it = append_u8_to(std::move(it), r);
            it = append_u8_to(std::move(it), g);
            it = append_u8_to(std::move(it), b);
        }

        // write background color
        if (!bg_color_.empty()) {
            detail::rgb_color rgb = bg_color_.get_rgb_color();
            auto [r, g, b] = detail::get_rgb(rgb);
            it = append_u8_to(std::move(it), 48);
            it = append_u8_to(std::move(it), 2);
            it = append_u8_to(std::move(it), r);
            it = append_u8_to(std::move(it), g);
            it = append_u8_to(std::move(it), b);
        }

        // write suffix
        *it = 'm';
        ++it;

        return it;
    }

private:
    struct do_parse_result
    {
        basic_colorize_parse_context<CharT>::iterator in;
        detail::color fg_color;
        detail::color bg_color;
        detail::emphasis emphasis;
        bool reset;
        bool fg_reset;
        bool bg_reset;
    };

    [[nodiscard]] static constexpr detail::color parse_rgb(std::string_view spec)
    {
        if (!spec.starts_with("rgb(")) {
            return detail::color{};
        }
        auto rest = spec.substr(4);
        auto const last = spec.data() + spec.size();
        std::uint8_t r, g, b;
        auto [ptr1, ec1] = std::from_chars(rest.data(), last, r);
        if (ec1 != std::errc{}) throw colorize_error("invalid number");
        if (*ptr1++ != ',') throw colorize_error("invalid delimiter");
        auto [ptr2, ec2] = std::from_chars(ptr1, last, g);
        if (ec2 != std::errc{}) throw colorize_error("invalid number");
        if (*ptr2++ != ',') throw colorize_error("invalid delimiter");
        auto [ptr3, ec3] = std::from_chars(ptr2, last, b);
        if (ec3 != std::errc{}) throw colorize_error("invalid number");
        if (*ptr3 != ')') throw colorize_error("unmatched right parenthesis");
        return detail::rgb_color{std::uint32_t(r) << 16 | std::uint32_t(g) << 8 | std::uint32_t(b)};
    }

    static constexpr do_parse_result do_parse(basic_colorize_parse_context<CharT>& pc)
    {
        do_parse_result result{
            pc.begin(), detail::color{}, detail::color{}, detail::emphasis{}, false, false, false,
        };

        auto& it = result.in;
        while (it != pc.end()) {
            if (*it == ']') break;
            if (*it == '|') pc.advance_to(++it);

            std::string_view const rest{it, pc.end()};
            auto const len = rest.find_first_of("|]");
            std::string_view const specifier = rest.substr(0, len);

            if (specifier == "reset") {
                result.reset = true;

            } else if (auto rgb_color = parse_rgb(specifier); !rgb_color.empty()) {
                if (!result.fg_color.empty()) throw colorize_error("multiple colors must not be specified");
                result.fg_color = rgb_color;

            } else if (auto color = detail::name_to_color(specifier); !color.empty()) {
                if (!result.fg_color.empty()) throw colorize_error("multiple colors must not be specified");
                result.fg_color = color;

            } else if (auto emphasis = detail::name_to_emphasis(specifier); emphasis != detail::emphasis{}) {
                using namespace bitops_operators;
                result.emphasis |= emphasis;

            } else if (specifier.starts_with("fg:")) {
                auto const spec = specifier.substr(3);

                if (spec == "reset") {
                    result.fg_reset = true;

                } else if (auto fg_rgb_color = parse_rgb(spec); !fg_rgb_color.empty()) {
                    if (!result.fg_color.empty()) throw colorize_error("multiple colors must not be specified");
                    result.fg_color = fg_rgb_color;

                } else if (auto fg_color = detail::name_to_color(spec); !fg_color.empty()) {
                    if (!result.fg_color.empty()) throw colorize_error("multiple colors must not be specified");
                    result.fg_color = fg_color;

                } else {
                    throw colorize_error("fg prefix must precede color name or reset");
                }

            } else if (specifier.starts_with("bg:")) {
                auto const spec = specifier.substr(3);

                if (spec == "reset") {
                    result.bg_reset = true;

                } else if (auto bg_rgb_color = parse_rgb(spec); !bg_rgb_color.empty()) {
                    if (!result.bg_color.empty()) throw colorize_error("multiple colors must not be specified");
                    result.bg_color = bg_rgb_color;

                } else if (auto bg_color = detail::name_to_color(spec); !bg_color.empty()) {
                    if (!result.bg_color.empty()) throw colorize_error("multiple colors must not be specified");
                    result.bg_color = bg_color;

                } else {
                    throw colorize_error("bg prefix must precede color name or reset");
                }

            } else {
                throw colorize_error("invalid specifier");
            }

            pc.advance_to(it += len);
        }

        if (result.reset && (!result.fg_color.empty() || result.emphasis != detail::emphasis{})) {
            throw colorize_error("reset must be independently specified");
        }

        return result;
    }

    detail::color fg_color_;
    detail::color bg_color_;
    detail::emphasis emphasis_{};
    bool reset_ = true;
    bool fg_reset_ = true;
    bool bg_reset_ = true;
};

namespace detail {

template<class CharT>
struct scanner
{
    using iterator = basic_colorize_parse_context<CharT>::iterator;

    constexpr scanner(std::basic_string_view<CharT> str) noexcept
        : pc(str)
    {}

    constexpr virtual ~scanner() {}

    constexpr void scan()
    {
        std::basic_string_view<CharT> fmt{pc.begin(), pc.end()};
        auto left_bracket = fmt.find('[');
        auto right_bracket = fmt.find(']');

        while (fmt.size()) {
            auto comp = left_bracket <=> right_bracket;
            if (comp == 0) {
                // no brackets found, consuming rest
                this->on_chars(pc.end());
                pc.advance_to(pc.end());
                return;
            }
            if (comp < 0) {
                // normal case
                if (left_bracket + 1 == fmt.size() || (right_bracket == fmt.npos && fmt[left_bracket + 1] == '[')) {
                throw colorize_error("unmatched left bracket");
            }

            bool const is_escape = fmt[left_bracket + 1] == '[';
            auto const last = pc.begin() + left_bracket + is_escape;
            this->on_chars(last);
            pc.advance_to(last + 1);
            fmt = {pc.begin(), pc.end()};

            if (is_escape) {
                if (right_bracket != fmt.npos) right_bracket -= left_bracket + 2;
                    left_bracket = fmt.find('[');

                } else {
                    this->on_replacement_field();
                    fmt = {pc.begin(), pc.end()};
                    left_bracket = fmt.find('[');
                    right_bracket = fmt.find(']');
                }

            } else {
                if (++right_bracket == fmt.size() || fmt[right_bracket] != ']') {
                    throw colorize_error("unmatched right bracket");
                }

                auto last = pc.begin() + right_bracket;
                this->on_chars(last);
                pc.advance_to(last + 1);
                fmt = {pc.begin(), pc.end()};
                if (left_bracket != fmt.npos) left_bracket -= right_bracket + 1;
                right_bracket = fmt.find(']');
            }
        }
    }

    constexpr void on_replacement_field()
    {
        auto next = pc.begin();
        if (*next == ']') {
            throw colorize_error("empty color specifier");
        }

        this->colorize();

        if (pc.begin() == pc.end() || *pc.begin() != ']') {
            throw colorize_error("unmatched left brace");
        }
        pc.advance_to(pc.begin() + 1);
    }

    constexpr virtual void on_chars(iterator) {}
    constexpr virtual void colorize() = 0;

    basic_colorize_parse_context<CharT> pc;
};

template<class CharT>
struct checking_scanner : scanner<CharT>
{
    constexpr checking_scanner(std::basic_string_view<CharT> str)
        : scanner<CharT>(str)
    {}

    constexpr void colorize() override { this->parse_colorize_spec(); }

    constexpr void parse_colorize_spec()
    {
        colorizer<CharT> c;
        this->pc.advance_to(c.parse(this->pc));
    }
};

template<class Out, class CharT>
struct noop_scanner : checking_scanner<CharT>
{
    using iterator = checking_scanner<CharT>::iterator;

    constexpr noop_scanner(basic_colorize_context<Out, CharT>& cc, std::basic_string_view<CharT> str)
        : checking_scanner<CharT>(str), cc(cc)
    {
    }

    constexpr void on_chars(iterator last) override
    {
        std::basic_string_view<CharT> str(this->pc.begin(), last);
        cc.advance_to(std::ranges::copy(str, cc.out()).out);
    }

private:
    basic_colorize_context<Out, CharT>& cc;
};

template<class Out, class CharT>
struct colorizing_scanner : scanner<CharT>
{
    using iterator = scanner<CharT>::iterator;

    constexpr colorizing_scanner(basic_colorize_context<Out, CharT>& cc, std::basic_string_view<CharT> str)
        : scanner<CharT>(str), cc(cc)
    {
    }

    constexpr void on_chars(iterator last) override
    {
        std::basic_string_view<CharT> str(this->pc.begin(), last);
        cc.advance_to(std::ranges::copy(str, cc.out()).out);
    }

    constexpr void colorize() override
    {
        colorizer<CharT> c;
        this->pc.advance_to(c.parse(this->pc));
        cc.advance_to(c.colorize(cc));
    }

    basic_colorize_context<Out, CharT>& cc;
};


template<class CharT>
struct basic_dynamic_colorized_string
{
    constexpr explicit basic_dynamic_colorized_string(std::basic_string_view<CharT> str)
        : str_(str)
    {
        checking_scanner<CharT> scanner(str);
        scanner.scan();
    }

    std::basic_string_view<CharT> str_;
};

using dynamic_colorized_string = basic_dynamic_colorized_string<char>;

#if __cpp_lib_format >= 202311L

template<class CharT>
struct basic_dynamic_colorized_format_string
{
    constexpr explicit basic_dynamic_colorized_format_string(std::basic_string_view<CharT> str)
        : str_(str)
    {
        checking_scanner<CharT> scanner(str);
        scanner.scan();
    }

    std::basic_string_view<CharT> str_;
};

using dynamic_colorized_format_string = basic_dynamic_colorized_format_string<char>;

#endif


template<class CharT>
struct counting_iterator
{
    using difference_type = std::ptrdiff_t;

    std::size_t count = 0;

    struct proxy
    {
        counting_iterator* ptr;
        constexpr void operator=(CharT) const { ++ptr->count; }
    };

    constexpr counting_iterator& operator++() { return *this; }
    constexpr void operator++(int) const {}
    constexpr proxy operator*() { return proxy{this}; }
};

} // detail

template<class CharT, class... Args>
struct basic_colorized_format_string
{
    // TODO: GCC emits ICE
    consteval basic_colorized_format_string(StringLike auto const& str)
        : fmt_(str)
    {
        detail::checking_scanner<CharT> scanner(str);
        scanner.scan();
    }

#if __cpp_lib_format >= 202311L
    explicit constexpr basic_colorize_format_string(detail::basic_dynamic_colorized_format_string<CharT> dynamic_str)
        : fmt_(std::runtime_format(dynamic_str.str_))
    {
    }
#endif

    [[nodiscard]] constexpr std::basic_format_string<CharT, Args...> get() const noexcept
    {
        return fmt_;
    }

private:
    std::basic_format_string<CharT, Args...> fmt_;
};

template<class... Args>
using colorized_format_string = basic_colorized_format_string<char, std::type_identity_t<Args>...>;

template<class CharT>
struct basic_colorized_string_view
{
    // TODO: This should be consteval, but GCC emits ICE
    constexpr basic_colorized_string_view(StringLike auto const& str)
        : str_(str)
    {
        detail::checking_scanner<CharT> scanner(str);
        scanner.scan();
    }

    constexpr basic_colorized_string_view(detail::basic_dynamic_colorized_string<CharT> runtime_str) noexcept
        : str_(runtime_str.str_)
    {
    }

    template<class... Args>
    constexpr basic_colorized_string_view(basic_colorized_format_string<CharT, Args...> fmt) noexcept
        : str_(fmt.get().get())
    {}

    [[nodiscard]] constexpr std::basic_string_view<CharT> get() const noexcept
    {
        return str_;
    }

private:
    std::basic_string_view<CharT> str_;
};

// --------------------------------------------------

using colorized_string_view = basic_colorized_string_view<char>;

template<class Out>
constexpr Out colorize_to(Out out, colorized_string_view col)
{
    basic_colorize_context<Out, char> ctx(std::move(out));
    detail::colorizing_scanner<Out, char> scanner(ctx, col.get());
    scanner.scan();
    return ctx.out();
}

template<int = 0>
[[nodiscard]] constexpr std::basic_string<char>
colorize(colorized_string_view col)
{
    std::basic_string<char> str;
    ansi_colorize::colorize_to(std::back_inserter(str), col);
    return str;
}

template<int = 0>
[[nodiscard]] constexpr std::size_t colorized_size(colorized_string_view col)
{
    return ansi_colorize::colorize_to(detail::counting_iterator<char>{}, col).count;
}


#if defined(__clang__) || _MSC_VER >= 1950 /* VS 2026 */

template<basic_fixed_string Str>
struct static_colorized_string
{
    using value_type = decltype(Str)::value_type;
    static constexpr std::size_t size = ansi_colorize::colorized_size(Str);

    static constexpr basic_fixed_string<value_type, size> colorized = []
    {
        basic_fixed_string<value_type, size> res;
        ansi_colorize::colorize_to(res.begin(), Str);
        return res;
    }();
};

inline namespace colorize_literals {

template<basic_fixed_string Str>
[[nodiscard]] consteval static_colorized_string<Str> operator""_col()
{
    return {};
}

} // colorize_literals

#endif // clang or VS >= 2026


template<int = 0>
[[nodiscard]] constexpr detail::dynamic_colorized_string
dynamic_colorize(std::string_view str)
{
    return detail::basic_dynamic_colorized_string{str};
}

#if __cpp_lib_format >= 202311L

template<int = 0>
[[nodiscard]] constexpr detail::dynamic_colorize_format_string
dynamic_colorize_format(std::string_view str)
{
    return detail::basic_dynamic_colorize_format_string{str};
}

#endif

#if defined(__clang__) || _MSC_VER >= 1950 /* VS 2026 */

template<basic_fixed_string Str, class... Args>
[[nodiscard]] constexpr std::string colorize_format(static_colorized_string<Str>, Args&&... args)
{
    return std::format(static_colorized_string<Str>::colorized, args...);
}

template<class Out, basic_fixed_string Str, class... Args>
constexpr Out colorize_format_to(Out out, static_colorized_string<Str>, Args&&... args)
{
    return std::format_to(std::move(out), static_colorized_string<Str>::colorized, args...);
}

#endif // clang or VS >= 2026


template<class... Args>
[[nodiscard]] constexpr std::string colorize_format(colorized_string_view str, Args&&... args)
{
#if __cpp_lib_format >= 202311L
    return std::format(std::runtime_format(ansi_colorize::colorize(str)), std::make_format_args(args...));
#else
    return std::vformat(ansi_colorize::colorize(str), std::make_format_args(args...));
#endif
}

template<class Out, class... Args>
constexpr Out colorize_format_to(Out out, colorized_string_view str, Args&&... args)
{
#if __cpp_lib_format >= 202311L
    return std::format_to(std::move(out), std::runtime_format(ansi_colorize::colorize(str)), args...);
#else
    return std::vformat_to(std::move(out), ansi_colorize::colorize(str), std::make_format_args(args...));
#endif
}

} // ansi_colorize


#if defined(__clang__) || _MSC_VER >= 1950 /* VS 2026 */

inline namespace colorize_literals {
using namespace ansi_colorize::colorize_literals;
} // colorize_literals

using ansi_colorize::static_colorized_string;

#endif // clang or VS >= 2026


using ansi_colorize::dynamic_colorize;

#if __cpp_lib_format >= 202311L
using ansi_colorize::dynamic_colorize_format;
#endif

using ansi_colorize::colorize;
using ansi_colorize::colorize_to;

using ansi_colorize::colorize_format;
using ansi_colorize::colorize_format_to;

} // iris

#endif

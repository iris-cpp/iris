#ifndef IRIS_ZZ_TEST_IRIS_IO_TEST_HPP
#define IRIS_ZZ_TEST_IRIS_IO_TEST_HPP

#include <ostream>

// Inject bad overloads
namespace {

namespace DirectNonStreamable_ns {

// Not streamable via ADL, but...
struct DirectNonStreamable {};

} // DirectNonStreamable_ns

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, DirectNonStreamable_ns::DirectNonStreamable&)
{
    return os << "polluted &";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, DirectNonStreamable_ns::DirectNonStreamable const&)
{
    return os << "polluted const&";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, DirectNonStreamable_ns::DirectNonStreamable&&)
{
    return os << "polluted &&";
}

// Bad global overload; can be avoided by the poison pill
[[maybe_unused]] std::ostream& operator<<(std::ostream& os, DirectNonStreamable_ns::DirectNonStreamable const&&)
{
    return os << "polluted const&&";
}

namespace TemplatedNonStreamable_ns {

// Not streamable via ADL, but...
struct TemplatedNonStreamable {};

} // TemplatedNonStreamable_ns

// Bad global overload; can be avoided by the poison pill
template<class CharT, class CharTraits>
[[maybe_unused]] std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, TemplatedNonStreamable_ns::TemplatedNonStreamable&)
{
    return os << "polluted &";
}

// Bad global overload; can be avoided by the poison pill
template<class CharT, class CharTraits>
[[maybe_unused]] std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, TemplatedNonStreamable_ns::TemplatedNonStreamable const&)
{
    return os << "polluted const&";
}

// Bad global overload; can be avoided by the poison pill
template<class CharT, class CharTraits>
[[maybe_unused]] std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, TemplatedNonStreamable_ns::TemplatedNonStreamable&&)
{
    return os << "polluted &&";
}

// Bad global overload; can be avoided by the poison pill
template<class CharT, class CharTraits>
[[maybe_unused]] std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, TemplatedNonStreamable_ns::TemplatedNonStreamable const&&)
{
    return os << "polluted const&&";
}

} // anonymous global

#endif

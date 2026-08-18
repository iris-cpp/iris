#ifndef IRIS_ZZ_NGRAM_ID_HPP
#define IRIS_ZZ_NGRAM_ID_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <format>
#include <utility>

#include <cstdint>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram {

// An external id that is always *stable* across document updates or removal.
enum struct document_id : std::uint32_t
{
    tombstone = static_cast<std::uint32_t>(-2),
};

namespace detail {

[[nodiscard]] constexpr std::size_t to_index(document_id doc_id) noexcept
{
    return static_cast<std::size_t>(doc_id);
}

} // detail

} // iris::ngram


namespace iris {

inline namespace ngram_literals {

[[nodiscard]] constexpr ngram::document_id operator ""_doc_id(unsigned long long id) noexcept
{
    return ngram::document_id{static_cast<std::underlying_type_t<ngram::document_id>>(id)};
}

} // ngram_literals

} // iris


template<class CharT>
struct std::formatter<iris::ngram::document_id, CharT>
    : std::formatter<std::underlying_type_t<iris::ngram::document_id>, CharT>
{
    using base_type = std::formatter<std::underlying_type_t<iris::ngram::document_id>, CharT>;

    template<class Ctx>
    Ctx::iterator format(iris::ngram::document_id doc_id, Ctx& ctx) const
    {
        return base_type::format(std::to_underlying(doc_id), ctx);
    }
};

#endif

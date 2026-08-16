#ifndef IRIS_ZZ_NGRAM_ID_HPP
#define IRIS_ZZ_NGRAM_ID_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>

#include <utility>

#include <cstdint>

namespace iris::ngram {

enum struct document_id : std::uint32_t {};

} // iris::ngram

namespace iris {

inline namespace ngram_literals {

[[nodiscard]] constexpr ngram::document_id operator ""_doc_id(unsigned long long id) noexcept
{
    return ngram::document_id{static_cast<std::underlying_type_t<ngram::document_id>>(id)};
}

} // ngram_literals

} // iris

#endif

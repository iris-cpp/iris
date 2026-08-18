#ifndef IRIS_ZZ_NGRAM_DETAIL_SLOT_HPP
#define IRIS_ZZ_NGRAM_DETAIL_SLOT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <cstdint>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram::detail {

// A monotonically increasing internal index used for bookkeeping.
// Obsolete documents and their posting data may still refer to this index (harmlessly)
// until `compact()` is requested on the database.
enum struct document_slot : std::uint32_t
{
    sentinel = static_cast<std::uint32_t>(-1),
    tombstone = static_cast<std::uint32_t>(-2),
};

[[nodiscard]] constexpr std::size_t to_index(document_slot doc_slot) noexcept
{
    return static_cast<std::size_t>(doc_slot);
}

} // iris::ngram::detail

#endif

#ifndef IRIS_ZZ_NGRAM_DETAIL_ID_STORE_HPP
#define IRIS_ZZ_NGRAM_DETAIL_ID_STORE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/detail/slot.hpp>
#include <iris/ngram/id.hpp>

#include <iris/exception.hpp>

#include <vector>
#include <span>
#include <limits>
#include <stdexcept>

#include <cassert>
#include <cstdint>
#include <cstddef>

namespace iris::ngram::detail {

class id_store
{
public:
    struct slot_info_t
    {
        document_id doc_id;
        bool is_visible = true;

        [[nodiscard]] bool is_stale() const noexcept { return doc_id == document_id::tombstone; }
        [[nodiscard]] bool is_used_for_search() const noexcept { return doc_id != document_id::tombstone && is_visible; }
    };

    [[nodiscard]] document_slot slot_for_new_document() const
    {
        if (id_to_slot_.size() >= to_index(document_id::tombstone)) {
            throw std::length_error{"no more document id can be issued"};
        }
        return this->next_slot();
    }

    [[nodiscard]] document_slot slot_for_update(document_id const doc_id) const
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the range of ids issued by this database", doc_id);
        }
        if (id_to_slot_[to_index(doc_id)] == document_slot::tombstone) {
            throwf<std::invalid_argument>("cannot update a removed document #{}", doc_id);
        }
        return this->next_slot();
    }

    // Requires the slot to be checked by `slot_for_new_document()`; `weight` is the length plus one
    document_id add_document(std::size_t const weight)
    {
        auto const doc_id = static_cast<document_id>(id_to_slot_.size());
        auto const doc_slot = static_cast<document_slot>(slot_infos_.size());
        id_to_slot_.emplace_back(doc_slot);
        slot_infos_.emplace_back(doc_id);
        this->add_slot_weight(weight);
        return doc_id;
    }

    void update_document(document_id const doc_id, std::size_t const weight)
    {
        document_slot& doc_slot = id_to_slot_[to_index(doc_id)];
        document_slot const old_slot = doc_slot;
        slot_info_t const info = this->get_info(old_slot);
        slot_infos_.emplace_back(info);
        this->add_slot_weight(weight);

        this->get_info(old_slot).doc_id = document_id::tombstone;
        this->kill_slot_weight(old_slot);
        doc_slot = static_cast<document_slot>(slot_infos_.size() - 1);
    }

    void remove_document(document_id const doc_id) noexcept
    {
        assert(to_index(doc_id) < id_to_slot_.size() && "document id is beyond the range of ids issued by this database");
        if (to_index(doc_id) >= id_to_slot_.size()) {
            return; // safely noop on Release build
        }

        document_slot& doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            return;
        }

        assert(to_index(doc_slot) < slot_infos_.size());
        auto& slot_info = slot_infos_[to_index(doc_slot)];

        assert(!slot_info.is_stale());
        slot_info.doc_id = document_id::tombstone;
        this->kill_slot_weight(doc_slot);

        doc_slot = document_slot::tombstone;
    }

    [[nodiscard]] bool has_document(document_id const doc_id) const noexcept
    {
        if (to_index(doc_id) >= id_to_slot_.size()) [[unlikely]] {
            return false;
        }
        document_slot const doc_slot = id_to_slot_[to_index(doc_id)];
        assert(doc_slot != document_slot::sentinel);
        return doc_slot != document_slot::tombstone;
    }

    [[nodiscard]] slot_info_t const& get_info(document_slot const doc_slot) const noexcept
    {
        assert(to_index(doc_slot) < slot_infos_.size());
        return slot_infos_[to_index(doc_slot)];
    }
    [[nodiscard]] slot_info_t& get_info(document_slot const doc_slot) noexcept
    {
        assert(to_index(doc_slot) < slot_infos_.size());
        return slot_infos_[to_index(doc_slot)];
    }

    [[nodiscard]] bool is_visible(document_id const doc_id) const
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the namespace acquired by the database", doc_id);
        }

        document_slot const doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            throwf<std::invalid_argument>("cannot fetch the visibility of already-removed document");
        }
        return this->get_info(doc_slot).is_visible;
    }

    void set_visible(document_id const doc_id, bool const flag)
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the namespace acquired by the database", doc_id);
        }

        document_slot const doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            throwf<std::invalid_argument>("cannot change the visibility of already-removed document");
        }
        this->get_info(doc_slot).is_visible = flag;
    }

    // Compact when the stale weight dominates, which amortizes the linear cost.
    // The minimum avoids frequent tiny rebuilds.
    [[nodiscard]] bool needs_compaction() const noexcept
    {
        return stale_weight_ >= min_compaction_weight && stale_weight_ > live_weight_;
    }

    // The new slot of each slot in the same order, or `document_slot::tombstone` for a stale one
    [[nodiscard]] std::vector<document_slot> compaction_map() const
    {
        std::vector<document_slot> slot_map;
        slot_map.reserve(slot_infos_.size());
        std::uint32_t new_slot = 0;
        for (auto const& info : slot_infos_) {
            slot_map.emplace_back(info.is_stale() ? document_slot::tombstone : static_cast<document_slot>(new_slot++));
        }
        return slot_map;
    }

    void compact(std::span<document_slot const> const slot_map) noexcept
    {
        assert(slot_map.size() == slot_infos_.size());

        std::size_t out = 0;
        for (std::size_t in = 0; in < slot_infos_.size(); ++in) {
            if (slot_map[in] == document_slot::tombstone) continue;
            assert(to_index(slot_map[in]) == out);
            slot_infos_[out] = slot_infos_[in];
            slot_weights_[out] = slot_weights_[in];
            id_to_slot_[to_index(slot_infos_[out].doc_id)] = slot_map[in];
            ++out;
        }
        slot_infos_.resize(out);
        slot_weights_.resize(out);
        stale_weight_ = 0;
    }

    void clear() noexcept
    {
        id_to_slot_.clear();
        slot_infos_.clear();
        slot_weights_.clear();
        live_weight_ = 0;
        stale_weight_ = 0;
    }

    void swap(id_store& other) noexcept
    {
        using std::swap;
        swap(id_to_slot_, other.id_to_slot_);
        swap(slot_infos_, other.slot_infos_);
        swap(slot_weights_, other.slot_weights_);
        swap(live_weight_, other.live_weight_);
        swap(stale_weight_, other.stale_weight_);
    }

private:
    // Stale documents below this weight hold at most about 128 KiB of postings and positions.
    // Below it, the fixed cost of the compaction was measured to be significant.
    static constexpr std::size_t min_compaction_weight = 4096;

    [[nodiscard]] document_slot next_slot() const
    {
        if (slot_infos_.size() >= to_index(document_slot::tombstone)) {
            throw std::length_error{"no more document slot is available"};
        }
        return static_cast<document_slot>(slot_infos_.size());
    }

    void add_slot_weight(std::size_t const weight)
    {
        assert(weight <= std::numeric_limits<std::uint32_t>::max());
        slot_weights_.emplace_back(static_cast<std::uint32_t>(weight));
        live_weight_ += weight;
    }

    void kill_slot_weight(document_slot const doc_slot) noexcept
    {
        std::size_t const weight = slot_weights_[to_index(doc_slot)];
        live_weight_ -= weight;
        stale_weight_ += weight;
    }

    std::vector<document_slot> id_to_slot_;
    std::vector<slot_info_t> slot_infos_;

    // Apart from `slot_infos_`, which searches access per posting
    std::vector<std::uint32_t> slot_weights_;
    std::size_t live_weight_ = 0;
    std::size_t stale_weight_ = 0;
};

inline void swap(id_store& a, id_store& b) noexcept
{
    a.swap(b);
}

} // iris::ngram::detail

#endif

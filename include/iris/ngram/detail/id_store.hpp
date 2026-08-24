#ifndef IRIS_ZZ_NGRAM_DETAIL_ID_STORE_HPP
#define IRIS_ZZ_NGRAM_DETAIL_ID_STORE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/detail/slot.hpp>
#include <iris/ngram/id.hpp>

#include <iris/exception.hpp>

#include <vector>

#include <cassert>

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

private:
    struct [[nodiscard]] add_document_transaction;
    friend add_document_transaction;
    struct add_document_transaction
    {
        add_document_transaction(id_store* store, document_id new_id, document_slot new_slot) noexcept
            : store_(store)
            , new_id_(new_id)
            , new_slot_(new_slot)
        {}

        [[nodiscard]] document_id new_id() const noexcept { return new_id_; }
        [[nodiscard]] document_slot new_slot() const noexcept { return new_slot_; }

        void commit()
        {
            store_->id_to_slot_.emplace_back(new_slot_);
            store_->slot_infos_.emplace_back(new_id_);
        }

    private:
        id_store* store_;
        document_id new_id_;
        document_slot new_slot_;
    };

public:
    add_document_transaction add_document()
    {
        return {
            this,
            static_cast<document_id>(id_to_slot_.size()),
            static_cast<document_slot>(slot_infos_.size())
        };
    }

private:
    struct [[nodiscard]] update_document_transaction;
    friend update_document_transaction;
    struct update_document_transaction
    {
        update_document_transaction(id_store* store, slot_info_t& old_slot_info, document_slot new_slot) noexcept
            : store_(store)
            , old_slot_info_(old_slot_info)
            , new_slot_(new_slot)
        {}

        [[nodiscard]] document_slot new_slot() const noexcept { return new_slot_; }

        void commit()
        {
            auto const doc_id = old_slot_info_.doc_id;
            old_slot_info_.doc_id = document_id::tombstone;
            store_->slot_infos_.emplace_back(doc_id, old_slot_info_.is_visible);
            store_->id_to_slot_[to_index(doc_id)] = new_slot_;
        }

    private:
        id_store* store_;
        slot_info_t& old_slot_info_;
        document_slot new_slot_;
    };

public:
    update_document_transaction update_document(document_id const doc_id)
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the range of ids issued by this database", doc_id);
        }

        document_slot const& doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            throwf<std::invalid_argument>("cannot update a removed document #{}", doc_id);
        }

        assert(to_index(doc_slot) < slot_infos_.size());
        auto& old_slot_info = slot_infos_[to_index(doc_slot)];
        assert(old_slot_info.doc_id == doc_id);

        return {
            this, old_slot_info, static_cast<document_slot>(slot_infos_.size())
        };
    }

    void remove_document(document_id const doc_id)
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the range of ids issued by this database", doc_id);
        }

        document_slot& doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            // Removing an already-removed document is no-op; same semantics as STL containers
            return;
        }

        assert(to_index(doc_slot) < slot_infos_.size());
        auto& slot_info = slot_infos_[to_index(doc_slot)];

        assert(!slot_info.is_stale());
        slot_info.doc_id = document_id::tombstone;

        doc_slot = document_slot::tombstone;
    }

    [[nodiscard]] bool has_document(document_id const doc_id) const noexcept
    {
        return detail::to_index(doc_id) < id_to_slot_.size();
    }

    // This method is intentionally `noexcept` and invokes only assertion because
    // it is heavily accessed via actual lookup
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
        return get_info(doc_slot).is_visible;
    }

    void set_visible(document_id const doc_id, bool flag)
    {
        if (to_index(doc_id) >= id_to_slot_.size()) {
            throwf<std::invalid_argument>("document id #{} is beyond the namespace acquired by the database", doc_id);
        }

        document_slot const doc_slot = id_to_slot_[to_index(doc_id)];
        if (doc_slot == document_slot::tombstone) {
            throwf<std::invalid_argument>("cannot change the visibility of already-removed document");
        }
        get_info(doc_slot).is_visible = flag;
    }

    void clear() noexcept
    {
        id_to_slot_.clear();
        slot_infos_.clear();
    }

private:
    std::vector<document_slot> id_to_slot_;
    std::vector<slot_info_t> slot_infos_;
};

} // iris::ngram::detail

#endif

#ifndef IRIS_ZZ_NGRAM_KEYED_DATABASE_HPP
#define IRIS_ZZ_NGRAM_KEYED_DATABASE_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/database.hpp>
#include <iris/ngram/id.hpp>

#include <iris/hash.hpp>

#include <unordered_map>
#include <stdexcept>
#include <memory>

namespace iris::ngram {

template<class KeyT, class HashT = std::hash<KeyT>, class EqualT = std::equal_to<>, class CharT = char32_t>
class keyed_database : private database<CharT>
{
    using base_type = database<CharT>;
    friend base_type;

public:
    using document_id_type = KeyT;

    template<class KeyLikeT>
        requires std::is_constructible_v<KeyT, KeyLikeT>
    void add_document(KeyLikeT&& key_like, std::basic_string_view<CharT> const doc_text)
    {
        auto const doc_id = base_type::add_document(doc_text);
        auto const [it, inserted] = key_to_doc_id_.emplace(std::forward<KeyLikeT>(key_like), doc_id);
        if (!inserted) {
            if constexpr (std::is_pointer_v<KeyT>) {
                throwf<std::invalid_argument>("key `{}` already exists in keyed_database", static_cast<void const*>(it->first));
            } else {
                throwf<std::invalid_argument>("key `{}` already exists in keyed_database", it->first);
            }
        }
        assert(doc_id_to_key_.size() == detail::to_index(doc_id));
        doc_id_to_key_.emplace_back(std::addressof(it->first));
    }

    template<class KeyLikeT>
    void update_document(KeyLikeT const& key_like, std::basic_string_view<CharT> const doc_text)
    {
        base_type::update_document(this->get_document_id(key_like), doc_text);
    }

    template<class KeyLikeT>
        requires std::is_constructible_v<KeyT, KeyLikeT>
    void add_or_update_document(KeyLikeT&& key_like, std::basic_string_view<CharT> const doc_text)
    {
        KeyT key{std::forward<KeyLikeT>(key_like)};
        auto const it = key_to_doc_id_.find(key);

        if (it == key_to_doc_id_.end()) {
            this->add_document(std::move(key), doc_text);
        } else {
            base_type::update_document(it->second, doc_text);
        }
    }

    template<class KeyLikeT>
    void remove_document(KeyLikeT const& key_like)
    {
        base_type::remove_document(this->get_document_id(key_like));
    }

    template<class KeyLikeT>
    [[nodiscard]] bool has_document(KeyLikeT const& key_like) const
    {
        return key_to_doc_id_.find(key_like) != key_to_doc_id_.end();
    }

    template<class KeyLikeT>
    [[nodiscard]] bool is_visible(KeyLikeT const& key_like) const
    {
        return base_type::is_visible(this->get_document_id(key_like));
    }

    template<class KeyLikeT>
    void set_visible(KeyLikeT const& key_like, bool const flag)
    {
        base_type::set_visible(this->get_document_id(key_like), flag);
    }

    void clear() noexcept
    {
        base_type::clear();
        key_to_doc_id_.clear();
        doc_id_to_key_.clear();
    }

    using base_type::search;

private:
    [[nodiscard]] KeyT const& make_document_id(document_id doc_id) const noexcept
    {
        assert(detail::to_index(doc_id) < doc_id_to_key_.size());
        return *doc_id_to_key_[detail::to_index(doc_id)];
    }

    template<class KeyLikeT>
    [[nodiscard]] document_id get_document_id(KeyLikeT const& key_like) const
    {
        // TODO: __cpp_lib_associative_heterogeneous_insertion
        auto const it = key_to_doc_id_.find(key_like);
        if (it == key_to_doc_id_.end()) throw std::out_of_range{"key not found"};
        return it->second;
    }

    std::unordered_map<KeyT, document_id, HashT, EqualT>
    key_to_doc_id_;

    std::vector<KeyT const*>
    doc_id_to_key_;
};

} // iris::ngram

#endif

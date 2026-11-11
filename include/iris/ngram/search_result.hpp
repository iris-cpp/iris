#ifndef IRIS_ZZ_NGRAM_SEARCH_RESULT_HPP
#define IRIS_ZZ_NGRAM_SEARCH_RESULT_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp> // IWYU pragma: keep

#include <iris/ngram/id.hpp>

#include <iris/interval.hpp>
#include <iris/container_traits.hpp>

#include <span>
#include <iterator>
#include <vector>
#include <limits>
#include <stdexcept>
#include <compare>
#include <utility>

#include <cassert>
#include <cstdint>
#include <cstddef> // IWYU pragma: keep

namespace iris::ngram {

template<class CharT>
class database;

template<class DocumentID>
class search_result;

namespace detail {

template<class DocumentID>
class word_spans
{
public:
    class iterator
    {
    public:
        using value_type = std::span<interval<int> const>;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        [[nodiscard]] value_type operator*() const noexcept
        {
            auto const& offsets = result_->span_offsets_;
            auto const i = static_cast<std::size_t>(offset_);
            return std::span{result_->spans_}.subspan(offsets[i], offsets[i + 1] - offsets[i]);
        }

        [[nodiscard]] value_type operator[](difference_type const n) const noexcept
        {
            return *(*this + n);
        }

        iterator& operator++() noexcept
        {
            ++offset_;
            return *this;
        }

        iterator operator++(int) noexcept
        {
            auto const it = *this;
            ++offset_;
            return it;
        }

        iterator& operator--() noexcept
        {
            --offset_;
            return *this;
        }

        iterator operator--(int) noexcept
        {
            auto const it = *this;
            --offset_;
            return it;
        }

        iterator& operator+=(difference_type const n) noexcept
        {
            offset_ += n;
            return *this;
        }

        iterator& operator-=(difference_type const n) noexcept
        {
            offset_ -= n;
            return *this;
        }

        [[nodiscard]] friend iterator operator+(iterator it, difference_type const n) noexcept { return it += n; }
        [[nodiscard]] friend iterator operator+(difference_type const n, iterator it) noexcept { return it += n; }
        [[nodiscard]] friend iterator operator-(iterator it, difference_type const n) noexcept { return it -= n; }

        [[nodiscard]] friend difference_type operator-(iterator const& a, iterator const& b) noexcept
        {
            return a.offset_ - b.offset_;
        }

        [[nodiscard]] friend bool operator==(iterator const& a, iterator const& b) noexcept
        {
            return a.offset_ == b.offset_;
        }

        [[nodiscard]] friend std::strong_ordering operator<=>(iterator const& a, iterator const& b) noexcept
        {
            return a.offset_ <=> b.offset_;
        }

    private:
        friend word_spans;

        iterator(search_result<DocumentID> const* const result, std::size_t const offset) noexcept
            : result_(result)
            , offset_(static_cast<difference_type>(offset))
        {}

        search_result<DocumentID> const* result_ = nullptr;
        difference_type offset_ = 0;
    };

    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    word_spans() = default;

    [[nodiscard]] std::span<interval<int> const> operator[](std::size_t const word) const noexcept
    {
        assert(word < this->size());
        return this->begin()[static_cast<std::ptrdiff_t>(word)];
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return result_ ? result_->word_count_ : 0;
    }

    [[nodiscard]] iterator begin() const noexcept
    {
        return {result_, doc_ * this->size()};
    }

    [[nodiscard]] iterator end() const noexcept
    {
        return {result_, (doc_ + 1) * this->size()};
    }

    [[nodiscard]] reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator{this->end()};
    }

    [[nodiscard]] reverse_iterator rend() const noexcept
    {
        return reverse_iterator{this->begin()};
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return this->begin();
    }

    [[nodiscard]] const_iterator cend() const noexcept
    {
        return this->end();
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept
    {
        return this->rbegin();
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept
    {
        return this->rend();
    }

private:
    friend search_result<DocumentID>;

    word_spans(search_result<DocumentID> const* const result, std::size_t const doc) noexcept
        : result_(result)
        , doc_(doc)
    {}

    search_result<DocumentID> const* result_ = nullptr;
    std::size_t doc_ = 0;
};

} // detail

template<class DocumentID = document_id>
class [[nodiscard]] search_result
{
public:
    using document_id_type = DocumentID;
    using word_spans = detail::word_spans<DocumentID>;
    using value_type = std::pair<DocumentID const&, word_spans>;

    class iterator
    {
    public:
        using value_type = search_result::value_type;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        [[nodiscard]] value_type operator*() const noexcept
        {
            return (*result_)[static_cast<std::size_t>(doc_)];
        }

        [[nodiscard]] value_type operator[](difference_type const n) const noexcept
        {
            return *(*this + n);
        }

        iterator& operator++() noexcept
        {
            ++doc_;
            return *this;
        }

        iterator operator++(int) noexcept
        {
            auto const it = *this;
            ++doc_;
            return it;
        }

        iterator& operator--() noexcept
        {
            --doc_;
            return *this;
        }

        iterator operator--(int) noexcept
        {
            auto const it = *this;
            --doc_;
            return it;
        }

        iterator& operator+=(difference_type const n) noexcept
        {
            doc_ += n;
            return *this;
        }

        iterator& operator-=(difference_type const n) noexcept
        {
            doc_ -= n;
            return *this;
        }

        [[nodiscard]] friend iterator operator+(iterator it, difference_type const n) noexcept { return it += n; }
        [[nodiscard]] friend iterator operator+(difference_type const n, iterator it) noexcept { return it += n; }
        [[nodiscard]] friend iterator operator-(iterator it, difference_type const n) noexcept { return it -= n; }

        [[nodiscard]] friend difference_type operator-(iterator const& a, iterator const& b) noexcept
        {
            return a.doc_ - b.doc_;
        }

        [[nodiscard]] friend bool operator==(iterator const& a, iterator const& b) noexcept
        {
            return a.doc_ == b.doc_;
        }

        [[nodiscard]] friend std::strong_ordering operator<=>(iterator const& a, iterator const& b) noexcept
        {
            return a.doc_ <=> b.doc_;
        }

    private:
        friend search_result;

        iterator(search_result const* const result, std::size_t const doc) noexcept
            : result_(result)
            , doc_(static_cast<difference_type>(doc))
        {}

        search_result const* result_ = nullptr;
        difference_type doc_ = 0;
    };

    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    search_result() = default;
    search_result(search_result const&) = default;
    search_result(search_result&&) = default;
    search_result& operator=(search_result&&) = default;

    search_result& operator=(search_result const& other)
    {
        search_result copy(other);
        this->swap(copy);
        return *this;
    }

    [[nodiscard]] value_type operator[](std::size_t const doc) const noexcept IRIS_LIFETIMEBOUND
    {
        assert(doc < this->size());
        return {doc_ids_[doc], word_spans{this, doc}};
    }

    [[nodiscard]] iterator begin() const noexcept IRIS_LIFETIMEBOUND
    {
        return {this, 0};
    }

    [[nodiscard]] iterator end() const noexcept IRIS_LIFETIMEBOUND
    {
        return {this, this->size()};
    }

    [[nodiscard]] reverse_iterator rbegin() const noexcept IRIS_LIFETIMEBOUND
    {
        return reverse_iterator{this->end()};
    }

    [[nodiscard]] reverse_iterator rend() const noexcept IRIS_LIFETIMEBOUND
    {
        return reverse_iterator{this->begin()};
    }

    [[nodiscard]] const_iterator cbegin() const noexcept IRIS_LIFETIMEBOUND
    {
        return this->begin();
    }

    [[nodiscard]] const_iterator cend() const noexcept IRIS_LIFETIMEBOUND
    {
        return this->end();
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept IRIS_LIFETIMEBOUND
    {
        return this->rbegin();
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept IRIS_LIFETIMEBOUND
    {
        return this->rend();
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return doc_ids_.size();
    }

    void clear() noexcept
    {
        word_count_ = 0;
        doc_ids_.clear();
        span_offsets_.clear();
        spans_.clear();
    }

    void swap(search_result& other) noexcept
    {
        using std::swap;
        swap(word_count_, other.word_count_);
        swap(doc_ids_, other.doc_ids_);
        swap(span_offsets_, other.span_offsets_);
        swap(spans_, other.spans_);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return doc_ids_.empty();
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return !this->empty();
    }

private:
    template<class CharT>
    friend class database;

    friend word_spans;

    void reset(std::size_t const word_count) noexcept
    {
        this->clear();
        word_count_ = word_count;
    }

    void reserve_documents(std::size_t const doc_count)
    {
        container::reserve_geometric(doc_ids_, doc_count);
        container::reserve_geometric(span_offsets_, doc_count * word_count_ + 1);
    }

    // Must be followed by `add_word_spans()` or `add_word_positions()` for every word
    template<class DocumentIDLike>
    void add_document(DocumentIDLike&& doc_id)
    {
        if (span_offsets_.empty()) {
            span_offsets_.emplace_back(0);
        }
        doc_ids_.emplace_back(std::forward<DocumentIDLike>(doc_id));
    }

    void add_word_spans(std::span<interval<int> const> const spans)
    {
        if (spans.size() > std::numeric_limits<std::uint32_t>::max() - spans_.size()) {
            throw std::length_error{"too many spans in the search result"};
        }
        container::reserve_geometric(spans_, spans_.size() + spans.size());
        spans_.insert(spans_.end(), spans.begin(), spans.end());
        span_offsets_.emplace_back(static_cast<std::uint32_t>(spans_.size()));
    }

    void add_word_positions(std::span<int const> const positions, int const length)
    {
        if (positions.size() > std::numeric_limits<std::uint32_t>::max() - spans_.size()) {
            throw std::length_error{"too many spans in the search result"};
        }
        int end = std::numeric_limits<int>::min();
        for (int const pos : positions) {
            if (pos < end) continue;
            end = pos + length;
            spans_.emplace_back(pos, end);
        }
        span_offsets_.emplace_back(static_cast<std::uint32_t>(spans_.size()));
    }

    std::size_t word_count_ = 0;
    std::vector<DocumentID> doc_ids_;

    // `[doc * word_count_ + word]` begins the spans of the word in the document; one extra element at the end
    std::vector<std::uint32_t> span_offsets_;

    std::vector<interval<int>> spans_;
};

template<class DocumentID>
void swap(search_result<DocumentID>& a, search_result<DocumentID>& b) noexcept
{
    a.swap(b);
}

} // iris::ngram

#endif

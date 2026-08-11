#ifndef IRIS_NGRAM_HPP
#define IRIS_NGRAM_HPP

// SPDX-License-Identifier: MIT

#include <iris/config.hpp>
#include <iris/interval.hpp>
#include <iris/string_algo.hpp>
#include <iris/unicode/string.hpp>
#include <iris/format.hpp>

#include <concepts>
#include <span>
#include <flat_map>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <compare>
#include <stdexcept>

#include <cassert>

namespace iris {

enum struct ngram_document_id : unsigned {};

struct ngram_occurrence
{
    ngram_document_id doc_id;
    int pos;

    [[nodiscard]] constexpr bool operator==(ngram_occurrence const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram_occurrence const&) const noexcept = default;
};

} // iris

template<class CharT>
struct std::formatter<iris::ngram_document_id, CharT>
    : std::formatter<std::underlying_type_t<iris::ngram_document_id>, CharT>
{
    using base_type = std::formatter<std::underlying_type_t<iris::ngram_document_id>, CharT>;

    template<class Ctx>
    Ctx::iterator format(iris::ngram_document_id doc_id, Ctx& ctx) const
    {
        return base_type::format(std::to_underlying(doc_id), ctx);
    }
};

template<class CharT>
struct std::formatter<iris::ngram_occurrence, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram_occurrence const& occ, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}", occ.doc_id, occ.pos);
    }
};

namespace iris {

namespace detail {

inline constexpr std::size_t N_GRAM_MAX_OPTIMIZED_N = 2;

} // detail

template<std::size_t N, class CharT>
struct ngram
{
    static_assert(N >= 1);

    // TODO: optimize for N=1
    std::array<CharT, N> chars;

    template<std::forward_iterator It>
    [[nodiscard]] static constexpr ngram from_copy_n(It it)
        noexcept(noexcept(*it++))
    {
        if constexpr (N == 1) {
            ngram ng;
            ng.chars[0] = *it;
            return ng;
        } else if constexpr (N == 2) {
            ngram ng;
            ng.chars[0] = *it++;
            ng.chars[1] = *it;
            return ng;
        } else {
            static_assert(detail::N_GRAM_MAX_OPTIMIZED_N == 2);
            ngram ng;
            std::ranges::copy_n(it, N, ng.chars.begin());
            return ng;
        }
    }

    template<std::size_t Len>
    [[nodiscard]] static constexpr ngram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        assert(chars[Len - 1] == static_cast<CharT>(0));
        return ngram::from_copy_n(std::ranges::begin(chars));
    }

    [[nodiscard]] constexpr bool operator==(ngram const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram const&) const noexcept = default;
};

inline namespace ngram_literals {

[[nodiscard]] constexpr ngram_document_id operator ""_doc_id(unsigned long long id) noexcept
{
    return ngram_document_id{static_cast<std::underlying_type_t<ngram_document_id>>(id)};
}

[[nodiscard]] constexpr auto operator ""_2gram(char32_t const* str, std::size_t len) noexcept
{
    assert(len == 2);
    (void)len;
    return ngram<2, char32_t>{str[0], str[1]};
}

[[nodiscard]] constexpr auto operator ""_1gram(char32_t const* str, std::size_t len) noexcept
{
    assert(len == 1);
    (void)len;
    return ngram<1, char32_t>{str[0]};
}

} // ngram_literals

template<class CharT, std::size_t N>
[[nodiscard]] ngram<N - 1, CharT> to_ngram(CharT const (&chars)[N]) noexcept
{
    return ngram<N - 1, CharT>::from_c_array(chars);
}


namespace detail {

enum struct [[nodiscard]] search_continuation : bool
{
    abort = false,
    proceed = true,
};

struct ngram_posting
{
    ngram_document_id doc_id;
    unsigned pos_offset = 0;
    unsigned pos_count = 0;
};

struct ngram_posting_list
{
    std::vector<ngram_posting> postings;
    std::vector<int> positions;

    void append(ngram_document_id const doc_id, int pos)
    {
        if (postings.empty() || postings.back().doc_id != doc_id) {
            if (!postings.empty() && postings.back().doc_id > doc_id) {
                throw std::invalid_argument{"documents must be indexed in non-decreasing order of document ID"};
            }
            postings.emplace_back(
                doc_id,
                static_cast<unsigned>(positions.size()),
                0
            );
        }
        ++postings.back().pos_count;
        positions.emplace_back(pos);
    }

    void to_occurrence_list(std::vector<ngram_occurrence>& occs) const
    {
        occs.clear();
        for (auto const& posting : postings) {
            for (std::size_t i = posting.pos_offset; i < posting.pos_offset + posting.pos_count; ++i) {
                occs.emplace_back(posting.doc_id, positions[i]);
            }
        }
    }

    template<class F>
    void for_each_documents(F&& f) const
    {
        static_assert(std::invocable<F, ngram_document_id, std::span<int const>>);

        constexpr bool f_returns_continuation = std::same_as<
            std::invoke_result_t<F, ngram_document_id, std::span<int const>>,
            search_continuation
        >;

        for (auto const& posting : postings) {
            std::span<int const> const posting_span{
                positions.begin() + posting.pos_offset,
                static_cast<std::size_t>(posting.pos_count)
            };

            if constexpr (f_returns_continuation) {
                search_continuation const cont = f(posting.doc_id, posting_span);
                if (cont == search_continuation::abort) break;
            } else {
                f(posting.doc_id, posting_span);
            }
        }
    }
};

template<std::size_t N, class CharT, class PostingListT = ngram_posting_list>
struct ngram_index
{
    void append(ngram<N, CharT> const ng, ngram_document_id const doc_id, int const pos)
    {
        gram_entries[ng].append(doc_id, pos);
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return gram_entries.empty();
    }

    void find_occurrences(ngram<N, CharT> const ng, std::vector<ngram_occurrence>& occs) const
    {
        occs.clear();
        auto const it = gram_entries.find(ng);
        if (it == gram_entries.end()) return;

        it->second.to_occurrence_list(occs);
    }

    template<class F>
    void search(ngram<N, CharT> const ng, F&& f) const
    {
        auto const it = gram_entries.find(ng);
        if (it == gram_entries.end()) return;
        it->second.for_each_documents(f);
    }

    std::flat_map<ngram<N, CharT>, PostingListT> gram_entries;
};


template<class CharT, class PostingListT = ngram_posting_list>
struct ngram_index_storage
{
    ngram_index<1, CharT, PostingListT> uni_idx;
    ngram_index<2, CharT, PostingListT> bi_idx;

    template<std::size_t N>
    [[nodiscard]] auto& get_index(this auto& self) noexcept
    {
        if constexpr (N == 1) {
            return self.uni_idx;
        } else if constexpr (N == 2) {
            return self.bi_idx;
        } else {
            static_assert(N_GRAM_MAX_OPTIMIZED_N == 2);
        }
    }

    void append_index(ngram_document_id const doc_id, std::basic_string_view<CharT> const input)
    {
        this->template append_index<1>(doc_id, this->template get_index<1>(), input);
        this->template append_index<2>(doc_id, this->template get_index<2>(), input);
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return uni_idx.empty() && bi_idx.empty();
    }

    template<std::size_t N, class F>
    void search(ngram<N, CharT> const ng, F&& f) const
    {
        this->template get_index<N>().search(ng, f);
    }

private:
    template<std::size_t N>
    void append_index(
        ngram_document_id const doc_id,
        ngram_index<N, CharT, PostingListT>& idx,
        std::basic_string_view<CharT> const input
    )
    {
        if (input.size() < N) return;

        if constexpr (N == 1) {
            for (std::size_t i = 0; i < input.size(); ++i) {
                idx.append(ngram<1, CharT>{input[i]}, doc_id, int(i));
            }

        } else {
            auto ng = ngram<N, CharT>::from_copy_n(input.begin());
            idx.append(ng, doc_id, 0);

            for (std::size_t i = N; i < input.size(); ++i) {
                std::shift_left(ng.chars.begin(), ng.chars.end(), 1);
                ng.chars[N - 1] = input[i];
                idx.append(ng, doc_id, int(i - N + 1));
            }
        }
    }
};

} // detail


template<class CharT = char32_t>
struct ngram_search_query
{
    explicit ngram_search_query(std::basic_string_view<CharT> input_sv)
    {
        std::basic_string<CharT> input{input_sv};
        iris::compact_spaces(input);
        if (input.empty()) return;

        words_ = input
            | std::views::split(detail::string_algo_traits<CharT>::space)
            | std::views::transform([](auto const& r) {
                return std::basic_string<CharT>{std::from_range, r};
            })
            | std::ranges::to<std::vector>();

        std::ranges::sort(words_);
        {
            auto const [first, last] = std::ranges::unique(words_);
            words_.erase(first, last);
        }
    }

    // ------------------------------------------

    [[nodiscard]]
    auto const& words() const noexcept
    {
        return words_;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return words_.empty();
    }

    [[nodiscard]] bool operator==(ngram_search_query const& other) const noexcept
    {
        return words_ == other.words_;
    }

private:
    std::vector<std::basic_string<CharT>> words_;
};

} // iris

template<class NGCharT, class CharT>
struct std::formatter<iris::ngram_search_query<NGCharT>, CharT>
    : iris::no_spec_formatter<CharT>
{
    template<class Ctx>
    Ctx::iterator format(iris::ngram_search_query<NGCharT> const& query, Ctx& ctx) const
    {
        return std::format_to(ctx.out(), "{}", query.words() | std::views::transform([](std::u32string_view ustr) {
            return iris::unicode::transcode<char>(ustr);
        }));
    }
};


namespace iris {

class [[nodiscard]] ngram_search_result
{
    struct word_matches_t
    {
        int word_id = 0;
        std::vector<interval<int>> matches;
    };

    using doc_matches_map = std::flat_map<ngram_document_id, std::vector<word_matches_t>>;

    struct word_matches_handle
    {
        doc_matches_map::iterator map_it;
        std::vector<interval<int>>* word_matches = nullptr;

        [[nodiscard]]
        std::vector<interval<int>>* operator->() const noexcept
        {
            return word_matches;
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return word_matches;
        }
    };

public:
    [[nodiscard]]
    bool has_document(ngram_document_id const doc_id) const noexcept
    {
        return doc_matches_.contains(doc_id);
    }

    [[nodiscard]]
    auto const& doc_matches() const noexcept { return doc_matches_; }

    // Returns whether search must continue
    template<std::size_t N>
    [[nodiscard]]
    bool init_word_matches(ngram_document_id const doc_id, int const word_id, std::span<int const> const positions)
    {
        auto doc_matches_it = doc_matches_.find(doc_id);
        if (doc_matches_it == doc_matches_.end()) {
            if (word_id != 0) return false;
            doc_matches_it = doc_matches_.try_emplace(doc_id).first;
        }

        auto& word_matches = doc_matches_it->second.emplace_back(word_id);
        word_matches.matches.assign_range(positions | std::views::transform([](int const pos) -> interval<int> {
            return {pos, pos + static_cast<int>(N)};
        }));
        return true;
    }

    [[nodiscard]]
    word_matches_handle get_word_matches(ngram_document_id const doc_id, int const word_id)
    {
        auto const doc_matches_it = doc_matches_.find(doc_id);
        if (doc_matches_it == doc_matches_.end()) return {};

        auto const it = std::ranges::find(doc_matches_it->second, word_id, &word_matches_t::word_id);
        if (it == doc_matches_it->second.end()) return {};
        return {doc_matches_it, &it->matches};
    }

    void erase_word_matches(word_matches_handle const& handle)
    {
        doc_matches_.erase(handle.map_it);
    }

    void clear() noexcept
    {
        doc_matches_.clear();
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return doc_matches_.empty();
    }

    [[nodiscard]]
    explicit operator bool() const noexcept
    {
        return !this->empty();
    }

private:
    doc_matches_map doc_matches_;
};

template<class CharT = char32_t>
class ngram_database
{
public:
    [[nodiscard]]
    ngram_document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        ngram_document_id const doc_id{max_doc_id_};
        max_doc_id_ = ngram_document_id{std::to_underlying(max_doc_id_) + 1u};

        store_.append_index(doc_id, doc_text);
        return doc_id;
    }

    template<std::size_t N>
    void find_occurrences(ngram<N, CharT> ng, std::vector<ngram_occurrence>& occs) const noexcept
    {
        occs.clear();
        auto const& idx = store_.template get_index<N>();
        idx.find_occurrences(ng, occs);
    }

    [[nodiscard]]
    ngram_search_result search(ngram_search_query<CharT> const& query) const
    {
        if (query.empty()) return {};
        if (store_.empty()) return {};

        ngram_search_result search_res;

        int word_id = 0;
        auto it = query.words().begin();
        assert(!it->empty());
        this->search_word<true>(search_res, word_id++, *it++);
        if (search_res.empty()) return search_res;

        for (; it != query.words().end(); ++it) {
            assert(!it->empty());
            this->search_word<false>(search_res, word_id++, *it);
            if (search_res.empty()) break;
        }
        return search_res;
    }

private:
    template<bool IsFirstWord>
    void search_word(ngram_search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        if (word.empty()) return;

        if (word.size() == 1) {
            this->search_word_impl<IsFirstWord, 1>(search_res, word_id, word);
        } else {
            this->search_word_impl<IsFirstWord, 2>(search_res, word_id, word);
        }
    }

    template<bool IsFirstWord, std::size_t N>
    void search_word_impl(ngram_search_result& search_res, int const word_id, std::basic_string_view<CharT> const word) const
    {
        auto ng = ngram<N, CharT>::from_copy_n(word.begin());

        std::size_t available_doc_count = 0;
        store_.search(ng, [&](ngram_document_id const doc_id, std::span<int const> const positions) {
            if (search_res.init_word_matches<N>(doc_id, word_id, positions)) {
                ++available_doc_count;
            }
        });

        if constexpr (!IsFirstWord) {
            if (available_doc_count == 0) {
                search_res.clear();
                return;
            }
        }

        for (std::size_t i = N; i < word.size(); i += N) {
            std::ranges::copy_n(word.begin() + i, N, ng.chars.begin());

            store_.search(ng, [&](ngram_document_id const doc_id, std::span<int const> const positions) {
                auto word_matches = search_res.get_word_matches(doc_id, word_id);
                if (!word_matches) return detail::search_continuation::proceed;
                assert(!word_matches->empty());

                for (auto it = word_matches->begin(); it != word_matches->end();) {
                    auto& prev_pos = *it;

                    // TODO: make this binary search
                    if (std::ranges::any_of(positions, [prev_pos](int const pos) {
                        return pos == prev_pos.right;
                    })) {
                        // Matched; the current word's current n-gram is contiguous to the previous n-gram
                        prev_pos.right += N;
                        ++it;
                        continue;
                    }

                    it = word_matches->erase(it);
                }

                if (word_matches->empty()) {
                    search_res.erase_word_matches(word_matches);
                    if (search_res.empty()) return detail::search_continuation::abort;
                }
                return detail::search_continuation::proceed;
            });
        }
    }

    ngram_document_id max_doc_id_{0_doc_id};
    detail::ngram_index_storage<CharT> store_;
};


} // iris

#endif

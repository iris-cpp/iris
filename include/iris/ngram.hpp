#ifndef IRIS_NGRAM_HPP
#define IRIS_NGRAM_HPP

#include <format>
#include <span>
#include <flat_map>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include <utility>
#include <compare>

#include <cassert>

namespace iris {

enum struct ngram_document_id : unsigned {};

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

namespace iris {

namespace detail {

inline constexpr std::size_t N_GRAM_MAX_OPTIMIZED_N = 2;

} // detail

struct ngram_occurrence
{
    ngram_document_id doc_id;
    int pos;

    [[nodiscard]] constexpr bool operator==(ngram_occurrence const&) const noexcept = default;
    [[nodiscard]] constexpr std::strong_ordering operator<=>(ngram_occurrence const&) const noexcept = default;
};

template<std::size_t N, class CharT>
struct ngram
{
    static_assert(N >= 1);

    // TODO: optimize for N=1
    std::array<CharT, N> chars;

    template<std::size_t Len>
    [[nodiscard]] static constexpr ngram from_c_array(CharT const (&chars)[Len]) noexcept
    {
        assert(chars[Len - 1] == static_cast<CharT>(0));

        if constexpr (N == 1) {
            return ngram{chars[0]};
        } else if constexpr (N == 2) {
            return ngram{chars[0], chars[1]};
        } else {
            static_assert(detail::N_GRAM_MAX_OPTIMIZED_N == 2);
            ngram ng;
            std::ranges::copy_n(chars, Len - 1, ng.chars.begin());
            return ng;
        }
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
    return ngram<2, char32_t>{str[0], str[1]};
}

[[nodiscard]] constexpr auto operator ""_1gram(char32_t const* str, std::size_t len) noexcept
{
    assert(len == 1);
    return ngram<1, char32_t>{str[0]};
}

} // ngram_literals

template<class CharT, std::size_t N>
[[nodiscard]] ngram<N - 1, CharT> to_ngram(CharT const (&chars)[N]) noexcept
{
    return ngram<N - 1, CharT>::from_c_array(chars);
}


namespace detail {

template<std::size_t N, class CharT>
struct ngram_index
{
    void append(ngram<N, CharT> ng, ngram_document_id doc_id, int pos)
    {
        occs[ng].emplace_back(doc_id, pos);
    }

    std::flat_map<ngram<N, CharT>, std::vector<ngram_occurrence>> occs;
};

template<class CharT>
struct ngram_index_storage
{
    ngram_index<1, CharT> uni_idx;
    ngram_index<2, CharT> bi_idx;
};

} // detail

template<class CharT = char32_t>
class ngram_database
{
public:
    [[nodiscard]]
    ngram_document_id add_document(std::basic_string_view<CharT> const doc_text)
    {
        ngram_document_id const doc_id{max_doc_id_};
        max_doc_id_ = ngram_document_id{std::to_underlying(max_doc_id_) + 1u};

        for (std::size_t i = 0; i < doc_text.size(); ++i) {
            store_.uni_idx.append(ngram<1, CharT>{doc_text[i]}, doc_id, int(i));
        }

        auto const do_ngram = [&]<std::size_t N>(detail::ngram_index<N, CharT>& idx) {
            if (doc_text.size() < N) return;

            ngram<N, CharT> ng;
            std::size_t i = 0;
            for (; i < N; ++i) {
                ng.chars[i] = doc_text[i];
            }
            idx.append(ng, doc_id, 0);

            for (; i < doc_text.size(); ++i) {
                std::shift_left(ng.chars.begin(), ng.chars.end(), 1);
                ng.chars[N - 1] = doc_text[i];
                idx.append(ng, doc_id, int(i - N + 1));
            }
        };
        do_ngram(get_index<2>());
        static_assert(detail::N_GRAM_MAX_OPTIMIZED_N == 2);

        return doc_id;
    }

    template<std::size_t N>
    [[nodiscard]]
    std::vector<ngram_occurrence> const* get_occurrences(ngram<N, CharT> ng) const noexcept
    {
        auto const& idx = get_index<N>();
        auto const it = idx.occs.find(ng);
        if (it == idx.occs.end()) return nullptr;
        return &it->second;
    }

private:
    template<std::size_t N>
    [[nodiscard]] auto& get_index(this auto& self) noexcept
    {
        if constexpr (N == 1) {
            return self.store_.uni_idx;
        } else if constexpr (N == 2) {
            return self.store_.bi_idx;
        } else {
            static_assert(detail::N_GRAM_MAX_OPTIMIZED_N == 2);
        }
    }

    ngram_document_id max_doc_id_{0_doc_id};
    detail::ngram_index_storage<CharT> store_;
};


} // iris

#endif

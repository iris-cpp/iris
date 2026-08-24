// SPDX-License-Identifier: MIT

#include <iris/alloy/adapted/std_pair.hpp>
#include <iris/alloy/adapted/std_tuple.hpp>
#include <iris/alloy/adapt.hpp>
#include <iris/alloy/io.hpp>
#include <iris/alloy/traits.hpp>
#include <iris/alloy/tuple.hpp>
#include <iris/alloy/utility.hpp>

#include <iris/bits/is_function_object.hpp>
#include <iris/bits/specialization_of.hpp>

#include <iris/hash/FNV_hash.hpp>
#include <iris/hash/string_like_hash.hpp>

#include <iris/ngram/database.hpp>
#include <iris/ngram/gram.hpp>
#include <iris/ngram/id.hpp>
#include <iris/ngram/keyed_database.hpp>
#include <iris/ngram/search_query.hpp>
#include <iris/ngram/search_result.hpp>

#include <iris/pp/add.hpp>
#include <iris/pp/bool.hpp>
#include <iris/pp/cat.hpp>
#include <iris/pp/comma.hpp>
#include <iris/pp/decrement.hpp>
#include <iris/pp/for.hpp>
#include <iris/pp/if.hpp>
#include <iris/pp/increment.hpp>
#include <iris/pp/not_equal.hpp>
#include <iris/pp/rec.hpp>
#include <iris/pp/repeat.hpp>
#include <iris/pp/seq.hpp>
#include <iris/pp/stringize.hpp>
#include <iris/pp/sub.hpp>
#include <iris/pp/tuple.hpp>
#include <iris/pp/while.hpp>

#include <iris/rvariant/pack.hpp>
#include <iris/rvariant/recursive_wrapper.hpp>
#include <iris/rvariant/recursive_wrapper_pmr.hpp>
#include <iris/rvariant/rvariant.hpp>
#include <iris/rvariant/rvariant_io.hpp>
#include <iris/rvariant/subset.hpp>
#include <iris/rvariant/variant_helper.hpp>

#include <iris/unicode/string.hpp>

#include <iris/colorize_format.hpp>
#include <iris/compare.hpp>
#include <iris/cond_trivial_smf.hpp>
#include <iris/config.hpp>
#include <iris/default_init_allocator.hpp>
#include <iris/enum_bitops.hpp>
#include <iris/enum_bitops_algorithm.hpp>
#include <iris/enum_bitops_io.hpp>
#include <iris/exception.hpp>
#include <iris/fixed_string.hpp>
#include <iris/format.hpp>
#include <iris/format_traits.hpp>
#include <iris/hash.hpp>
#include <iris/hash_fwd.hpp>
#include <iris/indirect.hpp>
#include <iris/indirect_pmr.hpp>
#include <iris/interval.hpp>
#include <iris/interval_algo.hpp>
#include <iris/interval_set.hpp>
#include <iris/io_fwd.hpp>
#include <iris/iterator.hpp>
#include <iris/requirements.hpp>
#include <iris/rvariant.hpp>
#include <iris/snippet.hpp>
#include <iris/stdint.hpp>
#include <iris/string.hpp>
#include <iris/string_algo.hpp>
#include <iris/type_traits.hpp>
#include <iris/utility.hpp>

// Intentionally included later (do not do this in other tests)
#include "iris_test.hpp"

TEST_CASE("library_config")
{
    CHECK(true);
}

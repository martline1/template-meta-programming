#pragma once

#include <type_traits>

#include "OrchestratorBackend.hpp"
#include "core/TMP/TMDS/MetaMono/MetaMono.hpp"
#include "core/TMP/TMDS/MetaList/MetaList.hpp"
#include "core/TMP/TMDS/MetaDictEntry/MetaDictEntry.hpp"

#include "core/TMP/TMF/Meta/MetaMerge/MetaMerge.hpp"
#include "core/TMP/TMF/Meta/MetaTransfer/MetaTransfer.hpp"
#include "core/TMP/TypeTraits/MatchesTemplate/MatchesTemplate.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for meta_dict_get_value.
 */
template<MatchesTemplate<meta_list> Entries, typename Key>
struct meta_dict_get_value;

/**
 * Template Meta Function through partial template specialization
 * to get the value type associated with a given key type
 * from a meta_dict.
 *
 * @tparam Entries A meta list of meta dict entries.
 * @tparam Key The key type whose associated value type is to be retrieved.
 * @return The value type associated with the provided key type.
 */
template<
  // Deducted from parameters:
  typename... Entries,

  // Parameters:
  typename Key
>
requires (MatchesTemplate<Entries, meta_dict_entry> && ...)
struct meta_dict_get_value<
  meta_list<Entries...>, // First  param is a meta list of meta dict entries
  Key                    // Second param is the key to search for
> {
  private:
    template<typename Entry>
    static auto filter() -> std::conditional_t<
      !std::is_same_v<typename Entry::key, Key>,
      meta_list<>,
      meta_list<typename Entry::value>
    >;

    using filterResults = meta_merge_t<decltype(filter<Entries>())...>;

    static_assert(
      filterResults::size == 1,
      "meta_dict_get_value error: The provided key does not exist in the meta dict, or it is not unique."
    );

    using filterResultMono = meta_transfer_t<
      filterResults,
      meta_mono
    >;

  public:
    using type = filterResultMono::type;

    meta_dict_get_value() = delete; // Prevent usage outside of TMP
};

/**
 * Type alias for easier usage of meta_dict_get_value.
 *
 * @tparam Entries A meta list of meta dict entries.
 * @tparam Key The key type whose associated value type is to be retrieved.
 * @return The value type associated with the provided key type.
 */
template<MatchesTemplate<meta_list> Entries, typename Key>
using meta_dict_get_value_t = typename meta_dict_get_value<Entries, Key>::type;

END_ORCHESTRATOR

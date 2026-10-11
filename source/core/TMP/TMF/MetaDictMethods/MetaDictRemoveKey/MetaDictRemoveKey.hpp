#pragma once

#include <type_traits>

#include "core/TMP/TMDS/MetaList/MetaList.hpp"
#include "OrchestratorBackend.hpp"
#include "core/TMP/TMF/Meta/MetaTransfer/MetaTransfer.hpp"
#include "core/TMP/TMDS/MetaDictEntry/MetaDictEntry.hpp"
#include "core/TMP/TypeTraits/MatchesTemplate/MatchesTemplate.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for meta_dict_remove_key.
 */
template<typename MetaDictLike, typename Key>
struct meta_dict_remove_key;


/**
 * Template Meta Function through partial template specialization
 * to remove a key-value entry associated with a given key type
 * from a meta_dict-like structure.
 *
 * @tparam MetaDictLike A meta_dict-like type containing meta_dict_entry types.
 * @tparam Key The key type whose associated entry is to be removed.
 * @return A new meta_dict-like type without the entry for the provided key type.
 */
template<
  // Deducted from parameters:
  template<typename...> typename MetaDictLike,
  typename... Entries,

  // Parameters:
  typename Key
>
requires (MatchesTemplate<Entries, meta_dict_entry> && ...)
struct meta_dict_remove_key<
  MetaDictLike<Entries...>, // First  param is a meta_dict-like type
  Key                       // Second param is the key to remove
> {
  private:
    template<typename Entry>
    static auto filter() -> std::conditional_t<
      std::is_same_v<typename Entry::key, Key>,
      meta_list<>,
      meta_list<Entry>
    >;

    using newEntriesMetaList = meta_merge_t<decltype(filter<Entries>())...>;

  public:
    using type = meta_transfer_t<newEntriesMetaList, MetaDictLike>;

    meta_dict_remove_key() = delete; // Prevent usage outside of TMP.
};

/**
 * Type alias for easier usage of TMF meta_dict_remove_key.
 *
 * @tparam MetaDictLike A meta_dict-like type containing meta_dict_entry types.
 * @tparam Key The key type whose associated entry is to be removed.
 * @return A new meta_dict-like type without the entry for the provided key type.
 */
template<typename MetaDictLike, typename Key>
using meta_dict_remove_key_t = typename meta_dict_remove_key<MetaDictLike, Key>::type;


END_ORCHESTRATOR

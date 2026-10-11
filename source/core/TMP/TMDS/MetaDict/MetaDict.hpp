#pragma once

#include "OrchestratorBackend.hpp"

#include "core/TMP/TMDS/MetaList/MetaList.hpp"
#include "core/TMP/TMDS/MetaDictEntry/MetaDictEntry.hpp"

#include "core/TMP/TMF/Meta/MetaPush/MetaPush.hpp"
#include "core/TMP/TMF/MetaDictMethods/MetaDictGetValue/MetaDictGetValue.hpp"
#include "core/TMP/TMF/MetaDictMethods/MetaDictRemoveKey/MetaDictRemoveKey.hpp"

#include "core/TMP/TypeTraits/MetaContains/MetaContains.hpp"
#include "core/TMP/concepts/meta/VariadicDuplication.hpp"

BEGIN_ORCHESTRATOR

template<MatchesTemplate<meta_dict_entry>... EntryTypes>
struct meta_dict {
  private:
    static_assert(
      HasNoDuplicates<typename EntryTypes::key...>,
      "meta_dict error: Duplicate keys detected in meta_dict entries."
    );

    // Properties:

    using keys    = meta_list<typename EntryTypes::key...>;
    using entries = meta_list<EntryTypes...>;

  public:
    // Type Traits:

    /**
     * Type trait that indicates whether the meta dict contains the given key.
     *
     * @param KeyType The key type to check for.
     * @return true if the key exists in the meta dict, false otherwise.
     */
    template<typename KeyType>
    static constexpr bool hasKey = meta_contains_v<keys, KeyType>;

    // Template Meta Functions:

    /**
     * Template Meta Function to add a new meta dict entry to the meta dict.
     * The entry is only added if its key does not already exist in the meta dict.
     *
     * @param MetaDictEntryType The meta dict entry type to be added.
     * @return A new meta dict type with the added entry.
     */
    template<MatchesTemplate<meta_dict_entry> MetaDictEntryType>
    requires (!hasKey<typename MetaDictEntryType::key>)
    using pushEntry =  meta_push_t<
      meta_dict, // Current specialization (Injected Class Name)
      MetaDictEntryType
    >;

    /**
     * Template Meta Function to get the value type associated with a given key type
     * from the meta dict.
     *
     * @param KeyType The key type whose associated value type is to be retrieved.
     * @return The value type associated with the provided key type.
     */
    template<typename KeyType>
    requires (hasKey<KeyType>)
    using getValue = meta_dict_get_value_t<entries, KeyType>;

    /**
     * Template Meta Function to remove a meta dict entry by its key type.
     * The entry is only removed if its key exists in the meta dict.
     *
     * @param KeyType The key type of the entry to be removed.
     * @return if the key existed, a new meta dict type without the removed entry,
     * the same meta dict type otherwise.
     */
    template<typename KeyType>
    using removeKey = meta_dict_remove_key_t<
      meta_dict, // Current specialization (Injected Class Name)
      KeyType
    >;

    meta_dict() = delete; // Prevent usage outside of TMP
};


END_ORCHESTRATOR

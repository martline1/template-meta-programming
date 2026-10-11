#pragma once

#include "OrchestratorBackend.hpp"
#include "core/TMP/TMDS/MetaList/MetaList.hpp"

BEGIN_ORCHESTRATOR

template<typename KeyType, typename ValueType>
struct meta_dict_entry {
  using key   = KeyType;
  using value = ValueType;

  meta_dict_entry() = delete; // Prevent usage outside of TMP
};

END_ORCHESTRATOR

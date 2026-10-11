#pragma once

#include "core/TMP/TMDS/MetaList/MetaList.hpp"
#include "core/TMP/TMF/Meta/MetaOmit/MetaOmit.hpp"
#include "core/TMP/TMF/Meta/MetaPush/MetaPush.hpp"
#include "core/TMP/TMF/Meta/MetaMerge/MetaMerge.hpp"
#include "OrchestratorBackend.hpp"
#include "core/TMP/TypeTraits/MatchesTemplate/MatchesTemplate.hpp"

BEGIN_ORCHESTRATOR

// Specializations:

/**
 * Template Meta Function to merge multiple meta_lists into one.
 *
 * @tparam T... The meta_lists to merge.
 */
template<MatchesTemplate<meta_list>... T>
using meta_list_merge_t = meta_merge_t<T...>;

/**
 * Template Meta Function to omit types from a meta_list.
 *
 * @tparam T The meta_list to omit from.
 * @tparam U... The types to omit.
 */
template<MatchesTemplate<meta_list> T, typename... U>
using meta_list_omit_t = meta_omit_t<T, U...>;

/**
 * Template Meta Function to push types to a meta_list.
 *
 * @tparam T The meta_list to push to.
 * @tparam First The first type to push.
 * @tparam Rest Optional types to push.
 * @return A new meta_list with the original and items to push appended.
 */
template<MatchesTemplate<meta_list> T, typename First, typename... Rest>
using meta_list_push_t = meta_push_t<T, First, Rest...>;

END_ORCHESTRATOR

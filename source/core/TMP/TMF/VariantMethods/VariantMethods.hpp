#pragma once

#include <variant>

#include "core/TMP/TMF/Meta/MetaOmit/MetaOmit.hpp"
#include "core/TMP/TMF/Meta/MetaMerge/MetaMerge.hpp"
#include "OrchestratorBackend.hpp"
#include "core/TMP/TypeTraits/MatchesTemplate/MatchesTemplate.hpp"

BEGIN_ORCHESTRATOR

// Specializations:

/**
 * Template Meta Function to merge multiple variants into one.
 *
 * @tparam T... The variants to merge.
 */
template<MatchesTemplate<std::variant>... T>
using variant_merge_t = meta_merge_t<T...>;

/**
 * Template Meta Function to omit types from a variant.
 *
 * @tparam T The variant to omit from.
 * @tparam U... The types to omit.
 */
template<MatchesTemplate<std::variant> T, typename... U>
using variant_omit_t = meta_omit_t<T, U...>;

END_ORCHESTRATOR
























#pragma once

#include <type_traits>

#include "core/TMP/TMDS/MetaList/MetaList.hpp"
#include "OrchestratorBackend.hpp"
#include "core/TMP/TMF/Meta/MetaTransfer/MetaTransfer.hpp"
#include "core/TMP/TypeTraits/IsContainerType/IsContainerType.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for meta_distinct_impl.
 */
template<IsContainerType Input>
struct meta_distinct;

// Handle 0 or no duplicates case, return input as is
template<
  // Deducted from parameters:
  template<typename...> typename InputContainer,
  typename... InputElements
>
requires (
  sizeof...(InputElements) == 0 ||
  HasNoDuplicates<InputElements...>
)
struct meta_distinct<
  // Parameters:
  InputContainer<InputElements...>
> {
  using type = InputContainer<InputElements...>;

  meta_distinct() = delete; // Prevent usage outside of TMP
};

/**
 * Template Meta Function through partial template specialization
 * to remove duplicate types from a container type.
 *
 * @tparam Input The input container type to process.
 * @return A container type resulting from removing duplicate
 * types from the input container type.
 */
template<
  // Deducted from parameters:
  template<typename...> typename InputContainer,
  typename... InputElements
>
requires (
  sizeof...(InputElements) > 0 &&
  HasDuplicates<InputElements...>
)
struct meta_distinct<
  // Parameters:
  InputContainer<InputElements...>
> {
  private:
    using result_meta_list = typename meta_distinct_impl<
      InputContainer<InputElements...>,
      meta_list<>
    >::type;

  public:
    using type = meta_transfer_t<result_meta_list, InputContainer>;

    meta_distinct() = delete; // Prevent usage outside of TMP
};

/**
 * Type alias for easier usage of TMF meta_distinct.
 *
 * @tparam Input The input container type to process.
 * @return A container type resulting from removing duplicate
 * types from the input container type.
 */
template<IsContainerType Input>
using meta_distinct_t = typename meta_distinct<Input>::type;

END_ORCHESTRATOR

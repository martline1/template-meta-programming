#pragma once

#include <type_traits>

#include "core/TMP/TMF/Meta/MetaPush/MetaPush.hpp"
#include "OrchestratorBackend.hpp"
#include "core/TMP/TypeTraits/MetaContains/MetaContains.hpp"
#include "core/TMP/TypeTraits/MetaContains/MetaContains.hpp"
#include "core/TMP/TypeTraits/IsContainerType/IsContainerType.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for meta_distinct_impl.
 */
template<IsContainerType Input, IsContainerType Result>
struct meta_distinct_impl;

// Base case: empty input container
template<
  // Deducted from parameters:
  template<typename...> typename InputContainer,

  // Parameters:
  IsContainerType Result
>
struct meta_distinct_impl<
  InputContainer<>,
  Result
> {
  using type = Result;

  meta_distinct_impl() = delete; // Prevent usage outside of TMP
};

/**
 * Template Meta Function through partial template specialization
 * to process an input container type recursively, removing
 * duplicate types and building a result container type. Keeping
 * order and original template.
 *
 * @tparam Input The input container type to process.
 * @tparam Result The result container type being built.
 * @return A container type resulting from removing duplicate
 * types from the input container type.
 */
template<
  // Deducted from parameters:
  template<typename...> typename InputContainer,
  typename InputElement,
  typename... Rest,

  // Parameters:
  IsContainerType Result
>
struct meta_distinct_impl<
  InputContainer<InputElement, Rest...>,
  Result
> {
  private:
    using Input = InputContainer<InputElement, Rest...>;

    static constexpr bool itemAlreadyAdded = meta_contains_v<Result, InputElement>;

    using newResult = std::conditional_t<
      itemAlreadyAdded,
      Result,
      meta_push_t<Result, InputElement>
    >;

  public:
    using type = typename meta_distinct_impl<
      InputContainer<Rest...>,
      newResult
    >::type;

    meta_distinct_impl() = delete; // Prevent usage outside of TMP
};

/**
 * Type alias for easier usage of TMF meta_distinct_impl.
 *
 * @tparam Input The input container type to process.
 * @tparam Result The result container type being built.
 * @return A container type resulting from removing duplicate
 * types from the input container type.
 */
template<IsContainerType Input, IsContainerType Result>
using meta_distinct_impl_t = typename meta_distinct_impl<Input, Result>::type;

END_ORCHESTRATOR

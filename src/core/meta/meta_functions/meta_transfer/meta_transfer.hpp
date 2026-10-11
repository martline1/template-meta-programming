#pragma once

#include "../../type_traits/variadic_instance.hpp"
#include "../meta_normalize.hpp"

namespace core::meta
{


template<VariadicInstance Source, template<typename...> typename Target>
struct meta_transfer;


template<
	// Deducted from parameters:
	template<typename...> typename SourceContainer,
	typename... SourceItems,

	// Parameters:
	template<typename...> typename Target
>
struct meta_transfer<
	SourceContainer<SourceItems...>,
	Target
> {
	meta_transfer() = delete;

	using type = meta_normalize_t<Target<SourceItems...>>;
};


template<VariadicInstance Source, template<typename...> typename Target>
using meta_transfer_t = typename meta_transfer<Source, Target>::type;


} // namespace core::meta

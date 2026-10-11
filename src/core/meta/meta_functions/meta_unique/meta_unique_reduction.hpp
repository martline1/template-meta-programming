#pragma once

#include <type_traits>

#include "../../type_traits/variadic_instance.hpp"
#include "../../type_traits/meta_contains.hpp"
#include "../meta_push.hpp"

namespace core::meta::detail
{


template<VariadicInstance TypeSequence, VariadicInstance Result>
struct meta_unique_reduction;


template<
	// Deducted from parameters:
	template<typename...> typename Container,

	// Parameters:
	VariadicInstance Result
>
struct meta_unique_reduction<Container<>, Result> {
	meta_unique_reduction() = delete;

	using type = Result;
};


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename CurrentItem,
	typename... RestOfItems,

	// Parameters:
	VariadicInstance Result
>
struct meta_unique_reduction<
	Container<CurrentItem, RestOfItems...>,
	Result
> {
	private:
		using new_result = std::conditional_t<
			meta_contains_v<Result, CurrentItem>, // If already there
			Result,																// Do nothing
			meta_push_back_t<Result, CurrentItem> // else add it to the result
		>;

	public:
		using type = typename meta_unique_reduction<
			Container<RestOfItems...>,
			new_result
		>::type;

	meta_unique_reduction() = delete;
};


template<VariadicInstance TypeSequence, VariadicInstance Result>
using meta_unique_reduction_t = typename meta_unique_reduction<TypeSequence, Result>::type;


} // namespace core::meta::detail

#pragma once

#include "../type_traits/variadic_instance.hpp"

namespace core::meta
{


template<VariadicInstance Container, typename FirstToPush, typename... RestToPush>
struct meta_push_back;


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename... Items,

	// Parameters:
	typename FirstToPush,
	typename... RestToPush
>
struct meta_push_back<
	Container<Items...>,
	FirstToPush,
	RestToPush...
> {
	meta_push_back() = delete;

	using type = Container<Items..., FirstToPush, RestToPush...>;
};


template<VariadicInstance Container, typename FirstToPush, typename... RestToPush>
using meta_push_back_t = typename meta_push_back<Container, FirstToPush, RestToPush...>::type;


// ==================================================================================================


template<VariadicInstance Container, typename FirstToPush, typename... RestToPush>
struct meta_push_front;


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename... Items,

	// Parameters:
	typename FirstToPush,
	typename... RestToPush
>
struct meta_push_front<
	Container<Items...>,
	FirstToPush,
	RestToPush...
> {
	meta_push_front() = delete;

	using type = Container<FirstToPush, RestToPush..., Items...>;
};


template<VariadicInstance Container, typename FirstToPush, typename... RestToPush>
using meta_push_front_t = typename meta_push_front<Container, FirstToPush, RestToPush...>::type;


} // namespace core::meta

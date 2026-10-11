#pragma once

#include "../../concepts/variadic_duplication.hpp"
#include "../../type_traits/variadic_instance.hpp"
#include "./meta_unique_reduction.hpp"

namespace core::meta
{


template<VariadicInstance T>
struct meta_unique;


// If there are no duplicates, return as is
template<
	template<typename...> typename Container,
	typename... Items
>
requires HasNoDuplicates<Items...>
struct meta_unique<
	Container<Items...>
> {
	meta_unique() = delete;

	using type = Container<Items...>;
};


// If there are duplicates, remove them
template<
	template<typename...> typename Container,
	typename... Items
>
requires HasDuplicates<Items...>
struct meta_unique<
	Container<Items...>
> {
	meta_unique() = delete;

	using type = detail::meta_unique_reduction_t<Container<Items...>, Container<>>;
};


template<VariadicInstance T>
using meta_unique_t = typename meta_unique<T>::type;


} // namespace core::meta

#pragma once

#include <type_traits>

#include "./variadic_instance.hpp"

namespace core::meta
{


template<VariadicInstance TypeSequence, typename ToFind>
struct meta_contains: std::false_type {};


template<
	// Deducted from parameters;
	template<typename...> typename Container,
	typename... Elements,

	// Parameters:
	typename ToFind
>
struct meta_contains<
	Container<Elements...>,
	ToFind
> {
	meta_contains() = delete;

	static constexpr auto value = std::disjunction_v<
		std::is_same<ToFind, Elements>...
	>;
};


template<VariadicInstance TypeSequence, typename ToFind>
inline constexpr bool meta_contains_v = meta_contains<TypeSequence, ToFind>::value;


template<typename TypeSequence, typename ToFind>
concept MetaContains = requires {
	requires meta_contains_v<std::remove_cvref_t<TypeSequence>, std::remove_cvref_t<ToFind>>;
};


} // namespace core::meta

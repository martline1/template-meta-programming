#pragma once

#include "../../type_traits/variadic_instance.hpp"
#include "../meta_normalize.hpp"
#include "./meta_merge_binary.hpp"

namespace core::meta
{


template<VariadicInstance FirstParam, VariadicInstance... RestOfParams>
struct meta_merge;


// Handle 1 parameter
template<VariadicInstance T>
struct meta_merge<T> {
	meta_merge() = delete;

	using type = meta_normalize_t<T>;
};


// Handle 2 or more parameters
template<
	VariadicInstance FirstParam,
	VariadicInstance SecondParam,
	// Type constraint is already verified at primary template definition:
	typename... Rest
>
struct meta_merge<FirstParam, SecondParam, Rest...> {
	meta_merge() = delete;

	using type =  typename meta_merge<
		detail::meta_merge_binary_t<FirstParam, SecondParam>,
		Rest...
	>::type;
};


template<VariadicInstance FirstParam, VariadicInstance... RestOfParams>
using meta_merge_t = typename meta_merge<FirstParam, RestOfParams...>::type;


} // namespace core::meta

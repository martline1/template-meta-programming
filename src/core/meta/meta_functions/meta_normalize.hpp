#pragma once

#include <variant>
#include <format>

#include "../type_traits/dependent_false.hpp"
#include "../type_traits/variadic_instance.hpp"
#include "../type_traits/matches_variadic_template.hpp"

namespace core::meta
{


template<VariadicInstance T>
struct meta_normalize;


// If not empty, return it as is
template<
	template<typename...> typename Container,
	typename... Items
>
requires (sizeof...(Items) > 0)
struct meta_normalize<Container<Items...>> {
	meta_normalize() = delete;

	using type = Container<Items...>;
};


// If empty, try to pupulate it or throw if not possible
template<
	template<typename...> typename Container,
	typename... Items
>
requires (sizeof...(Items) == 0)
struct meta_normalize<Container<Items...>> {
	meta_normalize() = delete;

	static_assert(
		!(
			matches_variadic_template_v<Container, std::format_string> ||
			matches_variadic_template_v<Container, std::wformat_string>
		),
		"[meta_normalize<expression>] expression evaluated to an empty format_string or wformat_string!"
	);

	static_assert(
		!matches_variadic_template_v<Container, std::common_type>,
		"[meta_normalize<expression>] expression evaluated to an empty common_type!"
	);

	static_assert(
		!matches_variadic_template_v<Container, std::common_reference>,
		"[meta_normalize<expression>] expression evaluated to an empty common_reference!"
	);

	using type = std::conditional_t<
		matches_variadic_template_v<Container, std::variant>,
		std::variant<std::monostate>,
		Container<Items...>
	>;
};


template<VariadicInstance T>
using meta_normalize_t = typename meta_normalize<T>::type;


} // namespace core::meta

#pragma once

#include <type_traits>

#include "./variadic_instance.hpp"

namespace core::meta
{


template<VariadicInstance TypeParam, template<typename...> typename TemplateParam>
struct matches_variadic_instance: std::false_type {};


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename... Args
>
struct matches_variadic_instance<
	Container<Args...>,
	Container
>: std::true_type {};


template<VariadicInstance TypeParam, template<typename...> typename TemplateParam>
inline constexpr bool matches_variadic_instance_v = matches_variadic_instance<TypeParam, TemplateParam>::value;


template<typename TypeParam, template<typename...> typename TemplateParam>
concept MatchesVariadicInstance = requires {
	requires matches_variadic_instance_v<std::remove_cvref_t<TypeParam>, TemplateParam>;
};


} // namespace core::meta

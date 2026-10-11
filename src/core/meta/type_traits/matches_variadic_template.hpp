#pragma once

#include <type_traits>

namespace core::meta
{


template<
	template<typename...> typename T1,
	template<typename...> typename T2
>
struct matches_variadic_template: std::false_type {};


template<
	template<typename...> typename SameTemplateTemplate
>
struct matches_variadic_template<SameTemplateTemplate, SameTemplateTemplate>: std::true_type {};


template<
	template<typename...> typename T1,
	template<typename...> typename T2
>
inline constexpr bool matches_variadic_template_v = matches_variadic_template<T1, T2>::value;


template<
	template<typename...> typename T1,
	template<typename...> typename T2
>
concept MatchesVariadicTemplate = matches_variadic_template_v<T1, T2>;


} // namespace core::meta

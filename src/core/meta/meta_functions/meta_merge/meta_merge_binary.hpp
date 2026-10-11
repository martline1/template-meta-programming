#pragma once

#include <type_traits>

#include "../../meta_structures/meta_list.hpp"
#include "../../type_traits/variadic_instance.hpp"
#include "../../type_traits/matches_variadic_template.hpp"

namespace core::meta::detail
{


template<VariadicInstance T1, VariadicInstance T2>
struct meta_merge_binary;


template<
	// Deducted from parameters:
	template<typename...> typename FirstContainer,
	template<typename...> typename SecondContainer,
	typename... FirstElements,
	typename... SecondElements
>
struct meta_merge_binary<
	FirstContainer<FirstElements...>,
	SecondContainer<SecondElements...>
> {
	private:
		static constexpr bool are_same_container = matches_variadic_template_v<
			FirstContainer,
			SecondContainer
		>;

	public:
		using type = std::conditional_t<
			are_same_container,
			FirstContainer<FirstElements..., SecondElements...>,
			meta_list<FirstElements..., SecondElements...>
		>;

		meta_merge_binary() = delete;
};


template<VariadicInstance T1, VariadicInstance T2>
using meta_merge_binary_t = typename meta_merge_binary<T1, T2>::type;


} // namespace core::meta::detail

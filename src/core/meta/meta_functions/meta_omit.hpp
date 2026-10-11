#pragma once

#include <type_traits>

#include "../meta_structures/meta_list.hpp"
#include "../type_traits/variadic_instance.hpp"
#include "../type_traits/meta_contains.hpp"
#include "./meta_transfer/meta_transfer.hpp"
#include "./meta_merge/meta_merge.hpp"
#include "./meta_normalize.hpp"

namespace core::meta
{


template<VariadicInstance Container, typename ToOmit, typename... RestToOmit>
struct meta_omit;


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename... Items,

	// Parameters:
	typename ToOmit,
	typename... RestToOmit
>
struct meta_omit<
	Container<Items...>,
	ToOmit,
	RestToOmit...
> {
	private:
		using all_to_omit = meta_list<ToOmit, RestToOmit...>;

		template<typename Element>
		using filter = std::conditional_t<
			meta_contains_v<all_to_omit, Element>,
			meta_list<>,
			meta_list<Element>
		>;

		using filtered_meta_list = meta_merge_t<filter<Items>...>;

		using result = meta_transfer_t<filtered_meta_list, Container>;

	public:
		using type = meta_normalize_t<result>;

		meta_omit() = delete;
};


template<VariadicInstance Container, typename ToOmit, typename... RestToOmit>
using meta_omit_t = typename meta_omit<Container, ToOmit, RestToOmit...>::type;


} // namespace core::meta

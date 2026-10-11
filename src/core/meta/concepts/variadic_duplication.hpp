#pragma once

#include <type_traits>

namespace core::meta
{


namespace detail
{

	template<typename ToFind, typename... Items>
	static constexpr size_t count_occurrences =
		(0 + ... + static_cast<size_t>(std::is_same_v<ToFind, Items>));

} // namespace detail


template<typename... Ts>
concept HasNoDuplicates =
	sizeof...(Ts) <= 1 ||
	((detail::count_occurrences<Ts, Ts...> == 1) && ...);


template<typename... Ts>
concept HasDuplicates = !HasNoDuplicates<Ts...>;


} // namespace core::meta

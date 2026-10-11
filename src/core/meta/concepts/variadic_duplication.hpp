#pragma once

#include <concepts>
#include <type_traits>

#include "../meta_structures/meta_mono.hpp"

namespace core::meta
{


namespace detail
{

	template<typename... Ts>
	struct DuplicateChecker: meta_mono<Ts>... {};

} // namespace detail


template<typename... Ts>
concept HasNoDuplicates =
	sizeof...(Ts) <= 1 ||
	requires(detail::DuplicateChecker<Ts...>& checker) {
		(static_cast<void>(static_cast<meta_mono<Ts>&>(checker)), ...);
	};


template<typename... Ts>
concept HasDuplicates = !HasNoDuplicates<Ts...>;


} // namespace core::meta

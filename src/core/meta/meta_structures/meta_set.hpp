#pragma once

#include "../concepts/variadic_duplication.hpp"
#include "./meta_list.hpp"

// TODO: WIP.

namespace core::meta
{


template<typename... Ts>
requires HasNoDuplicates<Ts...>
struct meta_set {
	meta_set() = delete;

	using values = meta_list<Ts...>;
};


template<typename... Ts>
requires HasDuplicates<Ts...>
struct meta_set {
	meta_set() = delete;

};




} // namespace core::meta

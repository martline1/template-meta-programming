#pragma once

namespace core::meta
{


template<typename... Ts>
struct meta_list {
	meta_list() = delete;

	// Properties:

	static constexpr size_t size = sizeof...(Ts);

	// Methods:

	template<typename FirstToPush, typename... RestToPush>
	using push_back = meta_list<Ts..., FirstToPush, RestToPush...>;


	template<typename FirstToPush, typename... RestToPush>
	using push_front = meta_list<FirstToPush, RestToPush..., Ts...>;
};


} // namespace core::meta

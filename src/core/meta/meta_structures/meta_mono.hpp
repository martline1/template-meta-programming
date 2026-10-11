#pragma once

namespace core::meta
{


template<typename T>
struct meta_mono {
	meta_mono() = delete;

	using type = T;
};

template<typename T>
using meta_mono_t = typename meta_mono<T>::type;


} // namespace core::meta

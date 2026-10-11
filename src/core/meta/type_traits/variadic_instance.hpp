#pragma once

#include <type_traits>

namespace core::meta
{


template<typename T>
struct is_variadic_instance: std::false_type {};


template<
	template<typename...> typename Container,
	typename... Args
>
struct is_variadic_instance<Container<Args...>>: std::true_type {};


template<typename T>
inline constexpr bool is_variadic_instance_v = is_variadic_instance<T>::value;


template<typename T>
concept VariadicInstance = is_variadic_instance_v<std::remove_cvref_t<T>>;


} // namespace core::meta

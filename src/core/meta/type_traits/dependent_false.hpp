#pragma once

#include <type_traits>

namespace core::meta
{


template<typename T, typename... Ts>
struct dependent_false: std::false_type {};


template<typename T, typename... Ts>
inline constexpr bool dependent_false_v = dependent_false<T, Ts...>::value;


template<typename T, typename... Ts>
concept DependentFalse = dependent_false_v<T, Ts...>;


} // namespace core::meta

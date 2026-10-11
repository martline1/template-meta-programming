#pragma once

#include <type_traits>

namespace core::meta
{


template<template<typename...> typename T>
struct is_variadic_template: std::true_type {};


template<template<typename...> typename T>
inline constexpr bool is_variadic_template_v = is_variadic_template<T>::value;


template<template<typename...> typename T>
concept VariadicTemplate = is_variadic_template_v<T>;


} // namespace core::meta

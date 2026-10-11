#pragma once

#include <concepts>
#include <type_traits>

namespace core::meta
{


template<typename T>
concept IsEnum = std::is_enum_v<std::remove_cvref_t<T>>;


} // namespace core::meta

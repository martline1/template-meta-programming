#pragma once

#include <variant>

#include "../type_traits/matches_variadic_instance.hpp"
#include "./meta_merge/meta_merge.hpp"

namespace core::meta
{


template<MatchesVariadicInstance<std::variant>... Ts>
using variant_merge_t =	meta_merge_t<Ts...>;










} // namespace core::meta

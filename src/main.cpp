#include <iostream>
#include <string>
#include <functional>
#include <type_traits>
#include <variant>

#include "core/meta/meta.hpp"

using namespace core::meta;

using result = meta_omit_t<std::variant<int>, int>;

static_assert(
	std::same_as<result, std::variant<std::monostate>>,
	"Expected that to be equal"
);


int main() {

	return 0;
}

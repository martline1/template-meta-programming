#include <gtest/gtest.h>

#include "../../meta_structures/meta_list.hpp"
#include "./meta_transfer.hpp"

using namespace core::meta;

TEST(MetaTransfer, Transfer_types_from_one_container_to_another) {
	using source = std::variant<int, double, char>;

	using transferred = meta_transfer_t<source, meta_list>;
	using expected = meta_list<int, double, char>;

	static_assert(
		std::is_same_v<transferred, expected>,
		"Expected transferred type to be meta_list<int, double, char>"
	);
}


TEST(MetaTransfer, Transfer_empty_meta_list_into_variant_yields_variant_of_monostate) {
	using transferred = meta_transfer_t<meta_list<>, std::variant>;
	using expected = std::variant<std::monostate>;

	static_assert(
		std::is_same_v<transferred, expected>,
		"Expected transferred type to be std::variant<std::monostate>"
	);
}

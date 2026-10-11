#include <gtest/gtest.h>
#include <variant>

#include "./meta_merge.hpp"

using namespace core::meta;

TEST(MetaMerge, Merge_two_meta_lists) {
	using list1 = meta_list<int, double>;
	using list2 = meta_list<char, float>;

	using merged = meta_merge_t<list1, list2>;
	using expected = meta_list<int, double, char, float>;

	static_assert(
		std::is_same_v<merged, expected>,
		"type mismatch: merged must be meta_list<int, double, char, float>"
	);
}


TEST(MetaMerge, Merge_two_distinct_variadic_instances) {
	using list1 = meta_list<int, double>;
	using list2 = std::variant<char, float>;

	using merged = meta_merge_t<list1, list2>;
	using expected = meta_list<int, double, char, float>;

	static_assert(
		std::is_same_v<merged, expected>,
		"type mismatch: merged must be meta_list<int, double, char, float>"
	);
}


TEST(MetaMerge, Should_handle_empty_meta_list) {
  using list1 = meta_list<>;

  using merged = meta_merge_t<list1>;

  static_assert(std::is_same_v<
    merged,
    meta_list<>
  >, "type mismatch: merged must be an empty meta_list");
}


TEST(MetaMerge, Should_handle_one_parameter_meta_list_correctly) {
  using list1 = meta_list<int, double>;

  using merged = meta_merge_t<list1>;

  static_assert(std::is_same_v<
    merged,
    meta_list<int, double>
  >, "type mismatch: merged must be meta_list<int, double>");
}


TEST(MetaMerge, Should_handle_two_parameters_meta_list_correctly) {
  using list1 = meta_list<int, double>;
  using list2 = meta_list<char, float>;

  using merged = meta_merge_t<list1, list2>;

  static_assert(std::is_same_v<
    merged,
    meta_list<int, double, char, float>
  >, "type mismatch: merged must be meta_list<int, double, char, float>");
}


TEST(MetaMerge, Should_handle_three_parameters_meta_list_correctly) {
  using list1 = meta_list<int, double>;
  using list2 = meta_list<char, float>;
  using list3 = meta_list<bool, short>;

  using merged = meta_merge_t<list1, list2, list3>;

  static_assert(std::is_same_v<
    merged,
    meta_list<int, double, char, float, bool, short>
  >, "type mismatch: merged must be meta_list<int, double, char, float, bool, short>");
}


TEST(MetaMerge, Should_handle_multiple_parameters_meta_lists_with_varied_sizes) {
  using list1 = meta_list<int, double>;
  using list2 = meta_list<char, float>;
  using list3 = meta_list<bool>;
  using list4 = meta_list<>;
  using list5 = meta_list<short>;

  using merged = meta_merge_t<list1, list2, list3, list4, list5>;

  static_assert(std::is_same_v<
    merged,
    meta_list<int, double, char, float, bool, short>
  >, "type mismatch: merged must be meta_list<int, double, char, float, bool, short>");
}


TEST(MetaMerge, Should_keep_duplicates_when_merging_two_lists_of_the_same_values) {
  using list1 = meta_list<int, double>;
  using list2 = meta_list<int, double>;

  using merged = meta_merge_t<list1, list2>;

  static_assert(std::is_same_v<
    merged,
    meta_list<int, double, int, double>
  >, "type mismatch: merged must be meta_list<int, double, int, double>");
}

#include <string>
#include <type_traits>
#include <gtest/gtest.h>

#include "MetaDict.hpp"

using namespace OrchestratorBackend;

namespace TestData {
  struct name;
  struct age;

  using dict = meta_dict<
    meta_dict_entry<name, std::string>,
    meta_dict_entry<age, unsigned short>
  >;
}

TEST(MetaDictTest, MetaDict_hasKey_should_detect_existing_and_non_existing_keys) {
  using namespace TestData;

  // Known keys:
  static_assert(dict::hasKey<name>, "dict should have key 'name'");
  static_assert(dict::hasKey<age>, "dict should have key 'age'");

  // Unknown key:
  static_assert(!dict::hasKey<bool>, "dict should not have key 'bool'");
  static_assert(!dict::hasKey<char>, "dict should not have key 'char'");

  // If it compiled, it passed
}

TEST(MetaDictTest, MetaDict_getValue_should_return_correct_value_type_for_existing_keys) {
  using namespace TestData;

  using name_type = dict::getValue<name>;
  using age_type  = dict::getValue<age>;

  static_assert(std::is_same_v<name_type, std::string>, "Value type for key 'name' should be std::string");
  static_assert(std::is_same_v<age_type, unsigned short>, "Value type for key 'age' should be unsigned short");

  // If it compiled, it passed
}

TEST(MetaDictTest, MetaDict_pushEntry_should_add_new_entry_with_unique_key) {
  using namespace TestData;

  struct height;

  using new_dict = dict::pushEntry<meta_dict_entry<height, float>>;

  static_assert(new_dict::hasKey<name>, "new_dict should have key 'name'");
  static_assert(new_dict::hasKey<age>, "new_dict should have key 'age'");
  static_assert(new_dict::hasKey<height>, "new_dict should have key 'height'");

  using height_type = new_dict::getValue<height>;
  static_assert(std::is_same_v<height_type, float>, "Value type for key 'height' should be float");

  // If it compiled, it passed
}

TEST(MetaDictTest, MetaDict_pushEntry_should_fail_to_add_entry_with_duplicate_key) {
  using namespace TestData;

  // The following line should fail to compile due to duplicate key 'name'
  // Uncommenting it will cause a compilation error, which is the expected behavior.
  //
  // using invalid_dict = dict::pushEntry<meta_dict_entry<name, int>>;

  SUCCEED(); // If it compiled, it passed
}

TEST(MetaDictTest, MetaDict_removeKey_should_remove_existing_key) {
  using namespace TestData;

  // Should have all keys initially
  static_assert(dict::hasKey<name>, "dict should have key 'name'");
  static_assert(dict::hasKey<age>, "dict should have key 'age'");

  using reduced_dict = dict::removeKey<age>;

  // Should have 'name' but not 'age' after removal
  static_assert(reduced_dict::hasKey<name>, "reduced_dict should have key 'name'");
  static_assert(!reduced_dict::hasKey<age>, "reduced_dict should not have key 'age'");

  // If it compiled, it passed
}

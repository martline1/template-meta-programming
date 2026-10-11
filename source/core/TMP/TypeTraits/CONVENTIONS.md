# Type Traits Implementation Structure & Code Patterns

This document describes the canonical code structure for implementing type traits in the OrchestratorBackend. Each section shows the exact pattern, documentation comments, and naming conventions developers must follow when creating a new type trait.

## What is a Type Trait?

In the OrchestratorBackend, a Type Trait is a template meta function that evaluates to true/false. Usually used
to do boolean logic with types in TMP.

---

## Core Code Structure

Every type trait follows a 4-section pattern. You may implement all sections or a subset, depending on your trait's purpose.

### Section 0: File Header & Includes

**Purpose:** Standard header guard and common includes for type traits.

**Structure:**

```cpp
#pragma once

#include <type_traits>
#include <concepts>

#include "OrchestratorBackend.hpp"
// Additional includes as needed
```

**Key Points:**
- `<type_traits>` — Provides `std::true_type`, `std::false_type`, `std::disjunction_v`, etc.
- `<concepts>` — Required when defining concepts
- `"OrchestratorBackend.hpp"` — Project header with `BEGIN_ORCHESTRATOR` and `END_ORCHESTRATOR` macros
- Additional includes depend on trait requirements (e.g., other type traits, concepts)

---

### Section 1: Primary Template Declaration

**Purpose:** Define the base case (fallback behavior) and establish the trait's interface.

**Structure & Documentation:**

```cpp
/**
 * Less Specialized primary template for [trait_name].
 */
template<typename T>
struct [trait_name]: std::false_type {};
```

**Key Points:**
- Primary template typically inherits from `std::false_type` (default rejection)
- Template parameters define the trait's **interface contract**
- Parameter constraints (e.g., `IsContainerType`) act as **preconditions**
- Do **not** add logic here; that belongs to specializations

---

### Section 2: Specialization

**Purpose:** Define the specific case(s) where the trait evaluates to `true` or contains computation logic.

**Structure & Documentation:**

```cpp
/**
 * Type trait through partial template specialization
 * to [describe what this specialization detects/validates].
 *
 * @tparam [ExplicitParameter1] [Description of explicit parameter 1]
 * @tparam [ExplicitParameter2] [Description of explicit parameter 2]
 * @return true if [condition is met], false otherwise.
 */
template<
  // Deducted from parameters:
  typename... Elements,

  // Parameters:
  IsContainerTemplate TemplateParam
>
struct matches_template<
  TemplateParam<Elements...>,
  TemplateParam
>: std::true_type {};
```

**Documentation Comment Pattern:**
- **Line 1-2:** `"Type trait through partial template specialization to [behavior]."`
- **@tparam lines:** Explain only explicit parameters
- **@return line:** Explain the result condition

**Parameter Organization Comments:**
- `// Deducted from parameter(s):` — Parameters extracted via pattern matching
- `// Parameter(s):` — Explicit parameters provided by caller
- These comments are **required** for trait clarity

**Examples from Codebase:**

From `MatchesTemplate.hpp`:
```cpp
/**
 * Type trait through partial template specialization
 * to check if a type matches a given template template parameter.
 *
 * @tparam TypeParam The type to validate.
 * @tparam TemplateParam The template template parameter to match against.
 * @return true if TypeParam is an instance of TemplateParam container,
 * false otherwise.
 */
template<
  // Deducted from parameters:
  typename... Elements,

  // Parameters:
  template<typename...> typename TemplateParam
>
struct matches_template<
  TemplateParam<Elements...>,
  TemplateParam
>: std::true_type {};
```

From `IsContainerType.hpp`:
```cpp
/**
 * Type trait through partial template specialization
 * to check if a type is a container type.
 *
 * @tparam T The type to validate.
 * @return true if T is a container type, false otherwise.
 */
template<
  // Deducted from parameter:
  template<typename...> typename Container,
  typename... Elements
>
struct is_container_type<
  Container<Elements...>
>: std::true_type {};
```

From `MetaContains.hpp`:
```cpp
/**
 * Type trait through partial template specialization
 * that checks if a given type is present within a container type.
 *
 * @tparam Container The container template to search within.
 * @tparam ToFind The type to search for within the container.
 * @return true if ToFind is present in Container, false otherwise.
 */
template<
  // Deducted from parameters:
  template<typename...> typename Container,
  typename... Elements,

  // Parameters:
  typename ToFind
>
struct meta_contains<
  Container<Elements...>,
  ToFind
> {
  static constexpr auto value =
    std::disjunction_v<
      std::is_same<ToFind, Elements>...
    >;

  meta_contains() = delete; // Prevent usage outside of TMP.
};
```

**Key Points:**
- Comments separating deduced/explicit parameters are **mandatory**
- If specialization contains computation, use `static constexpr` members
- Add `= delete` constructor to signal "this is not a runtime type"
- Use standard algorithms (`std::disjunction_v`, `std::conjunction_v`) for logic

---

### Section 3: Value Helper (`_v` suffix)

**Purpose:** Provide a `constexpr` boolean variable for convenient trait access.

**Structure & Documentation:**

```cpp
/**
 * Constant expression for easier usage of type trait [trait_name].
 *
 * @tparam T [Parameter description].
 * @return true if [condition], false otherwise.
 */
template<typename T>
inline constexpr bool [trait_name]_v = [trait_name]<T>::value;
```

**Documentation Comment Pattern:**
- **Line 1:** `"Constant expression for easier usage of type trait [trait_name]."`
- **@tparam lines:** Explain only explicit parameters
- **@return line:** Explain the result condition

**Examples from Codebase:**

From `IsContainerType.hpp`:
```cpp
/**
 * Constant expression for easier usage of type trait is_container_type.
 *
 * @tparam T The type to validate.
 * @return true if T is a container type, false otherwise.
 */
template<typename T>
static inline constexpr bool is_container_type_v = is_container_type<T>::value;
```

From `MetaContains.hpp`:
```cpp
/**
 * Constant expression for easier usage of type trait meta_contains.
 *
 * @tparam Container The container template to search within.
 * @tparam ToFind The type to search for within the container.
 * @return true if ToFind is present in Container, false otherwise.
 */
template<IsContainerType Container, typename ToFind>
static inline constexpr bool meta_contains_v = meta_contains<Container, ToFind>::value;
```

From `DependentFalse.hpp`:
```cpp
/**
 * Constant expression for easier usage of type trait dependent_false.
 *
 * @tparam T Any template parameters
 * @return Always false.
 */
template<typename... T>
inline constexpr bool dependent_false_v = dependent_false<T...>::value;
```

**Key Points:**
- Always use `static inline constexpr` (C++17+)
- Name must follow `[trait_name]_v` convention
- Delegate to the struct's `.value` member
- Extract template parameters from the primary template signature

---

### Section 4: Concept Wrapper (Optional)

**Purpose:** Provide a C++20 concept for constraint-based trait usage.

**Structure & Documentation:**

```cpp
/**
 * Concept for easier usage of type trait [trait_name].
 *
 * @tparam T [Description of types satisfying this concept].
 * @return true if T satisfies [condition], false otherwise.
 */
template<typename T>
concept [TraitName] = [trait_name]_v<std::remove_cvref_t<T>>;
```

**Documentation Comment Pattern:**
- **Line 1:** `"Concept for easier usage of type trait [trait_name]."`
- **@tparam lines:** Describe what types satisfy the concept
- **@return line:** Describe the condition

**Naming Convention:**
- Concept name uses **PascalCase** (e.g., `IsContainerType`, not `is_container_type`)

**Example from Codebase:**

From `IsContainerType.hpp`:
```cpp
/**
 * Concept for easier usage of type trait is_container_type.
 *
 * @tparam T The type to validate.
 * @return true if T is a container type, false otherwise.
 */
template<typename T>
concept IsContainerType = is_container_type_v<std::remove_cvref_t<T>>;
```

**Key Points:**
- Concept implementation is **always one line** (after documentation)
- Delegate to the `_v` helper
- Apply `std::remove_cvref_t` to strip `const`, `volatile`, and reference qualifiers
- Use **PascalCase** for concept names to distinguish from struct names

---

## Trait Anatomy: What Each Part Is Responsible For

| Component | Responsibility | Who Uses It |
|-----------|-----------------|------------|
| **Primary Template** | Define interface & fallback behavior | Template instantiation, compiler resolution |
| **Specialization** | Pattern match & implement specific cases | SFINAE, template overload resolution |
| **`_v` Helper** | Provide compile-time boolean constant | `if constexpr`, `static_assert`, concepts |
| **Concept** | Enable C++20 constraint syntax | Template constraints, requires clauses |

---

## Naming Conventions (Enforced Across Codebase)

**Trait Struct Names:**
- Format: `snake_case`
- Prefixes: `is_*` (boolean), `has_*` (feature detection), `matches_*` (pattern matching), `dependent_*` (template helpers)
- Examples: `is_container_type`, `is_string_like`, `matches_template`, `meta_contains`, `dependent_false`

**Value Helper:**
- Format: `[struct_name]_v`
- Examples: `is_container_type_v`, `is_string_like_v`, `matches_template_v`, `meta_contains_v`, `dependent_false_v`

**Type Alias (when needed):**
- Format: `[struct_name]_t`
- Examples: `remove_const_t`, `add_pointer_t`

**Concept:**
- Format: **PascalCase** (different from struct!)
- Examples: `IsContainerType`, `StringLike`, `MatchesTemplate`, `IsContainerTemplate`

---

## Parameter Classification Comments

When writing specialization templates, always clarify which parameters are deduced vs. explicit:

```cpp
template<
  // Deducted from parameter(s):        ← Extracted via pattern matching
  template<typename...> typename C,
  typename... E,

  // Parameter(s):                       ← Explicitly provided by caller
  typename X
>
```

**Why This Matters:**
- Deduced parameters show the **pattern structure** being destructured
- Explicit parameters show the **comparison target** or additional context
- This clarity helps developers understand why specialization works

---

## Complete Minimal Example

Here's a complete type trait following the full pattern:

```cpp
#pragma once

#include <type_traits>
#include "OrchestratorBackend.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for my_custom_trait.
 */
template<typename T>
struct my_custom_trait: std::false_type {};

/**
 * Type trait through partial template specialization
 * to check if a type is a std::tuple.
 *
 * @tparam T The type to validate.
 * @return true if T is a std::tuple instantiation, false otherwise.
 */
template<typename... Args>
struct my_custom_trait<std::tuple<Args...>>: std::true_type {};

/**
 * Constant expression for easier usage of type trait my_custom_trait.
 *
 * @tparam T The type to validate.
 * @return true if T is a std::tuple, false otherwise.
 */
template<typename T>
inline constexpr bool my_custom_trait_v = my_custom_trait<T>::value;

/**
 * Concept for easier usage of type trait my_custom_trait.
 *
 * @tparam T The type to validate.
 * @return true if T is a std::tuple, false otherwise.
 */
template<typename T>
concept MyCustomTrait = my_custom_trait_v<std::remove_cvref_t<T>>;

END_ORCHESTRATOR
```

---

## When to Include Each Section

**Always Include:**
- Primary template declaration
- Documentation comments

**Usually Include:**
- Specialization (unless trait is constant)
- `_v` helper (enables compile-time boolean access)

**Optionally Include:**
- Concept wrapper (required if users need C++20 constraint syntax)

---

## Integration Usage Patterns

Once you have a complete trait, users can access it via:

```cpp
// Via concept in constraints
template<MyCustomTrait T>
void process(T value) { /* ... */ }

// Via _v helper
if constexpr (my_custom_trait_v<MyType>) { /* ... */ }

// Via static assertion
static_assert(MyCustomTrait<std::tuple<int, float>>);
```

The consistent structure (`struct`, `_v`, concept) enables all these usage patterns automatically.

<!-- TODO: Implement tests with gtest for all type traits, all must pass a certain concept -->

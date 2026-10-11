<!-- // Instrucciones

Hi copilot hope you're doing great, so this is what I need you to work on:

- Rewrite this document keeping the top comments (current section).
- Provide a Senior Software Engineer level, technical document talking about
  the PATTERNS in the WRITTEN CODE (including comments) for the `TemplateMetaFunctions` dir
  and explain the different ways in which a developer can implement a template metafunction
  for the OrchestratorBackend. -->

# Template Meta Functions Implementation Structure & Code Patterns

This document describes the canonical code structure for implementing template meta functions (TMFs) in the OrchestratorBackend. Each section shows the exact pattern, documentation comments, and naming conventions developers must follow when creating a new template meta function.

## What is a Template Meta Function?

In the OrchestratorBackend, a Template Meta Function (TMF) is a compile-time function that takes types as input and produces a new type as output. Unlike type traits (which evaluate to `true`/`false`), TMFs perform **type transformations**, **type extraction**, or **type manipulation**.

**Key Distinction:**
- **Type Traits**: Return boolean values (`std::true_type`/`std::false_type`)
- **Template Meta Functions**: Return transformed types via `using type = ...`

---

## Core Code Structure

Every template meta function follows a 4-section pattern. You may implement all sections or a subset, depending on your TMF's purpose.

---

### Section 1: Primary Template Declaration

**Purpose:** Define the base case and establish the TMF's interface.

**Structure & Documentation:**

```cpp
/**
 * Less Specialized primary template for [tmf_name].
 */
template<typename T>
struct [tmf_name];
```

**Alternative Pattern (With Constraint):**

```cpp
/**
 * Less Specialized primary template for [tmf_name].
 */
template<IsContainerType Container, typename... Args>
struct [tmf_name];
```

**Key Points:**
- Primary template typically has **no implementation** (forward declaration)
- Template parameters define the TMF's **interface contract**
- Parameter constraints (e.g., `IsContainerType`, `Awaitable`) act as **preconditions**
- Unlike type traits, TMFs don't inherit from `std::true_type`/`std::false_type`
- Implementation happens in specializations

---

### Section 2: Specialization(s)

**Purpose:** Implement the type transformation logic through pattern matching.

**Structure & Documentation:**

```cpp
/**
 * Template Meta Function through partial template specialization
 * to [describe what transformation this performs]. E.g. [example usage].
 *
 * @tparam [ExplicitParam1] [Description of explicit parameter 1].
 * @tparam [ExplicitParam2] [Description of explicit parameter 2].
 * @return [Description of the resulting type].
 */
template<
  // Deducted from parameters:
  IsContainerTemplate SourceContainer,
  typename... SourceElements,

  // Parameters:
  IsContainerTemplate TargetContainer
>
struct [tmf_name]<
  SourceContainer<SourceElements...>,
  TargetContainer
> {
  using type = TargetContainer<SourceElements...>;

  [tmf_name]() = delete;
};
```

**Documentation Comment Pattern:**
- **Line 1-2:** `"Template Meta Function through partial template specialization to [behavior]."`
- **Line 2-3:** Include usage example: `"E.g. meta_transfer<std::tuple<int, float>, std::variant> results in std::variant<int, float>."`
- **@tparam lines:** Explain explicit parameters
- **@return line:** Describe the resulting type (not `true`/`false`)

**Parameter Organization Comments:**
- `// Deducted from parameter(s):` — Parameters extracted via pattern matching
- `// Parameters:` — Explicit parameters provided by caller
- These comments are **required** for TMF clarity

**Examples from Codebase:**

From `MetaTransfer.hpp`:
```cpp
/**
 * Template Meta Function through partial template specialization
 * to transfer the element types from one container type to another
 * container type. E.g. meta_transfer<std::tuple<int, float>, std::variant>
 * results in std::variant<int, float>.
 *
 * @tparam SourceContainer The source container template.
 * @tparam TargetContainer The target container template.
 * @return A target container type containing the
 * element types from the source container.
 */
template<
  // Deducted from parameters:
  IsContainerTemplate SourceContainer,
  typename... SourceElements,

  // Parameters:
  IsContainerTemplate TargetContainer
>
struct meta_transfer<
  SourceContainer<SourceElements...>,
  TargetContainer
> {
  using type = TargetContainer<SourceElements...>;

  meta_transfer() = delete;
};
```

From `MetaPush.hpp`:
```cpp
/**
 * Template Meta Function through partial template specialization
 * that appends one or more types to the end of a given container type.
 *
 * @tparam Container The container template to which types will be appended.
 * @tparam FirstToPush The first type to append to the container.
 * @tparam RestToPush Additional types to append to the container.
 * @return A new container type with the original and items to push appended.
 */
template<
  // Deducted from parameters:
  typename... Elements,

  // Parameters:
  IsContainerType Container,
  typename FirstToPush,
  typename... RestToPush
>
struct meta_push<
  Container<Elements...>,
  FirstToPush,
  RestToPush...
> {
  using type = Container<Elements..., FirstToPush, RestToPush...>;

  meta_push() = delete;
};
```

**Key Points:**
- **`using type = ...`** is the output of the TMF
- Comments separating deduced/explicit parameters are **mandatory**
- Add `= delete` constructor to signal "this is not a runtime type"
- For complex transformations, use private helper members (see MetaOmit example)
- May have multiple specializations for different cases (e.g., base case + recursive case)

---

### Section 2a: Multiple Specializations (Recursive Pattern)

**Purpose:** Handle variadic inputs through recursion.

**Structure:**

```cpp
// Base case: single parameter
template<IsContainerType T>
struct meta_merge<T> {
  using type = T;

  meta_merge() = delete;
};

/**
 * Template Meta Function through partial template specialization
 * to merge multiple container types into a single container type.
 * [Description of merging behavior]
 *
 * @tparam FirstParam The first container type to merge.
 * @tparam SecondParam The second container type to merge.
 * @tparam Rest The remaining container types to merge.
 * @return A container type resulting from merging all input container types.
 */
template<
  IsContainerType FirstParam,
  IsContainerType SecondParam,
  IsContainerType... Rest
>
struct meta_merge<FirstParam, SecondParam, Rest...> {
  using type = typename meta_merge<
    meta_merge_binary_t<FirstParam, SecondParam>,
    Rest...
  >::type;

  meta_merge() = delete;
};
```

**Key Points:**
- Base case handles terminal condition (often 0 or 1 parameters)
- Recursive case reduces problem size
- Use helper TMFs to avoid deep recursion (e.g., `meta_merge_binary_t`)
- Both specializations should have `= delete` constructors

---

### Section 2b: Advanced Pattern with Private Helpers

**Purpose:** Complex transformations requiring intermediate computations.

**Structure:**

```cpp
template<
  // Deducted from parameters:
  typename... Elements,

  // Parameters:
  template<typename...> typename Container,
  typename... ToOmit
>
struct meta_omit<
  Container<Elements...>,
  ToOmit...
> {
  private:
    // Helper: filter each element
    template<typename T>
    using filter =
      std::conditional_t<
        std::disjunction_v<std::is_same<T, ToOmit>...>,
        meta_list<>,
        meta_list<T>
      >;

    // Intermediate result
    using filtered_meta_list = meta_merge_t<filter<Elements>...>;

    // Compile-time conditionals
    static constexpr bool resultIsEmpty = /* ... */;
    static constexpr bool isStdVariant  = /* ... */;

  public:
    using type = std::conditional_t<
      resultIsEmpty && isStdVariant,
      std::variant<std::monostate>,
      meta_transfer_t<filtered_meta_list, Container>
    >;

    meta_omit() = delete;
};
```

**Key Points:**
- Use `private:` section for helper aliases and constexpr values
- Use `public:` section for the final `using type = ...`
- Helper aliases can be template aliases themselves
- Compose existing TMFs (e.g., `meta_merge_t`, `meta_transfer_t`)

---

### Section 3: Type Alias (`_t` suffix)

**Purpose:** Provide convenient access to the TMF's result type.

**Structure & Documentation:**

```cpp
/**
 * Type alias for easier usage of TMF [tmf_name].
 *
 * @tparam [Param1] [Parameter description].
 * @tparam [Param2] [Parameter description].
 * @return [Description of the resulting type].
 */
template<typename T1, IsContainerTemplate T2>
using [tmf_name]_t = typename [tmf_name]<T1, T2>::type;
```

**Documentation Comment Pattern:**
- **Line 1:** `"Type alias for easier usage of TMF [tmf_name]."`
- **@tparam lines:** Describe parameters
- **@return line:** Describe the resulting type

**Examples from Codebase:**

From `MetaTransfer.hpp`:
```cpp
/**
 * Type alias for easier usage of TMF meta_transfer.
 *
 * @tparam T1 The source container type.
 * @tparam T2 The target container template.
 * @return A target container type containing the
 * element types from the source container.
 */
template<typename T1, IsContainerTemplate T2>
using meta_transfer_t = typename meta_transfer<T1, T2>::type;
```

From `MetaPush.hpp`:
```cpp
/**
 * Type alias for easier usage of TMF meta_push.
 *
 * @tparam Container The container template to which types will be appended.
 * @tparam FirstToPush The first type to append to the container.
 * @tparam RestToPush Additional types to append to the container.
 * @return A new container type with the original and items to push appended.
 */
template<IsContainerType Container, typename FirstToPush, typename... RestToPush>
using meta_push_t = typename meta_push<Container, FirstToPush, RestToPush...>::type;
```

**Key Points:**
- Always use `_t` suffix (standard library convention)
- **Always include `typename` keyword** before extracting `::type`
- Extract template parameters from the primary template signature
- This is the **primary interface** users interact with

---

### Section 4: Specialized Type Aliases (Optional)

**Purpose:** Provide domain-specific convenience aliases from meta template meta functions (defined in `TMP/TemplateMetaFunctions/Meta/`) for specific template instantiations.

**Structure & Documentation:**

```cpp
/**
 * Template Meta Function to [specific operation on specific type].
 *
 * @tparam T The [specific type] to [operation].
 * @tparam U... [Additional parameters].
 * @return [Resulting type].
 */
template<MatchesTemplate<meta_list> T, typename... U>
using meta_list_omit_t = meta_omit_t<T, U...>;
```

**Examples from Codebase:**

From `MetaListMethods.hpp`:
```cpp
/**
 * Template Meta Function to merge multiple meta_lists into one.
 *
 * @tparam T... The meta_lists to merge.
 */
template<MatchesTemplate<meta_list>... T>
using meta_list_merge_t = meta_merge_t<T...>;

/**
 * Template Meta Function to omit types from a meta_list.
 *
 * @tparam T The meta_list to omit from.
 * @tparam U... The types to omit.
 */
template<MatchesTemplate<meta_list> T, typename... U>
using meta_list_omit_t = meta_omit_t<T, U...>;

/**
 * Template Meta Function to push types to a meta_list.
 *
 * @tparam T The meta_list to push to.
 * @tparam First The first type to push.
 * @tparam Rest Optional types to push.
 */
template<MatchesTemplate<meta_list> T, typename First, typename... Rest>
using meta_list_push_t = meta_push_t<T, First, Rest...>;
```

From `VariantMethods.hpp`:
```cpp
/**
 * Template Meta Function to merge multiple variants into one.
 *
 * @tparam T... The variants to merge.
 */
template<MatchesTemplate<std::variant>... T>
using variant_merge_t = meta_merge_t<T...>;

/**
 * Template Meta Function to omit types from a variant.
 *
 * @tparam T The variant to omit from.
 * @tparam U... The types to omit.
 */
template<MatchesTemplate<std::variant> T, typename... U>
using variant_omit_t = meta_omit_t<T, U...>;
```

From `MetaDictGetValue.hpp`:
```cpp
/**
 * Template Meta Function through partial template specialization
 * to get the value type associated with a given key type
 * from a meta_dict.
 *
 * @tparam Entries A meta list of meta dict entries.
 * @tparam Key The key type whose associated value type is to be retrieved.
 * @return The value type associated with the provided key type.
 */
template<
  // Deducted from parameters:
  typename... Entries,

  // Parameters:
  typename Key
>
requires (MatchesTemplate<Entries, meta_dict_entry> && ...)
struct meta_dict_get_value<meta_list<Entries...>, Key> {
  private:
    template<typename Entry>
    static auto filter() -> std::conditional_t<
      !std::is_same_v<typename Entry::key, Key>,
      meta_list<>,
      meta_list<typename Entry::value>
    >;

    using filterResults = meta_merge_t<decltype(filter<Entries>())...>;

  public:
    using type = typename meta_transfer_t<filterResults, meta_mono>::type;

    meta_dict_get_value() = delete;
};
```

**Key Points:**
- These are **wrappers** around generic TMFs
- Use concepts/traits to constrain to specific types (`MatchesTemplate<meta_list>`, `MatchesTemplate<std::variant>`)
- Provide better error messages and semantic clarity
- Optional, but useful for frequently-used specializations
- Organized into domain-specific modules:
  - `MetaListMethods/` — Operations on `meta_list`
  - `MetaDictMethods/` — Operations on `meta_dict` 
  - `VariantMethods/` — Operations on `std::variant`

---

## TMF Anatomy: What Each Part Is Responsible For

| Component | Responsibility | Who Uses It |
|-----------|-----------------|------------|
| **Primary Template** | Define interface & forward declaration | Template instantiation, compiler resolution |
| **Specialization(s)** | Pattern match & implement type transformation | SFINAE, template overload resolution |
| **`_t` Alias** | Provide convenient access to result type | Type aliases, other TMFs, user code |
| **Specialized Aliases** | Domain-specific convenience wrappers | User code requiring semantic clarity |

---

## Naming Conventions (Enforced Across Codebase)

**TMF Struct Names:**
- Format: `snake_case`
- Prefixes: `meta_*` (type container operations), `get_*` (type extraction)
- Examples: `meta_transfer`, `meta_merge`, `meta_push`, `get_awaiter`

**Type Alias:**
- Format: `[struct_name]_t`
- Examples for `meta_list`: `meta_list_merge_t`, `meta_list_omit_t`, `meta_list_push_t`
- Examples for `variant`: `variant_merge_t`, `variant_omit_t`
- Examples for `meta_dict`: `meta_dict_get_value_t`, `meta_dict_remove_key_t`

**Specialized Aliases:**
- Format: `[type]_[operation]_t`
- Examples: `meta_list_merge_t`, `meta_list_omit_t`, `meta_list_push_t`

---

## Parameter Classification Comments

When writing specialization templates, always clarify which parameters are deduced vs. explicit:

```cpp
template<
  // Deducted from parameter(s):        ← Extracted via pattern matching
  template<typename...> typename C,
  typename... E,

  // Parameters:                         ← Explicitly provided by caller
  typename X
>
```

**Why This Matters:**
- Deduced parameters show the **pattern structure** being destructured
- Explicit parameters show the **target** or **additional context**
- This clarity helps developers understand why specialization works

---

## Complete Minimal Example

Here's a complete TMF following the full pattern:

```cpp
#pragma once

#include "OrchestratorBackend.hpp"
#include "IsContainerType/IsContainerType.hpp"

BEGIN_ORCHESTRATOR

/**
 * Less Specialized primary template for meta_duplicate.
 */
template<IsContainerType Container>
struct meta_duplicate;

/**
 * Template Meta Function through partial template specialization
 * to duplicate all element types in a container. E.g.
 * meta_duplicate<std::tuple<int, float>> results in
 * std::tuple<int, int, float, float>.
 *
 * @tparam Container The container type to duplicate elements from.
 * @return A container with each element duplicated.
 */
template<
  // Deducted from parameters:
  template<typename...> typename Container,
  typename... Elements
>
struct meta_duplicate<Container<Elements...>> {
  using type = Container<Elements..., Elements...>;

  meta_duplicate() = delete;
};

/**
 * Type alias for easier usage of TMF meta_duplicate.
 *
 * @tparam Container The container type to duplicate elements from.
 * @return A container with each element duplicated.
 */
template<IsContainerType Container>
using meta_duplicate_t = typename meta_duplicate<Container>::type;

END_ORCHESTRATOR
```

---

## Pattern Categories

### Pattern 1: Type Transfer/Conversion

**Purpose:** Move element types from one container template to another.

**Example:** `meta_transfer`

**Structure:**
- Extract elements from source container
- Instantiate target container with extracted elements

```cpp
using type = TargetContainer<SourceElements...>;
```

---

### Pattern 2: Type Appending/Prepending

**Purpose:** Add types to a container.

**Example:** `meta_push`

**Structure:**
- Extract existing elements
- Append new types to parameter pack

```cpp
using type = Container<Elements..., NewTypes...>;
```

---

### Pattern 3: Type Filtering

**Purpose:** Remove specific types from a container.

**Example:** `meta_omit`

**Structure:**
- Use helper template alias to filter each element
- Merge filtered results
- Transfer back to original container type

```cpp
template<typename T>
using filter = std::conditional_t<condition, meta_list<>, meta_list<T>>;
using filtered = meta_merge_t<filter<Elements>...>;
using type = meta_transfer_t<filtered, Container>;
```

---

### Pattern 4: Recursive Reduction

**Purpose:** Process variadic inputs through recursion.

**Example:** `meta_merge`

**Structure:**
- Base case: single parameter
- Recursive case: process first two, recurse on rest

```cpp
// Base case
template<T>
struct tmf<T> { using type = T; };

// Recursive case
template<T1, T2, Rest...>
struct tmf<T1, T2, Rest...> {
  using type = typename tmf<binary_op_t<T1, T2>, Rest...>::type;
};
```

---

### Pattern 5: Type Extraction (SFINAE/Requires)

**Purpose:** Extract a type from an expression or trait.

**Example:** `get_awaiter`, `get_awaiter_result`

**Structure:**
- Use `decltype` on expressions
- Use `requires` expressions for conditional extraction
- Use `std::declval` for fake instances

```cpp
template<Awaitable T>
struct get_awaiter_result {
  using type = decltype(
    std::declval<get_awaiter_t<T>>().await_resume()
  );
};
```

---

## Advanced Techniques

### Technique 1: Using `std::conditional_t` for Compile-Time Branching

```cpp
using type = std::conditional_t<
  condition,
  TypeIfTrue,
  TypeIfFalse
>;
```

### Technique 2: Using `decltype` for Type Extraction

```cpp
using type = decltype(some_expression());
```

### Technique 3: Composing Multiple TMFs

```cpp
using intermediate = meta_merge_t<filter<Elements>...>;
using type = meta_transfer_t<intermediate, Container>;
```

### Technique 4: Using Private Helpers for Readability

```cpp
private:
  template<typename T>
  using helper = /* complex expression */;
  
  using intermediate = helper<Elements>...;

public:
  using type = FinalTransform<intermediate>;
```

---

## When to Include Each Section

**Always Include:**
- Primary template declaration
- At least one specialization with `using type = ...`
- Type alias with `_t` suffix
- Documentation comments

**Usually Include:**
- `= delete` constructor in specializations
- Parameter classification comments

**Optionally Include:**
- Multiple specializations (base case + recursive)
- Private helper aliases
- Specialized convenience aliases (e.g., `meta_list_*_t`)

---

## Common Pitfalls

### Pitfall 1: Forgetting `typename` Keyword

**Problem:**
```cpp
template<typename T>
using my_alias = meta_transfer<T, std::variant>::type;  // ❌ Missing typename
```

**Solution:**
```cpp
template<typename T>
using my_alias = typename meta_transfer<T, std::variant>::type;  // ✅
```

---

### Pitfall 2: Not Using `= delete` Constructor

**Problem:**
```cpp
struct meta_push</* ... */> {
  using type = /* ... */;
  // Missing = delete constructor
};
```

**Solution:**
```cpp
struct meta_push</* ... */> {
  using type = /* ... */;
  meta_push() = delete;  // ✅ Prevent instantiation
};
```

---

### Pitfall 3: Infinite Recursion

**Problem:**
```cpp
template<typename T, typename... Rest>
struct tmf<T, Rest...> {
  using type = typename tmf<T, Rest...>::type;  // ❌ Infinite recursion
};
```

**Solution:**
```cpp
// Add base case
template<typename T>
struct tmf<T> {
  using type = T;  // ✅ Base case stops recursion
};

template<typename T, typename... Rest>
struct tmf<T, Rest...> {
  using type = typename tmf<Rest...>::type;  // ✅ Reduces problem size
};
```

---

## Integration Usage Patterns

Once you have a complete TMF, users can access it via:

```cpp
// Via _t alias (most common)
using Result = meta_transfer_t<std::tuple<int, float>, std::variant>;

// Via struct::type (explicit)
using Result = typename meta_transfer<std::tuple<int, float>, std::variant>::type;

// In other TMFs (composition)
template<typename T>
using my_transform = meta_transfer_t<meta_merge_t<T, U>, std::variant>;

// In template parameters
template<typename T>
void process(meta_push_t<T, int> container) { /* ... */ }
```

The consistent structure (`struct`, `_t` alias) enables all these usage patterns automatically.

---

## Relationship to Type Traits

**Type Traits vs Template Meta Functions:**

| Aspect | Type Traits | Template Meta Functions |
|--------|-------------|------------------------|
| **Output** | Boolean (`std::true_type`/`std::false_type`) | Type (`using type = ...`) |
| **Purpose** | Test properties of types | Transform/manipulate types |
| **Helper** | `_v` suffix (boolean) | `_t` suffix (type) |
| **Example** | `is_container_type_v<T>` | `meta_transfer_t<T, U>` |
| **Usage** | `if constexpr`, concepts, `static_assert` | Type aliases, template parameters |

**They complement each other:**
- Type traits **constrain** what types TMFs can accept
- TMFs **transform** types that satisfy trait constraints

```cpp
template<IsContainerType T>  // ← Type trait as constraint
using Result = meta_transfer_t<T, std::variant>;  // ← TMF as transformation
```

<!-- TODO: Create MetaUnshift -->
<!-- TODO: Create MetaUnshiftUnique -->

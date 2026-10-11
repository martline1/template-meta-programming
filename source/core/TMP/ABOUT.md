# Template Meta Programming (TMP) System

## Overview

The OrchestratorBackend's Template Meta Programming (TMP) system is a compile-time computation framework that enables sophisticated type manipulation, validation, and transformation. The system consists of four complementary subsystems that work together to provide a complete meta-programming infrastructure.

---

## The Four Pillars of TMP

### 1. Type Traits (`TypeTraits/`)

**Purpose:** Validate and test type properties at compile-time.

**Key Characteristics:**
- Return boolean values via `std::true_type`/`std::false_type`
- Enable conditional compilation via `if constexpr` and concepts
- Act as "gatekeepers" for type constraints
- Provide type inspection capabilities

**Usage Pattern:**
```cpp
template<IsContainerType T>  // ← Type trait as constraint
void process(T container) { /* ... */ }

if constexpr (is_container_type_v<MyType>) { /* ... */ }
```

**Reference:** See [TypeTraits/CONVENTIONS.md](TypeTraits/CONVENTIONS.md)

**Available Type Traits:**
- `is_container_type` / `IsContainerType` — Validates container types (e.g., `std::tuple`, `std::variant`)
- `is_container_template` / `IsContainerTemplate` — Validates template template parameters
- `matches_template` / `MatchesTemplate` — Checks if a type matches a given template
- `is_template_template_equal` / `IsTemplateTemplateEqual` — Compares template templates for equality
- `meta_contains` / `MetaContains` — Checks if a type is contained in a container
- `is_string_like` / `IsStringLike` — Validates string-like types
- `is_duration` / `IsDuration` — Validates `std::chrono::duration` types
- `dependent_false` — Always-false type trait for `static_assert` in templates

---

### 2. Template Meta Functions (`TemplateMetaFunctions/`)

**Purpose:** Transform and manipulate types at compile-time.

**Key Characteristics:**
- Return new types via `using type = ...`
- Implement algorithms on type sequences
- Enable type composition and transformation pipelines
- Build complex type relationships

**Usage Pattern:**
```cpp
// Transfer elements from one container to another
using result = meta_transfer_t<std::tuple<int, float>, std::variant>;

// Merge multiple containers
using merged = meta_merge_t<std::tuple<int>, std::variant<float>>;

// Filter types from a container
using filtered = meta_omit_t<std::tuple<int, float, bool>, float>;

// Domain-specific operations (organized by data structure)
using list_merged = meta_list_merge_t<meta_list<int>, meta_list<float>>;
using variant_filtered = variant_omit_t<std::variant<int, float>, float>;
using dict_value = meta_dict_get_value_t<MyDict, KeyType>;
```

**Reference:** See [TemplateMetaFunctions/CONVENTIONS.md](TemplateMetaFunctions/CONVENTIONS.md)

**Organization:**
- `Meta/` — Generic algorithms working on arbitrary container types
- `Common/` — Type extraction utilities (awaiter, awaiter result, etc.)
- `MetaListMethods/` — Specialized operations for `meta_list`
- `MetaDictMethods/` — Specialized operations for `meta_dict`
- `VariantMethods/` — Specialized operations for `std::variant`

---

### 3. Template Meta Data Structures (`TemplateMetaDataStructures/`)

**Purpose:** Hold and organize type information during transformations.

**Key Characteristics:**
- Core structures: `meta_list<T...>`, `meta_dict<...>`, `meta_mono<T>`
- `meta_list` serves as **universal intermediate representation** for TMF algorithms
- `meta_dict` provides compile-time key-value mapping
- `meta_mono` wraps single types (the "unit" monad)
- Enables algorithm generalization across different container types
- Decouples algorithm logic from container-specific details

**Usage Pattern:**
```cpp
// Internal usage in TMFs
// Input type → Extract to meta_list → Algorithm → Convert to output type

// Public interface (rarely used directly):
using types = meta_list<int, float, bool>;
using result = meta_list_to_variant_t<types>;  // meta_list → std::variant

// meta_dict for compile-time key-value storage
using my_dict = meta_dict<
  meta_dict_entry<int, std::string>,
  meta_dict_entry<float, std::vector<int>>
>;
using value = meta_dict_get_value_t<my_dict, int>;  // std::string

// meta_mono for single-type wrapping
using wrapped = meta_mono<int>;
using unwrapped = wrapped::type;  // int
```

**Reference:** See [TemplateMetaDataStructures/ABOUT.md](TemplateMetaDataStructures/ABOUT.md)

---

### 4. Concepts (`Concepts/`)

**Purpose:** Provide semantic constraints and category-based type validation.

**Key Characteristics:**
- Enable C++20 requires expressions and constraints
- Organize concepts by category (common, coroutines, methods, strings)
- Provide semantic validation beyond structural pattern matching
- Allow intuitive constraint syntax in template declarations

**Usage Pattern:**
```cpp
template<CommonConcepts::VoidLike T>
void processResult(T result) { /* ... */ }

template<CoroConcepts::Awaitable T>
auto getResult(T task) { /* ... */ }

template<StringConcepts::StringLike T>
void logMessage(T message) { /* ... */ }

// Meta-programming debugging
template<MetaConcepts::MetaPrintType T>  // Compile error shows type T
void debug() { }
```

**Categories:**
- `CommonConcepts/` — General type categories (void-like, enum, pipe callbacks, etc.)
- `CoroConcepts/` — Coroutine and async-related concepts
- `MetaConcepts/` — Meta-programming utilities (type printing, duplicate detection)
- `MethodConcepts/` — Method and callable semantics
- `StringConcepts/` — String and string-like types

**Key Concepts:**
- `CommonConcepts::VoidLike` — Validates void or void-returning async types
- `CommonConcepts::IsEnum` — Validates enumeration types
- `CommonConcepts::PipeCallback` — Validates pipeline callback signatures
- `CoroConcepts::Awaitable` — Validates awaitable coroutine types
- `MetaConcepts::MetaPrintType` — Debugging tool to print types via compiler errors
- `MetaConcepts::HasNoDuplicates` / `HasDuplicates` — Detects duplicate types in parameter packs
- `StringConcepts::StringLike` — Validates string-like types

---

## The Complete TMP Stack

### Architecture Diagram

```
  ┌──────────────────────────────────────────────────────────┐
  │                 User Code / Application                  │
  └─────┬───────────────────────┬───────────────────────┬────┘
        │                       │                       │
        ▼                       ▼                       ▼
  ┌───────────┐          ┌─────────────┐          ┌────────────┐
  │   TMFs    │ ◀──────▶ │ Type Traits │ ◀──────▶ │  Concepts  │
  └─────┬─────┘          └──────┬──────┘          └─────┬──────┘
        ▼                       ▼                       ▼
        │                       │                       │
        └───────────┬───────────┴───────────┬───────────┘
                    │   Shared Resources    │
                    ▼                       ▼
            ┌─────────────┐         ┌──────────────┐
            │    TMDSs    │ ◀─────▶ │ std Library  │
            │ meta_list   │         │ tuple        │
            │ meta_dict   │         │ variant      │
            │ meta_mono   │         │ etc          │
            └─────────────┘         └──────────────┘
```

---

## Key Documentation Files

For detailed implementation guidance, refer to:

1. **[TypeTraits/CONVENTIONS.md](TypeTraits/CONVENTIONS.md)**
   - How to implement new type traits
   - Documentation patterns
   - Code structure guidelines
   - Examples and pitfalls

2. **[TemplateMetaFunctions/CONVENTIONS.md](TemplateMetaFunctions/CONVENTIONS.md)**
   - How to implement new TMFs
   - Pattern categories (transfer, append, filter, recursive, extraction)
   - Advanced techniques with private helpers
   - Specialized type aliases for domain-specific operations
   - Integration patterns

3. **[TemplateMetaDataStructures/ABOUT.md](TemplateMetaDataStructures/ABOUT.md)**
   - What `meta_list` is and why it exists
   - The TMDS sandwich pattern
   - TMDS benefits and design principles
   - Current implementations

---

## Design Principles

### 1. Separation of Concerns

Each subsystem has a distinct responsibility:

| Subsystem | Responsibility | Output |
|-----------|-----------------|--------|
| **Type Traits** | Answer "does this type satisfy a property?" | Boolean (`true_type`/`false_type`) |
| **TMFs** | Transform types through algorithms | Type (via `using type = ...`) |
| **TMDSs** | Hold and organize type information | Type structure (`meta_list`, `meta_dict`, `meta_mono`) |
| **Concepts** | Constrain and validate template parameters | Boolean (compile-time check) |

### 2. Composition Over Duplication

TMFs are built on TMDS to enable single-implementation algorithms:

```cpp
// Without TMDS: Would need separate implementations for tuple, variant, custom types
template<typename T> using omit_from_tuple_v = /* tuple-specific */;
template<typename T> using omit_from_variant_v = /* variant-specific */;
template<typename T> using omit_from_custom_v = /* custom-specific */;

// With TMDS: Single implementation works for all containers
template<typename Container, typename... ToOmit>
using meta_omit_t = /* works on any container via meta_list */;
```

### 3. Type Safety Through Constraints

Type traits gate what types TMFs will accept:

```cpp
// Only containers can be omitted
template<typename Container, typename... ToOmit>
struct meta_omit<Container, ToOmit...>
  requires IsContainerType<Container>
{ /* ... */ };
```

### 4. Composability

Complex transformations are built from simpler operations:

```cpp
// Three-step transformation
using step1 = meta_push_t<meta_list<int>, float>;
using step2 = meta_omit_t<step1, float>;
using step3 = meta_list_to_variant_t<step2>;
// Result: std::variant<int>
```

---

## Real-World Example: Type Filtering and Conversion

**Goal:** Convert `std::tuple<int, float, bool>` to `std::variant<int, bool>` (removing float).

**Implementation Chain:**

```cpp
using input = std::tuple<int, float, bool>;

// Step 1: Use TMF to omit float (internally converts to meta_list)
using filtered = meta_omit_t<input, float>;
// Internally: tuple → meta_list<int, bool> → back to tuple

// Step 2: Use TMF to transfer to variant
using output = meta_transfer_t<filtered, std::variant>;
// Internally: tuple → meta_list<int, bool> → std::variant<int, bool>

static_assert(std::is_same_v<output, std::variant<int, bool>>);
```

**How Type Traits Participate:**

```cpp
// Type traits validate preconditions
static_assert(IsContainerType<decltype(input)>);     // ✅ Pass
static_assert(IsContainerType<std::variant<>>);      // ✅ Pass
static_assert(!IsContainerType<int>);                // ✅ Correctly rejects
```

**Complex Variant Example:**

```cpp
// Combine multiple variants and filter specific types
using IntegerVariant = std::variant<int, unsigned int, long, unsigned long>;
using FloatingPointVariant = std::variant<float, double, long double>;

using IdVariant = variant_omit_t<
  variant_merge_t<
    IntegerVariant,                             // integers
    FloatingPointVariant,                       // floating points
    std::variant<std::string, std::string_view> // strings
  >,
  long,
  long double,
  unsigned long
>;

// Result: std::variant<int, unsigned int, float, double, std::string, std::string_view>
```

**Meta Dictionary Example:**

```cpp
// Define type mappings for serialization
using SerializerMap = meta_dict<
  meta_dict_entry<int, IntSerializer>,
  meta_dict_entry<float, FloatSerializer>,
  meta_dict_entry<std::string, StringSerializer>
>;

// Retrieve serializer for a specific type
template<typename T>
using serializer_for = meta_dict_get_value_t<SerializerMap, T>;

// Usage
using IntSerializerType = serializer_for<int>;  // IntSerializer
```

---

## When to Use Each Subsystem

### Use Type Traits When...

- Validating type properties (is it a container? does it match a template?)
- Constraining template parameters (`template<IsContainerType T>`)
- Making compile-time decisions (`if constexpr (is_container_type_v<T>)`)
- Writing concepts for generic code

**Example:**
```cpp
template<typename T>
concept ProcessableType = IsContainerType<T> && !std::is_const_v<T>;

template<ProcessableType T>
void process(T& container) { /* ... */ }
```

### Use Template Meta Functions When...

- Transforming types (extract elements, append, merge, filter)
- Building type pipelines (input → algorithm → output)
- Converting between container types
- Composing type relationships

**Example:**
```cpp
// Extract handlers from coroutine types
using handler_types = meta_merge_t<
  get_awaiter_t<Task1>,
  get_awaiter_t<Task2>
>;

// Convert to variant for processing
using handler_variant = meta_list_to_variant_t<handler_types>;
```

### Use Template Meta Data Structures When...

- You're implementing new TMFs
- You need an abstraction over container types
- You're building intermediate type representations
- You're composing multiple TMF operations

**Example (TMF Implementation):**
```cpp
template<typename Container, typename... ToOmit>
struct meta_omit {
  // Internal: Work with meta_list abstraction
  using filtered = meta_omit_impl<meta_list<...>, ToOmit...>;

  // Output: Convert back to original container
  using type = meta_transfer_t<filtered, Container>;
};
```

### Use Concepts When...

- Constraining template parameters with semantic meaning
- Enabling overload resolution based on type properties
- Improving error messages with clear constraint violations
- Debugging compile-time code (e.g., `MetaPrintType`)
- Checking for duplicate types in parameter packs

**Example:**
```cpp
template<CoroConcepts::Awaitable T>
auto processTask(T task) { /* Only accepts awaitable types */ }

template<typename... Ts>
  requires MetaConcepts::HasNoDuplicates<Ts...>
using UniqueVariant = std::variant<Ts...>;

// Debugging: Intentionally fail to see type
template<MetaConcepts::MetaPrintType T>
void show_type();  // Compiler error reveals T
```

---

## Integration Points

### With Concepts (C++20)

```cpp
template<typename T>
concept Awaitable = requires(T t) {
  { get_awaiter_t<T>() };
};

template<Awaitable T>
using result_type = get_awaiter_result_t<T>;
```

### With SFINAE (Pre-C++20)

```cpp
template<
  typename T,
  std::enable_if_t<is_container_type_v<T>, int> = 0
>
auto process(T container) { /* ... */ }
```

### With Type Aliases

```cpp
template<typename T>
using filtered_type = meta_omit_t<T, std::monostate>;

template<typename... Ts>
using variant_from_types = meta_list_to_variant_t<meta_list<Ts...>>;
```

---

## Performance Characteristics

All TMP operations are **compile-time only** with **zero runtime overhead**:

| Operation | When | Overhead |
|-----------|------|----------|
| Type trait evaluation | Compile-time | None (erased) |
| TMF instantiation | Compile-time | None (produces type) |
| TMDS operations | Compile-time | None (pure type manipulation) |
| Runtime type checks | Runtime | Only what you explicitly add |

**Key Insight:** Using `meta_omit_t<T, float>` has zero runtime cost—it's purely compile-time type generation.

---

## Common Patterns

### Pattern 0: Debugging TMP Code

**Purpose:** Print types at compile-time for debugging.

```cpp
// Force compiler to show the actual type in error message
template<MetaConcepts::MetaPrintType T>
void debug_type() { }

// Usage: Compiler will show "T = std::variant<int, float>" in error
debug_type<std::variant<int, float>>();

// Check for duplicates in type packs
static_assert(MetaConcepts::HasNoDuplicates<int, float, bool>);
static_assert(!MetaConcepts::HasNoDuplicates<int, float, int>);  // Duplicate int
```

---

### Pattern 1: Filter and Convert

```cpp
using filtered = meta_omit_t<std::tuple<int, float, bool>, float>;
using as_variant = meta_transfer_t<filtered, std::variant>;
```

### Pattern 2: Merge Container Types

```cpp
using merged = meta_merge_t<std::tuple<int>, std::variant<float>>;
// Result: Either std::tuple<int, float> or meta_list<int, float>
// depending on template matching
```

### Pattern 3: Extract and Augment

```cpp
template<typename Task>
using task_handlers = meta_push_t<
  get_awaiter_t<Task>,
  std::exception_ptr  // Add error handler
>;
```

### Pattern 4: Conditional Type Building

```cpp
template<typename T>
using augmented = std::conditional_t<
  is_container_type_v<T>,
  meta_push_t<T, std::monostate>,
  T
>;
```

---

## Future Extensions

The TMP system is designed to be extensible:

- **New Type Traits:** Add to `TypeTraits/` following [CONVENTIONS.md](TypeTraits/CONVENTIONS.md)
- **New TMFs:** Add to `TemplateMetaFunctions/Meta/` following [CONVENTIONS.md](TemplateMetaFunctions/CONVENTIONS.md)
- **New Concepts:** Add to appropriate `Concepts/` subdirectory (Common, Coro, Meta, Method, String)
- **New Container Types:** Implement `meta_list ↔ NewContainer` conversion via TMFs
- **New Data Structures:** Extend `TemplateMetaDataStructures/` as needed
- **Domain-Specific Methods:** Add specialized aliases in `*Methods/` directories

---

## Conclusion

The OrchestratorBackend's TMP system provides a powerful, composable framework for compile-time type manipulation:

- **Type Traits** validate what types can be processed
- **Template Meta Functions** define how types are transformed
- **Template Meta Data Structures** provide the abstraction layer enabling algorithm generalization
- **Concepts** constrain and document type requirements with clear semantics

Together, they enable writing generic, type-safe, zero-overhead meta-programs that work across arbitrary container types and complex type relationships. The system includes debugging utilities (`MetaPrintType`, `HasDuplicates`) and domain-specific convenience methods for common operations on `meta_list`, `meta_dict`, and `std::variant`.

# Template Meta Data Structures (TMDS)

## What is a Template Meta Data Structure?

A Template Meta Data Structure (TMDS) is a **compile-time data structure** that holds and organizes type information, enabling manipulation and analysis of types during template instantiation. Unlike runtime data structures (arrays, lists, maps), TMDSs exist purely in the type system and are resolved at compile-time through template specialization.

In the OrchestratorBackend, there are three core TMDS types:
- **`meta_list`** — An ordered sequence of types (primary intermediate representation)
- **`meta_dict`** — A compile-time key-value mapping structure
- **`meta_mono`** — A single-type holder (the "unit" monad)

---

## Core TMDS Structures

### 1. `meta_list` — Ordered Type Sequence

#### Definition

```cpp
/**
 * Template Meta Data Structure that holds a variadic list of types.
 * This acts as an ordered sequence of types.
 * 
 * @note Because this is an ordered sequence,
 * meta_list<int, double> IS NOT the same as meta_list<double, int>
 */
template<typename... T>
struct meta_list {
  static constexpr size_t size = sizeof...(T);
  meta_list() = delete; // Prevent usage outside of TMP
};
```

#### Characteristics

- **No runtime members** — purely a compile-time type container
- **Ordered sequence** — preserves element order (important for semantics)
- **Variadic** — accepts any number of types as template parameters
- **Size tracking** — provides compile-time `size` constant
- **Inert** — contains no operations; only used as intermediate representation
- **Neutral** — type-agnostic; works with any types

#### Usage Examples

```cpp
using empty_list = meta_list<>;                   // Empty list
using int_list = meta_list<int, float, bool>;     // List of 3 types
static_assert(int_list::size == 3);               // Size query
```

---

### 2. `meta_dict` — Compile-Time Key-Value Map

#### Definition

```cpp
/**
 * Template Meta Data Structure that holds key-value pairs.
 * Each entry is a meta_dict_entry<KeyType, ValueType>.
 */
template<MatchesTemplate<meta_dict_entry>... EntryTypes>
struct meta_dict {
  template<typename KeyType>
  static constexpr bool hasKey = /* ... */;
  
  template<typename KeyType>
  using getValue = /* ... */;
  
  template<MatchesTemplate<meta_dict_entry> Entry>
  using pushEntry = /* ... */;
  
  template<typename KeyType>
  using removeKey = /* ... */;
  
  meta_dict() = delete;
};
```

#### Characteristics

- **Key-value mapping** — associates type keys with type values
- **Unique keys** — enforces no duplicate keys at compile-time
- **Type-safe lookup** — `getValue<KeyType>` retrieves associated value
- **Immutable operations** — `pushEntry`/`removeKey` return new `meta_dict` types
- **Query support** — `hasKey<KeyType>` checks key existence

#### Entry Structure

```cpp
template<typename KeyType, typename ValueType>
struct meta_dict_entry {
  using key = KeyType;
  using value = ValueType;
  meta_dict_entry() = delete;
};
```

#### Usage Examples

```cpp
// Define entries
using entry1 = meta_dict_entry<int, std::string>;
using entry2 = meta_dict_entry<float, std::vector<int>>;

// Create dictionary
using my_dict = meta_dict<entry1, entry2>;

// Query operations
static_assert(my_dict::hasKey<int>);              // true
static_assert(!my_dict::hasKey<bool>);            // false

// Retrieve value
using value_for_int = my_dict::getValue<int>;     // std::string

// Add entry (returns new type)
using entry3 = meta_dict_entry<bool, char>;
using extended_dict = my_dict::pushEntry<entry3>;

// Remove entry (returns new type)
using smaller_dict = my_dict::removeKey<float>;
```

---

### 3. `meta_mono` — Single-Type Holder

#### Definition

```cpp
/**
 * Template Meta Data Structure that holds a required single type.
 * This acts as the "unit" part of a Monad pattern.
 */
template<typename T>
struct meta_mono {
  using type = T;
  meta_mono() = delete;
};
```

#### Characteristics

- **Single type holder** — wraps exactly one type
- **Monadic unit** — represents the simplest container (contains one element)
- **Type accessor** — exposes `type` member for wrapped type
- **Semantic marker** — distinguishes "single required type" from bare types

#### Usage Examples

```cpp
using wrapped_int = meta_mono<int>;
using unwrapped = wrapped_int::type;              // int

// Used in algorithms requiring uniform structure
template<typename T>
using ensure_mono = std::conditional_t<
  is_meta_mono_v<T>,
  T,
  meta_mono<T>
>;
```

### Why Not Use `std::tuple` or `std::variant`?

**Problem:** Different container templates have different semantics:
- `std::tuple<T...>` — runtime aggregation of values
- `std::variant<T...>` — runtime type union with overhead
- `std::array<T, N>` — fixed-size, homogeneous collection

**Solution:** `meta_list` is **semantically neutral**:
- Not tied to runtime behavior
- Not tied to specific container semantics
- Can be converted to/from any container template
- Serves as a **universal intermediate representation**

---

## The Role of TMDS in Template Meta Programming

### 1. Type Transformation Pipeline

TMDS enables a pipeline where types flow through TMFs:

```
Input Type → Extract Elements → TMDS → Filter/Transform → TMDS → Convert → Output Type
  ↓              ↓                ↓           ↓              ↓         ↓
std::tuple   (Elements...)   meta_list   meta_omit_t    meta_list  std::variant
```

### 2. Intermediate Representation

TMDSs decouple the **structure of the algorithm** from the **implementation details**:

```cpp
// Without TMDS: Algorithm must handle each container type separately
template<typename T> using omit_from_tuple = /* tuple-specific logic */;
template<typename T> using omit_from_variant = /* variant-specific logic */;
template<typename T> using omit_from_custom = /* custom-specific logic */;

// With TMDS: Algorithm works on a universal representation
template<typename T>
using omit_from_any = meta_transfer_t<
  meta_omit_t<meta_list<...>, ToRemove...>,  // Work on meta_list
  Container                                   // Convert back to original
>;
```

### 3. Type Element Extraction and Manipulation

TMDS allows arbitrary operations on type sequences:

```cpp
// Extract elements from any container
std::tuple<int, float, bool> → meta_list<int, float, bool>

// Perform operations (all work on meta_list internally)
- Filter: meta_omit_t<meta_list<int, float, bool>, float>
- Append: meta_push_t<meta_list<int, float>, bool>
- Merge: meta_merge_t<meta_list<int>, meta_list<float>>

// Convert back to target container
meta_list<int, bool> → std::variant<int, bool>
```

---

## Design Pattern: The TMDS Sandwich

All TMDS-based algorithms follow this pattern:

```
Container Type → Destructure → TMDS → Algorithm → TMDS → Reconstruct → Result Type
```

**Example: `meta_omit`**

```cpp
template<typename Container, typename... ToOmit>
using meta_omit_t = /* ... */;
```

**Execution Flow:**

1. **Input:** `std::variant<int, float, bool>`
2. **Destructure:** Extract `int, float, bool` → `meta_list<int, float, bool>`
3. **Algorithm:** Filter `meta_list` based on `ToOmit` constraints
4. **Result:** `meta_list<int, bool>`
5. **Reconstruct:** Convert `meta_list<int, bool>` back to `std::variant<int, bool>`

---

## TMDS Benefits in OrchestratorBackend

### 1. Algorithm Generalization

Once an algorithm is written for `meta_list`, it works with:
- `std::tuple<...>`
- `std::variant<...>`
- Custom container templates
- Future container types (no code changes)

### 2. Type Safety

Operations on type sequences happen at compile-time:
- Errors are caught immediately
- No runtime overhead
- Full type information available

### 3. Composability

TMDSs enable building complex transformations from simple ones:

```cpp
using step1 = meta_push_t<meta_list<int>, float>;           // step1 = meta_list<int, float>
using step2 = meta_omit_t<step1, float>;                     // step2 = meta_list<int>
using step3 = meta_merge_t<step2, meta_list<bool, char>>;   // step3 = meta_list<int, bool, char>
using result = meta_list_to_variant_t<step3>;               // result = std::variant<int, bool, char>
```

### 4. Code Reuse

Without TMDS, every algorithm must be written for every container type. With TMDS:
- Write algorithm once on `meta_list`
- Convert any container type to/from `meta_list`
- One algorithm → infinite container types

---

## Common TMDS Usage Patterns

### Pattern 1: Extract, Transform, Convert

```cpp
// Convert std::tuple to std::variant with filtered types
using source = std::tuple<int, float, bool, double>;
using filtered = meta_omit_t<source, float, double>;
using result = /* conversion to variant */;
```

### Pattern 2: Merge Multiple Containers

```cpp
// Merge types from multiple containers into one
using list1 = std::tuple<int, float>;
using list2 = std::variant<bool, char>;
using merged = meta_merge_t<list1, list2>;  // Works on both!
```

### Pattern 3: Type Sequence Algorithms

```cpp
// Apply algorithm to each element's type
using types = meta_list<int, float, bool>;
using with_pointers = meta_push_t<types, int*, float*, bool*>;
using with_refs = /* filter to get references */;
```

### Pattern 4: Conditional Type Construction

```cpp
// Build a type based on properties of other types
template<typename T>
using augmented_type = std::conditional_t<
  is_container_type_v<T>,
  meta_merge_t<T, meta_list<std::monostate>>,  // Add sentinel if container
  T                                             // Keep as-is otherwise
>;
```

---

## TMDS vs Runtime Collections

| Aspect | TMDS | Runtime Collection |
|--------|------|-------------------|
| **Existence** | Compile-time only | Runtime objects |
| **Memory** | No runtime overhead | Heap/stack allocation |
| **Operations** | Template specialization | Function calls |
| **Type Safety** | Compiler-checked | Runtime exceptions |
| **Immutability** | Immutable (types don't change) | Mutable |
| **Overhead** | Zero (erased at runtime) | O(n) space |

---

## Advanced: TMDS Composition

Complex metafunctions build on simpler ones using TMDS as the "working format":

```cpp
// meta_merge_binary: Merge two containers
template<typename T1, typename T2>
struct meta_merge_binary {
  private:
    using elem1 = /* extract elements from T1 */;
    using elem2 = /* extract elements from T2 */;
    
    // Work with meta_list internally
    using combined = meta_list<elem1..., elem2...>;
    
  public:
    using type = /* convert combined back to appropriate container */;
};

// meta_merge: Recursively merge multiple containers (builds on meta_merge_binary)
template<typename First, typename Second, typename... Rest>
struct meta_merge {
  using type = typename meta_merge<
    meta_merge_binary_t<First, Second>,  // ← Uses binary merge
    Rest...
  >::type;
};
```

---

## The TMDS Abstraction: Why It Matters

At its core, TMDS provides **abstraction** at the type system level:

1. **Separation of Concerns:**
   - Algorithm logic (filter, merge, transform)
   - Container conversion logic (extract elements, reconstruct type)

2. **Uniform Interface:**
   - All operations work on `meta_list`
   - Container-specific details are handled at boundaries

3. **Scalability:**
   - Adding new container types doesn't require rewriting algorithms
   - Just implement `meta_list ↔ NewContainer` conversion

4. **Composability:**
   - Complex transformations = pipeline of simple TMDS operations
   - Each step produces a valid `meta_list` (or target container)

---

## Current TMDS Implementations in OrchestratorBackend

### Core Data Structures

- **`meta_list`** — Ordered type sequence (in `TemplateMetaDataStructures/MetaList/`)
- **`meta_dict`** — Compile-time key-value map (in `TemplateMetaDataStructures/MetaDict/`)
  - **`meta_dict_entry`** — Key-value pair for `meta_dict` (in `MetaDict/MetaDictEntry/`)
- **`meta_mono`** — Single-type wrapper (in `TemplateMetaDataStructures/MetaMono/`)

### Supporting Infrastructure

**Type Traits (for validation):**
- `IsContainerType` — Validates that a type is a container
- `IsContainerTemplate` — Validates that a type is a template container
- `MatchesTemplate` — Checks if a type matches a template
- `IsTemplateTemplateEqual` — Compares two template templates for equality
- `MetaContains` — Checks if a type is contained in a type list
- `IsStringLike` — Validates that a type behaves like a string
- `IsDuration` — Validates that a type is a std::chrono duration
- `DependentFalse` — Always false for static_assert in templates

**Template Meta Functions (algorithms on TMDS):**
- `meta_transfer` — Convert elements from one container to another
- `meta_merge` — Combine multiple containers
- `meta_push` — Append types
- `meta_push_unique` — Append types only if not already present
- `meta_omit` — Filter out specific types
- `meta_distinct` — Remove duplicate types from a list
- `meta_reduce` — Fold/reduce operation on type lists
- `meta_variadic_map` — Map function over variadic type packs
- `meta_list_to_variant` — Convert meta_list to std::variant

**Convenience Aliases (domain-specific TMDS operations):**
- `meta_list_merge_t` — Merge meta_lists specifically
- `meta_list_omit_t` — Filter meta_lists specifically
- `meta_list_push_t` — Append to meta_lists specifically
- `meta_dict_get_value_t` — Get value by key from meta_dict
- `meta_dict_remove_key_t` — Remove entry by key from meta_dict

---

## Design Principles for TMDS

When creating or using TMDS in OrchestratorBackend:

1. **Keep TMDS Neutral** — Don't encode semantic meaning in TMDS; use target containers for that
2. **Use TMDS as Intermediate** — Transform at boundaries (enter/exit the algorithm)
3. **Work with meta_list** — Algorithms should operate on `meta_list`, not container types
4. **Reconstruct on Exit** — Convert `meta_list` back to appropriate container type
5. **Compose Operations** — Build complex transformations from simple TMDS operations

---

## TMDS Interoperability

The three TMDS structures work together to enable different type-level programming patterns:

### Pattern: Dictionary of Lists

```cpp
using list1 = meta_list<int, float>;
using list2 = meta_list<bool, char>;

using dict = meta_dict<
  meta_dict_entry<std::string, list1>,
  meta_dict_entry<std::vector<int>, list2>
>;

using retrieved_list = dict::getValue<std::string>;  // meta_list<int, float>
```

### Pattern: Monadic Wrapping in Lists

```cpp
using wrapped_types = meta_list<
  meta_mono<int>,
  meta_mono<float>,
  meta_mono<bool>
>;

// Can be unwrapped via TMF to get: meta_list<int, float, bool>
```

### Pattern: Dictionary with Mono Values

```cpp
using config_dict = meta_dict<
  meta_dict_entry<struct PortKey, meta_mono<std::integral_constant<int, 8080>>>,
  meta_dict_entry<struct HostKey, meta_mono<const char*>>
>;

using port_type = config_dict::getValue<PortKey>::type;  // std::integral_constant<int, 8080>
```

---

## Conclusion

Template Meta Data Structures are the **foundational abstraction** that enables the OrchestratorBackend's template meta programming system to:

- Work with heterogeneous type sequences (`meta_list`)
- Perform compile-time key-value lookups (`meta_dict`)
- Wrap single types uniformly (`meta_mono`)
- Compose algorithms generically across all structures
- Maintain type safety at compile-time
- Avoid code duplication across container types
- Build elegant, composable metaprograms

By providing three specialized structures (`meta_list`, `meta_dict`, `meta_mono`), the TMDS layer offers the right abstraction for different compile-time data organization needs, while `meta_list` continues to serve as the primary intermediate representation for type sequence transformations.

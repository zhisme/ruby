# Static Typing Research Implementation

This document tracks the implementation of parse-time static typing for Ruby primitives as a research project.

## Goal

Add static type checking for primitive types (Integer, Float, String, Symbol, TrueClass, FalseClass, NilClass)
at parse-time to evaluate:
- Performance overhead (parse-time only, NO runtime cost)
- Code readability impact
- Maintenance burden
- Overall fit with Ruby's design philosophy

## Syntax

### Parameter Type Annotations
```ruby
def method_name(param1: Integer, param2: String)
  # method body
end
```

### Return Type Annotations
```ruby
def method_name(param: Integer) -> String
  # method body
end
```

### Combined
```ruby
def calculate(x: Integer, y: Integer) -> Integer
  x + y
end
```

## Implementation Status

### ✅ Phase 1: Type System Foundation (COMPLETED)

**Files Created:**
- `type_system.h` - Type definitions and function prototypes
- `type_system.c` - Type system implementation

**Features:**
- Primitive type enumeration (Integer, Float, String, Symbol, TrueClass, FalseClass, NilClass)
- Type info structures for parameters and return types
- Method signature tracking
- Type compatibility checking
- Error reporting functions

### ✅ Phase 2: AST Node Extensions (COMPLETED)

**Files Modified:**
- `rubyparser.h` - Extended node structures

**Changes:**
- Added `nd_type` field to `rb_node_args_aux_t` for parameter types
- Added `nd_return_type` field to `rb_node_defn_t` for method return types
- Added `nd_return_type` field to `rb_node_defs_t` for singleton method return types

### ✅ Phase 3: Parser Token Definitions (COMPLETED)

**Files Modified:**
- `parse.y` - Added type annotation tokens

**New Tokens:**
- `tTYPE_INTEGER` - Integer type annotation
- `tTYPE_FLOAT` - Float type annotation
- `tTYPE_STRING` - String type annotation
- `tTYPE_SYMBOL` - Symbol type annotation
- `tTYPE_TRUE` - TrueClass type annotation
- `tTYPE_FALSE` - FalseClass type annotation
- `tTYPE_NIL` - NilClass type annotation

**Grammar Rules Added:**
- `type_name` - Inline rule to parse type names and return type enum values
- Modified `f_arg_item` - Now supports `param: Type` syntax
- Initialized return types in `rb_node_defn_new` and `rb_node_defs_new`

### ✅ Phase 4: Build System Integration (COMPLETED)

**Files Modified:**
- `common.mk` - Added `type_system.$(OBJEXT)` to COMMONOBJS

## Remaining Work

### 🔄 Phase 5: Lexer Support (PENDING)

**Tasks:**
- Modify lexer to recognize type names (Integer, Float, etc.) in type annotation context
- Distinguish between type annotations and regular constant references
- Handle `:` and `->` tokens in parameter and return type positions

### 🔄 Phase 6: Return Type Grammar (PENDING)

**Tasks:**
- Add grammar rules for `-> ReturnType` in method definitions
- Support both `def ... end` and endless method syntax
- Store return type in method definition nodes

### 🔄 Phase 7: Type Checking Implementation (PENDING)

**Files to Modify:**
- `compile.c` - Add type checking during compilation

**Tasks:**
- Implement type inference for literal expressions
- Check parameter type compatibility
- Check return type compatibility
- Generate SyntaxError on type mismatches

### 🔄 Phase 8: Testing (PENDING)

**Tests to Create:**
- `test/ruby/test_static_types_basic.rb` - Basic type annotations
- `test/ruby/test_static_types_errors.rb` - Type mismatch errors
- `test/ruby/test_static_types_primitives.rb` - All primitive types
- `test/ruby/test_static_types_edge.rb` - Edge cases
- `benchmark/static_types_overhead.rb` - Performance benchmarks

### 🔄 Phase 9: Documentation (PENDING)

**Documents to Create:**
- `doc/syntax/types.md` - Syntax guide and examples
- Research findings document with performance analysis

## Design Decisions

### Parse-Time Only
- Type checking happens during parsing/compilation
- NO runtime overhead whatsoever
- Errors are SyntaxError, not runtime exceptions

### Simple Type Inference
- Infer types from literals only (123 → Integer, "hello" → String)
- No cross-method type inference
- Unknown types are always compatible (gradual typing)

### Primitive Types Only
- No Array/Hash typing (by design for research scope)
- No generics or complex types
- No duck typing with annotations

### Type Compatibility
- Exact match required for primitives
- Unknown types (untyped code) always compatible
- No implicit conversions

## Build Instructions

```bash
# Configure
autoconf
./configure --disable-install-doc

# Build
make

# Test
./ruby -e "def foo(x: Integer) -> Integer; x; end; foo(42)"
```

## Example Code

### Valid (Should Work)
```ruby
def add(x: Integer, y: Integer) -> Integer
  x + y
end

def greet(name: String) -> String
  "Hello, #{name}"
end
```

### Invalid (Should Raise SyntaxError)
```ruby
def bad_return(x: Integer) -> String
  123  # SyntaxError: expected String, got Integer
end

def bad_param(x: Integer)
  x  # Untyped is OK
end
```

## Research Metrics to Collect

1. **Parse Time Overhead**: Compare parse time with/without type annotations
2. **Code Verbosity**: Line count increase with type annotations
3. **Error Detection**: Types of errors caught at parse-time
4. **Maintenance Impact**: Complexity of implementation
5. **Readability**: Subjective assessment of annotated vs unannotated code

## Notes

- This is a research implementation to validate arguments against static typing in Ruby
- Not intended for production use
- Demonstrates the complexity and overhead of adding static types to a dynamic language
- Validates concerns about maintenance burden and philosophical mismatch

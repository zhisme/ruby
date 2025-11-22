# Testing and Building Ruby with Static Typing

This document describes how to test and build the Ruby interpreter with static typing extensions.

## Building the Interpreter

### Prerequisites
- Standard Ruby build tools (gcc, make, autoconf)
- Optional: gperf (for keyword modifications)

### Build Steps

```bash
# Clean previous builds (if any)
make clean

# Generate configure script (if needed)
autoconf

# Configure the build
./configure --disable-install-doc

# Build the interpreter
make

# Build just miniruby (faster for testing)
make miniruby
```

### Quick Build for Testing
```bash
# After modifying parser or type system
make parse.c  # Regenerate parser
make miniruby # Rebuild interpreter
```

## Testing Static Type Features

### Syntax

#### Parameter Type Annotations
```ruby
def method_name(param1: Integer, param2: String)
  # method body
end
```

#### Return Type Annotations

**Regular Methods:**
```ruby
def foo() => Integer
  42
end
```

**Endless Methods:**
```ruby
def foo() => Integer = 42
```

**Methods with Parameters:**
```ruby
def add(x: Integer, y: Integer) => Integer
  x + y
end
```

### Running Tests

#### Basic Functionality Test
```bash
./miniruby -e "def add(x: Integer, y: Integer) => Integer; x + y; end; puts add(2, 3)"
```

#### Type Checking Tests

**Valid Type (Should Work):**
```bash
./miniruby -e "def foo() => Integer = 42; end; puts foo()"
```

**Invalid Type (Should Raise SyntaxError):**
```bash
./miniruby -e "def bad() => String = 123; end; puts bad()"
# Expected: SyntaxError: expected String, got Integer
```

**Impossible Operations (Should Raise SyntaxError):**
```bash
# Float divided by Symbol - type mismatch
./miniruby -e "def bad(x: Float, y: Symbol) => Float; x / y; end"
# Expected: SyntaxError at parse time
```

### Test Cases

#### 1. Basic Type Annotations
```bash
# Integer return type
./miniruby -e "def get_num() => Integer = 42; end; puts get_num()"

# String return type
./miniruby -e "def get_str() => String = 'hello'; end; puts get_str()"

# Float return type
./miniruby -e "def get_float() => Float = 3.14; end; puts get_float()"
```

#### 2. Parameter Types
```bash
# Single parameter
./miniruby -e "def double(x: Integer); x * 2; end; puts double(21)"

# Multiple parameters
./miniruby -e "def greet(name: String, age: Integer); puts name; end; greet('Alice', 30)"
```

#### 3. Combined Parameter and Return Types
```bash
./miniruby -e "def calc(a: Integer, b: Integer) => Integer; a + b; end; puts calc(10, 20)"
```

#### 4. Type Mismatch Detection
```bash
# Return type mismatch
./miniruby -e "def bad_return() => String; 123; end"
# Expected: SyntaxError: expected String, got Integer

# Parameter type issues (detected at runtime, not parse-time)
./miniruby -e "def foo(x: Integer); x; end; foo('string')"
```

#### 5. All Primitive Types
```bash
# Integer
./miniruby -e "def test() => Integer = 42; end; puts test()"

# Float
./miniruby -e "def test() => Float = 3.14; end; puts test()"

# String
./miniruby -e "def test() => String = 'hello'; end; puts test()"

# Symbol
./miniruby -e "def test() => Symbol = :sym; end; puts test()"

# TrueClass
./miniruby -e "def test() => TrueClass = true; end; puts test()"

# FalseClass
./miniruby -e "def test() => FalseClass = false; end; puts test()"

# NilClass
./miniruby -e "def test() => NilClass = nil; end; puts test()"
```

## Verification Examples

### Invalid Operations

These examples demonstrate operations that should be caught at parse-time:

```ruby
# Float cannot be divided by Symbol
def impossible(x: Float, y: Symbol) => Float
  x / y  # Type error: Cannot divide Float by Symbol
end

# String cannot be multiplied by Symbol
def also_bad(s: String, sym: Symbol) => String
  s * sym  # Type error: Cannot multiply String by Symbol
end
```

Note: Current implementation focuses on return type checking. Full operation type checking across all operations is not yet implemented.

## Debugging Build Issues

### Parser Generation Fails
```bash
# Check for syntax errors in parse.y
make parse.c

# If reduce/reduce conflicts occur, check:
# tool/lrama/lib/lrama/grammar_validator.rb
# Ensures 1 reduce/reduce conflict is allowed for type annotations
```

### Compilation Errors
```bash
# Check type_system.c and type_system.h are in COMMONOBJS
grep type_system common.mk

# Verify includes
grep "type_system.h" compile.c iseq.h
```

### Runtime Test Failures
```bash
# Run with verbose error output
./miniruby -W2 -e "your test code here"

# Check if type checking is active
./miniruby -e "def foo() => Integer = 'wrong'; end"
# Should show SyntaxError, not execute
```

## Performance Testing

### Parse-Time Overhead

The type system adds minimal parse-time overhead. To measure:

```bash
# Time parsing with type annotations
time ./miniruby -e "1000.times { eval 'def f(x: Integer) => Integer; x; end' }"

# Time parsing without type annotations
time ./miniruby -e "1000.times { eval 'def f(x); x; end' }"
```

### No Runtime Overhead

Type checking occurs only at parse-time. There is ZERO runtime overhead:

```bash
# These should have identical runtime performance
time ./miniruby -e "def f(x); x; end; 1000000.times { f(42) }"
time ./miniruby -e "def f(x: Integer) => Integer; x; end; 1000000.times { f(42) }"
```

## Test Suite Integration

### Running Existing Tests
```bash
# Run full test suite
make test

# Run specific test files
make test-all TESTS="../test/ruby/test_syntax.rb"
```

### Adding New Tests

Create test files in `test/ruby/`:

```ruby
# test/ruby/test_static_types.rb
require 'test/unit'

class TestStaticTypes < Test::Unit::TestCase
  def test_integer_return_type
    assert_nothing_raised do
      eval 'def foo() => Integer = 42; end'
    end
  end

  def test_type_mismatch_error
    assert_raise(SyntaxError) do
      eval 'def bad() => String = 123; end'
    end
  end
end
```

## Continuous Integration

For CI environments, ensure:

1. **Parser regeneration works**: `make parse.c` should succeed
2. **Type system compiles**: No errors in `type_system.c`
3. **Tests pass**: Both new type tests and existing Ruby tests

```bash
#!/bin/bash
# CI build script
set -e

autoconf
./configure --disable-install-doc
make parse.c
make miniruby
make test-all
```

## Troubleshooting

### "dynamic constant assignment" Error

If you see this error with type annotations:
```
syntax error: dynamic constant assignment
```

This means the parser is interpreting the type annotation as a constant reference instead of a type annotation. This is a known grammar ambiguity issue with operator-based syntax.

### Type Checking Not Working

If type mismatches aren't caught:

1. Check compile.c has type checking code
2. Verify NODE structures have nd_return_type field
3. Ensure type_system.c is linked

```bash
# Verify type checking is compiled in
nm miniruby | grep check_primitive_type_name
nm miniruby | grep rb_infer_node_type
```

## Known Limitations

1. **Grammar Conflicts**: Operator-based syntax (`=>`, `::`) has ambiguities
2. **Parameter Type Checking**: Only return types are fully checked at parse-time
3. **Complex Expressions**: Type inference only works for literals
4. **No Generic Types**: Arrays, Hashes not supported
5. **Keyword Addition**: Requires gperf to add new keywords properly

## Future Improvements

- Full operator type checking (division, multiplication, etc.)
- Better syntax without grammar conflicts (requires keyword support)
- Cross-method type inference
- Generic type support for collections


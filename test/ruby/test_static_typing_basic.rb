# frozen_string_literal: true
require 'test/unit'

class TestStaticTypingBasic < Test::Unit::TestCase
  def test_integer_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => Integer = 42; end'
    end
  end

  def test_string_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => String = "hello"; end'
    end
  end

  def test_float_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => Float = 3.14; end'
    end
  end

  def test_symbol_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => Symbol = :test; end'
    end
  end

  def test_true_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => TrueClass = true; end'
    end
  end

  def test_false_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => FalseClass = false; end'
    end
  end

  def test_nil_return_type_literal
    assert_nothing_raised do
      eval 'def foo() => NilClass = nil; end'
    end
  end

  def test_type_mismatch_integer_string
    assert_raise(SyntaxError) do
      eval 'def bad() => Integer = "string"; end'
    end
  end

  def test_type_mismatch_string_integer
    assert_raise(SyntaxError) do
      eval 'def bad() => String = 123; end'
    end
  end

  def test_type_mismatch_float_symbol
    assert_raise(SyntaxError) do
      eval 'def bad() => Float = :symbol; end'
    end
  end

  def test_parameter_type_annotation
    assert_nothing_raised do
      eval 'def foo(x: Integer); x; end'
    end
  end

  def test_multiple_parameter_types
    assert_nothing_raised do
      eval 'def foo(x: Integer, y: String, z: Float); x; end'
    end
  end

  def test_combined_param_and_return_types
    assert_nothing_raised do
      eval 'def add(x: Integer, y: Integer) => Integer; x + y; end'
    end
  end

  def test_singleton_method_with_return_type
    assert_nothing_raised do
      eval 'def self.foo() => Integer = 42; end'
    end
  end

  def test_method_without_type_annotations
    assert_nothing_raised do
      eval 'def untyped_method(x, y); x + y; end'
    end
  end

  def test_invalid_type_name
    assert_raise(SyntaxError) do
      eval 'def bad() => InvalidType = 42; end'
    end
  end
end

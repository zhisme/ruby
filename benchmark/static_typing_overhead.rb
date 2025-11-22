# Benchmark: Parse-time overhead of static type annotations

require 'benchmark'

ITERATIONS = 10_000

puts "="*60
puts "Static Typing Parse-Time Overhead Benchmark"
puts "="*60
puts

# Benchmark 1: Method definition without type annotations
puts "Benchmark 1: Parsing methods WITHOUT type annotations"
time_untyped = Benchmark.measure do
  ITERATIONS.times do |i|
    eval "def untyped_method_#{i}(x, y); x + y; end"
  end
end
puts time_untyped

# Benchmark 2: Method definition with parameter type annotations only
puts "\nBenchmark 2: Parsing methods WITH parameter type annotations"
time_param_typed = Benchmark.measure do
  ITERATIONS.times do |i|
    eval "def param_typed_method_#{i}(x: Integer, y: Integer); x + y; end"
  end
end
puts time_param_typed

# Benchmark 3: Method definition with return type annotations
puts "\nBenchmark 3: Parsing methods WITH return type annotations"
time_return_typed = Benchmark.measure do
  ITERATIONS.times do |i|
    eval "def return_typed_method_#{i}() => Integer = 42; end"
  end
end
puts time_return_typed

# Benchmark 4: Method definition with both parameter and return types
puts "\nBenchmark 4: Parsing methods WITH both parameter and return types"
time_fully_typed = Benchmark.measure do
  ITERATIONS.times do |i|
    eval "def fully_typed_method_#{i}(x: Integer, y: Integer) => Integer; x + y; end"
  end
end
puts time_fully_typed

# Summary
puts "\n" + "="*60
puts "SUMMARY (#{ITERATIONS} iterations each)"
puts "="*60
puts "Untyped methods:                #{time_untyped.real.round(4)}s"
puts "Parameter-typed methods:        #{time_param_typed.real.round(4)}s"
puts "Return-typed methods:           #{time_return_typed.real.round(4)}s"
puts "Fully-typed methods:            #{time_fully_typed.real.round(4)}s"
puts
puts "Overhead:"
puts "  Parameter types:              +#{((time_param_typed.real / time_untyped.real - 1) * 100).round(2)}%"
puts "  Return types:                 +#{((time_return_typed.real / time_untyped.real - 1) * 100).round(2)}%"
puts "  Full typing:                  +#{((time_fully_typed.real / time_untyped.real - 1) * 100).round(2)}%"
puts "="*60

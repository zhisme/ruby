# Fibonacci with Static Type Annotations
# Demonstrates parameter and return type annotations

# Recursive fibonacci with type annotations
def fib_typed(n: Integer) => Integer
  if n <= 1
    n
  else
    fib_typed(n - 1) + fib_typed(n - 2)
  end
end

# Untyped version for comparison
def fib_untyped(n)
  if n <= 1
    n
  else
    fib_untyped(n - 1) + fib_untyped(n - 2)
  end
end

# Test both versions
puts "Fibonacci(10) with types: #{fib_typed(10)}"
puts "Fibonacci(10) without types: #{fib_untyped(10)}"

# Benchmark
require 'benchmark'

n = 20
iterations = 10

puts "\nBenchmark (n=#{n}, #{iterations} iterations):"
Benchmark.bm(15) do |x|
  x.report("typed:") do
    iterations.times { fib_typed(n) }
  end

  x.report("untyped:") do
    iterations.times { fib_untyped(n) }
  end
end

puts "\nNote: Runtime performance should be identical."
puts "Type checking occurs only at parse-time (zero runtime cost)."

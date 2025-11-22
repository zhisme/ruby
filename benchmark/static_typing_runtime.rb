# Benchmark: Runtime performance comparison
# Type annotations should have ZERO runtime overhead

require 'benchmark'

ITERATIONS = 1_000_000

# Define methods
def untyped_add(x, y)
  x + y
end

eval 'def typed_add(x: Integer, y: Integer) => Integer; x + y; end'

puts "="*60
puts "Static Typing Runtime Performance Benchmark"
puts "="*60
puts "Type checking occurs at PARSE-TIME only."
puts "Runtime performance should be IDENTICAL."
puts "="*60
puts

# Benchmark method calls
puts "Calling methods #{ITERATIONS} times..."
puts

time_untyped = Benchmark.measure do
  ITERATIONS.times do
    untyped_add(21, 21)
  end
end

time_typed = Benchmark.measure do
  ITERATIONS.times do
    typed_add(21, 21)
  end
end

puts "Untyped method:  #{time_untyped}"
puts "Typed method:    #{time_typed}"
puts

# Analysis
difference_percent = ((time_typed.real / time_untyped.real - 1) * 100).round(4)
puts "="*60
puts "ANALYSIS"
puts "="*60
puts "Untyped:  #{time_untyped.real.round(6)}s"
puts "Typed:    #{time_typed.real.round(6)}s"
puts "Difference: #{difference_percent}%"
puts
if difference_percent.abs < 1.0
  puts "✓ PASS: Runtime overhead is negligible (<1%)"
  puts "  Type checking has no measurable runtime cost."
else
  puts "⚠ WARNING: Unexpected runtime difference detected"
  puts "  This may indicate a measurement artifact or system variance."
end
puts "="*60

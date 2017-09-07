var fib_sum
var fib1 = 1, fib2 = 1

var n = 10

var i = 2
while i < n:
	fib_sum = fib1 + fib2
	fib1 = fib2
	fib2 = fib_sum
	i = i+1

sub factorial(n):
	if n < 2:
		return 1
	return n*factorial(n-1)

i=0
while i < 10:
	if i % 2:
		c = factorial(i)
	else:
		c = i**2
	i -= 1

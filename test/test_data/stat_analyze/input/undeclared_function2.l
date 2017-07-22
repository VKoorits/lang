sub factorial(n):
	var n
	if n < 2:
		return 1
	print("ok")
	return n*factorial(n-1)

var lst = {"5! = " => factorial(5)}

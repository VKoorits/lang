nums = [ 1->60 ] // nums = [1,2,3,...,59,60]
for i in nums:
	for j in nums:
		if random.float() > 0.5 :
			swap i, j
/*
	bubble sort, 
	but in random order
*/
print nums

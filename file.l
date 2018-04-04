var c = 5
var d = 5
var res = 8

while c < 100000:
    while d < 100000:
        var a = c
        var b = d
        
        
        while a*b>0:
            if a > b:
                a = a % b
            else:
                b = b % a
        
        res = res*(a+b+1)
        res = res % 1000
        d = d+1
    c = c+1

println(res)
        

package main

func main () {

	a:=3
	b:=3
	c:=0.0
c = int2float(a)/2.0e0   // c = 2.0
print(c, "\n")
if (c > 1.9) {
	b:=2.0
}
a = lol(c)
print("\t jj", a, "\n")
}

func lol (n float64) (int) {
n = n * n
a:= float2int(n)
return a - 1
}

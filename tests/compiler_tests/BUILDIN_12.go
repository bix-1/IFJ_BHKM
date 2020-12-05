package main

func main () {

	a:=5
	b:= int2float(a) / int2float(5)

	if b == 1.0 {
	print("ok", b)
	}
	
	c:=4.0
	d:= int2float(8) / c
	if (d == 2.0) {
	print("ok", d)
	}
}



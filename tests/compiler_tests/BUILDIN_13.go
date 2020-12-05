package main

func main () {

	a:=0.3
	b:= a / int2float(100)
	if (b == 0.0) {
	print("ok")
	}

	c:= int2float(42) / 1.0
	if (c == 42.0) {
	print("ok")
	}


}



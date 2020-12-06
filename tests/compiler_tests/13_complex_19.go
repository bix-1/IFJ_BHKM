package main
func foo (a int, b float64) (float64) {
if (a != 5) {
	if b == 4.3 {
		print("OK")
		return 4.42
	}
} else if (a == 10) {
	if (b >= 2.0e-2) {
		
		print("OK")
		return 20.0e-1
	}
} else {
	if b > 0.0 {
	
		print("OK")
		return 1e2
	}
	print("FAIL")
}
}

func main() {
	a:=0.0
	b:=0
	c:=20
	
	a = foo(c, 4.30)
	if a == 4.42 {
		print("OK", a)
	}
	
	a = foo(10, 2.0e-2) 
	if a == 20.0e-1 {
		print("OK", a)
	}
	
	a = foo (5, 4.3 / 2.1)
	if (a == 1e2) {
		print("OK", a)
	}
}

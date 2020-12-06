package main

func foo (a int) (bool) {

	c:= a == 10 // true

return c // true
}

func main () {

	b := foo(10) // b = true
	d := true

	if  b && d {
	print("OK")
	} else if (5 < 4 && true) {
	print ("FAIL")
	}

}

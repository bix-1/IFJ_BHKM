package main

func foo (a int, b int) (s int, k int) {
s = a + 1
k = b / 10
return
}
func main () {
	x:=3
	y:=10
	a, b := foo(x, y)

	if a == 4 {
		if b == 1 {
		print("OK")
		}
	
	} else {
	print("fail")
	}


}


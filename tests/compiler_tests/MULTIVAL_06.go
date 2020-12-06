package main

func foo (n float64) (s float64, f float64) {

s = n
f = n
return
}

func main () {

	a:= 1.1
	a, b := foo(a)

}

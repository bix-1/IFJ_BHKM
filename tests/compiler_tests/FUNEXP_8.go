package main 

func foo (s int) (int) {

return s
}

func main () { 

	a:=0
	b:=5
	c:=0
	c = ((2 + foo(1)) * 2) - 1 / foo(2)

	print(c) //6

}

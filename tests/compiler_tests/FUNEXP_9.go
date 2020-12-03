package main 

func foo (s int) (int) {

return s
}

func main () { 

	a:=2
	b:= (foo(a*a)-1) / (foo(1*1) / 1)
	print(b) //3


}


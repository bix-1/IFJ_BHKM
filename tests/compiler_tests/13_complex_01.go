package main


func foo (lo int) (float64){
return 10.2e-1
}

func foo1 (lso float64) (string) {
return "xd"
}

func main() {

	a:= 42.2
	b:= 1.1
	c:= 1.02
	d:= ""
	d = foo1(b)
	a = foo(2)

	if a == c {
	b = 4.1
	}

	for ;b == 4.1; b = b + 1.05 {
	d = "xd"
	}

	d = d + d

	if 1==1 {

	}
}

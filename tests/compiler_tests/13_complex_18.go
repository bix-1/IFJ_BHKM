package main

func foo (a string, b string) (s string) {
s = a + b
return
}

func main() {
	a := "a"
	a = foo(a, a)
	print("OK", a) // a = aa
	
	for b:=0; b < 4; b = b + 1 {
	a = foo(a, a)
	print("OK", a)
	}
	//OK aaaa
	//OK aaaaaaaa
	//OK aaaaaaaaaaaaaaaa
	//OK aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	
	if (a <= "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa") {
	
	} else if (a <= "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") {
	
	} else { // true
	print("OK IF", a)
	}

}

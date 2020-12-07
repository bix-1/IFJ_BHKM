package main

func foo (s int, b bool) (a bool, p int) {
if (s == 1) {
p = s + 2
a = b
} else if (s == 5) {

p = s * 2
a = true
} else {

p = s/s
a = false
}
return
}

func foo1 (s float64) (float64) {
s = s * 2.0
a:= 1.05
a = s - 1.5
return a
}

func foo2 (s string) (string) {

	return s + s
}
func main () {
	a:=0
	b:=true
	c:=0
	if b != false {
		a:=1e2
		b:=foo1(a + 1.0)
		if b == 200.5 {
		print(b, "OK\n")
		} else {
		print(b, "FAIL\n")
		}
	}

	print("Zadaj string: Ahoj")
	x := "xd"
	x, _ = inputs()

	if x == "Ahoj" {
		print(a, "OK\n")
	} else {
		print(a, "pepega\n")
	}

	for d:=len(foo2("a")); d < 10; d = d + float2int(1.999){
		if d == 9 {
		print(d, "9 - Koniec cyklu = OK\n")
		}
	}

	g,a := foo(1 + 0, 1>0)

	if (a == 3) {
		if g == true {
		print("OK\n")
		} else {
		print("fail\n")
		}
	} else {
	print("fail\n")
	}

	g,a = foo(10/2, false)

	if a == 10 {
		if g == true {
		print("OK\n")
		} else {
		print("fail\n")
		}
	} else {
	print("fail\n")
}

}

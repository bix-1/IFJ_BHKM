package main

func main() {
	print("### Variable declarations ###\n")
	a := 12
	b := 24.567
	c := "Hello"
	d := true

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================

	print("### Variable definitions ###\n")
	a = 24
	b = 25.835793
	c = "World"
	d = false

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================

	print("### Variable adv. definitions ###\n")
	a = 24 + 21 + 899 - 453
	b = 25.835793 + 6.0000001 - 1.42942849942849
	c = "World" + " Hello"

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================

	print("### Variable adv.2 definitions ###\n")
	a = 24 + 21 * 899 - 452 / 2 * 1 - 20 + 50 * 5 + 222 - 777 + 300 / 6
	b = 25.835793 + 6.0000001 - 1.42942849942849 + 7.4544545 * 2.000001 / 1.424424 - 7.6464
	c = "World" + " Hello" + " and Another world" + " and another one."

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================

	print("### Variable adv.2 definitions ###\n")
	a = 24 + 21 * 899 - 452 / 2 * 1 - 20 + 50 * 5 + 222 - 777 + 300 / 6
	b = 25.835793 + 6.0000001 - 1.42942849942849 + 7.4544545 * 2.000001 / 1.424424 - 7.6464
	c = "World" + " Hello" + " and Another world" + " and another one."

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================

	print("### Variables with functions 1 ###\n")
	a1()
	b1()
	c1()
	d = d1()

	//==========================================

	print("### Variables with functions 1 ###\n")
	a = a2(a)
	b = b2(b)
	c = c2(c)
	d = d1()

	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(d, "\n")

	//==========================================
	// .... TODO : add more
	//==========================================
	print("### Basic build in functions ###\n")

	s := ""
	o := 0
    i := 0
    f := 0.0

    s,i = inputs()
    o,i = inputi()
    f,i = inputf()

	f = int2float(o)
    o = float2int(f)
    o = len(s)
    o = len("fff")
    s,i = substr("fff", o, 0)
    i,i = ord(s, 5)
    s,i = chr(5)

	print(s, "\n")
	print(o, "\n")
	print(i, "\n")
	print(f, "\n")
}

func a1 () {
	a := 554
	b := 3590
	c := 12

	a = b * c - (b - b) * 2 - 220 + c + a - a + a * a + a - b - 19849

	print(a, "\n")
	print(2 * a, "\n")
}

func a2 (a int) (int) {
	b := 899
	c := 2

	a = b * c - (b - b) * 2 - 220 + c + a - a + a * a + a - b - 19849

	return a
}

func b1 () {
	a := 8785.434981498
	b := 0.41894914813
	c := 94184.4148914

	a = b * c - (b - b) * 2.41 - 29.41 + c + a - a + a * a + a - b - 1984.49814

	print(a, "\n")
	print(2.0 / a, "\n")
}

func b2 (b float64) (float64) {
	a := 94309034.44101498
	b = 23.45194914813
	c := 6784.15320914

	return b * c - (b - b) * 2.41 - 29.41 + c + a - a + a * a + a - b - 1984.49814
}

func c1 () {
	a := "Hello"
	b := "42424"
	c := "30k3f"

	a = a + b + c + "faf" + b

	print(a, "\n")
	print(a + "#..#", "\n")
}

func c2 (c string) (string) {
	a := "fasfasf"
	b := "afmlva"

	a = a + b + c + "faf" + b

	return a
}

func d1 () (bool) {
	return true && true || false || true && !!!!!!!!!!false && !!!!!!!!true
}

package main

func foo(a int) {
	print(a, "\n")
}

func bar(x int) int {
    y := 0
	y = x + 24
	return y
}

func main() {
	a := 42
	foo(a)

	b := bar(a)
	print(b, "\n")

	c := "Hello\n"
	print(c, "\n")

	return
}

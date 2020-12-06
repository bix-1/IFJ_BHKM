package main

func foo(s bool) (bool) {

return false
}

func main () {

	a:=true
	c:=false

	if a == true {
		a:= 5
		a = a+a
	} else if (c != true) {
	c = foo(false)
	} else {
		print("nenastane")
	}

}

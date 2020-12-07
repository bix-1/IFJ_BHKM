package main

func main () {

	a:=len("x\\\n")
	if a == 3 {
		b:=len("\xff")
		if b != 1{
		print("Nenastane\n")
		} else {
			print("ok\n")
		}
	}

	if (len("\xff") == len("1")) {
	print("ok\n")
	}

}



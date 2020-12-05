package main

func main () {

	a:=len("x\\\n")
	if a == 3 {
		b:=len("\xff")
		if b != 1{
		print("Nenastane")
		} else {
			print("ok")
		}
	}
	
	if (len("\xff") == len("")) {
	print("ok")
	}

}



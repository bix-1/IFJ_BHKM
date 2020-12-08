package main

func main () {
	print("Zadaj string dlzky mensej ako 5\n")
	a, _:=inputs()
	b:=0
	b = len(a)
	print("String ma dlzku:", b, "\n")

	for ;b < 20; b = b + 1 {
		a = a + "1"
		if b == 5 {
			print("Retazec je dlzky\t", b, "\n")
		} else if b == 10 {
			print("Retazec je dlzky\t", b, "\n")
		} else if b == 15 {
			print("Retazec je dlzky\t", b, "\n")
		}
	}
	b = len(a)
	print("Vysledny retazec je dlzky\t", b, "\n")
	print("Odcitame z retazca posledne dve acka")
	h,_ := substr(a, 18, 2)
	b = len(h)
	print("Dlzka retazca je: ", b,"\n")
	print("Vysledny retazec je: ", h, "\n")
}

package main

func main () {
	print("Zadaj retazec o dlzky 5\n")
	a,_:=inputs()
	if len(a) == 5 {
		print("Zadaj cislo o ktore ma byt skrateny rerazec\n")
		b, _:=inputi()
		x:=0
		c:= "lollo"
		c, x = substr(a, 5 - b ,5 - (5 - b))
		if x == 1 {
			print("error\n")
		}
		xz:=len(c)
		print("Vysledny retazec je: ",c, " a jeho dlzka je ", xz,"\n")
	} else {
		print("Zadal si nespravnu dlzku rerazca")
	}

}

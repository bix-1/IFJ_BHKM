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
	print("Zadaj string\n")
    s,i = inputs()
            print("Zadaj int\n")
    o,i = inputi()
            print("Zadaj float\n")
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

	/*
		INPUT FUNCTIONS
	*/
	print("Zadaj cislo znaku z ASCII od ktoreho chces vypisat znaky: \n")
	vstup_x, x_ := inputi()
	print("Zadaj cislo znaku po ktore ma byt ascii tabulka vypisana: \n")
	vstup_y, y_ := inputi()

	if x_ == 1 || y_ == 1  {
		print("Nespravne, alebo ziadne cislo\n")
	} else if vstup_x < vstup_y {
		print("Prve cislo je mensie ako druhe\n")
	} else if vstup_x > 255 {
		print("Priliz velke x\n")
	} else if vstup_y > 255{
		print("Priliy velke y\n")
	}

	for ;vstup_x <= vstup_y; vstup_x = vstup_x + 1 {
		xd, _ := chr(vstup_x)
		print("Znak: ", xd, "\n")
	}

	print("Zadaj retazec a ja vypisem ordinalne hodnoty z neho\n")
	xdxd,_ :=inputs()
	lenxdxd:= len(xdxd)

	for xdxdx := 0;xdxdx < lenxdxd;xdxdx = xdxdx + 1 {
		xdxdxd, _:= ord(xdxd, xdxdx)
		print(xdxdxd, "\n")
	}

	print("Zadaj retazec o dlzky 5\n")
	xdxdxdx,_:=inputs()
	if len(xdxdxdx) == 5 {
		print("Zadaj cislo o na byt skrateny rerazec\n")
		b, _:=inputi()
		x:=0
		c:= "lollo"
		c, x = substr(xdxdxdx, 5 - b ,5 - (5 - b))
		if x == 1 {
			print("error\n")
		}
		xz:=len(c)
		print("Vysledny retazec je: ",c, " a jeho dlzka je ", xz,"\n")
	} else {
		print("Zadal si nespravnu dlzku rerazca\n")
	}

	print("Volanie ord mimo retazca\n")
	xdxdxdxd := "45"
	xdxdxdxdx, xdxdxdxdxd := ord(xdxdxdxd, 1-2)
	if (xdxdxdxdxd == 1){
		print("OK\n")
	}
	
	xdxdxdxdx, xdxdxdxdxd = ord(xdxdxdxd, 2)
        if (xdxdxdxdxd == 1){
                print("OK\n")
        }

	xdxdxdxdx, xdxdxdxdxd = ord(xdxdxdxd, 3)
        if (xdxdxdxdxd == 1){
                print("OK\n")
        }

	print("SUBSTR TEST OK\n")
	xdxdxdx = "\x44\\\n\t\xff\xff\""
	bxx := len(xdxdxdx)
	xzy := bxx - 2
	xxx,_ := substr(xdxdxdx, xzy, 2)
	print(xxx, "\n")
	
	if 9 < 8{
	print("NOTOK\n")
	} else if false || true{
		print("OK\n")

		for a:= 0.05e1;a <= 0.5e2; a = a + 0.090e1 {
			print("CYK\t")

			if a >= 0.4e+2 {
			print("OK\n")
			}
		}

		if false {
	
		} else {
			for ax := 5 ;ax >= 0; ax = ax - 1{
				a:="WHIII\x10"
				print(a)
				if ax <= 0 {
					print("OK\n")
				}
				for y:=0 ;y <=3; y = y + 1 {
					a:="xd "
					print(a, "\n")
				}
			}
		}
	} else {
	print("NOTOK\n")
	}
	print("OK\n")

	aloha:=0.0
	for ;aloha <= 20.0; {
		aloha = aloha + 1.0
		print(aloha, "\n")
	}

	print("OK\n")

	print("Zadaj hex int cislo\n")
	puss:=0
	puss, ss_:= inputi()
	print(puss, "\n")
	if ss_ == 1 {
	print("Ziadna hodnota, alebo v zlom formate\n")
	}


	if false {
	
	} else if false {
	
	} else if true {
		print("OK 1\n")
	} else {
	
	}

	if 1<2 && 5< 10 && true {
	print("OK 2\n")
	}

	if (true && true && 1.5 < 2.6) || false {
	print("OK 3\n")
	}

	if 1 > 2 {
	
	} else if !true {
	
	} else if !false {
		print("OK 4\n") 
	}
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

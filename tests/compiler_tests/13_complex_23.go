package main 

func main () {
print("Vypis ordinarnych hodnot z retazca\n")
print("Zadaj retazec\n")
a,_:= inputs()
b:=len(a)
for x:=0 ;x < b; x = x + 1 {
	c,_ :=ord(a, x)
	print(c, "\n")
}


}

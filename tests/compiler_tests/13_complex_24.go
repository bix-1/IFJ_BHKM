package main

func main () {
	print("Vypis znakov od 0 po 255\n")
	for a:=0 ;a <= 255; a = a + 1 {
		x,_ := chr(a)
		print("Znak: ", x, "\n")
	}
	print("Done\n")

}

//Program2:Vypocetfaktorialu(rekurzivne)
package main
func factorial(n int) (int) {
	dec_n := n - 1
	if n < 2 {
		return 1
	}else{
		tmp := 0
		tmp = factorial(dec_n)
		return n * tmp
	}
}
func main() {
	print("Zadejtecisloprovypocetfaktorialu:")
	a := 0
	err := 0
	a, err =inputi()
	if err == 0 {
		if a < 0 {
			print("Faktorialnejdespocitat!","\n")
		}else{
			vysl := 0
			vysl = factorial(a)
			print("Vysledekje", vysl,"\n")
		}
	}else{
		print("Chybaprinacitanicelehocisla!\n")
	}
}

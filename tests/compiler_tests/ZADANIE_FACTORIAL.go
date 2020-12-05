//Program1:Vypocetfaktorialu(iterativne)
package main
func main() {
	print("Zadejtecisloprovypocetfaktorialu:")
	a := 0
	a, _ =inputi()
	if a < 0 {
		print("Faktorialnejdespocitat!\n")
	}else{
		vysl := 1
		for; a > 0; a = a - 1 {
			vysl = vysl * a
		}
		print("Vysledekje", vysl,"\n")
	}
}

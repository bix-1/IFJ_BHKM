package main

func main () {

        a:=true
       	b := false
	
        if  b && 1>2 {
        fmt.Println("FAIL")
        } else if (5 > 4 && a) {
        fmt.Println("OK") // true
        }
}

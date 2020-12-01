package main

func m (n int) (int) {
        dec := n - 1
if n <= 2 {
        return 1 // returns 1 is second call
} else {
a:= 0
a = m(dec) //a = 1 
return n + a // returns 3 + 1 = 4 in first and final call
}
}


func main() {
        a:= 0
        a = m(3)
        print(a) // 4
}

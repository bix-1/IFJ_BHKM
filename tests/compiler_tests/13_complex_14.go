package main

func m (n int) (int) {
        dec := n - 1
if n <= 2 {
        return 1 // returns 1 is third call
} else {
a:= 0
a = m(dec) //

return n * a // returns 3*1 = 3 in second call, returns 4*3 in first call
}
}


func main() {
        a:= 0
        a = m(4)
        print(a) // 12
}
